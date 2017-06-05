#include "nrf52_ble.h"
#include "nrf52_board.h"

#include "app_error.h"
#include "app_fifo.h"
#include "app_timer.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_hci.h"
#include "ble_nus.h"
#include "ble_srv_common.h"
#include "fds.h"
#include "fds.h"
#include "fstorage.h"
#include "peer_manager.h"
#include "softdevice_handler.h"


#define CENTRAL_LINK_COUNT          0       /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT       1       /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                 "micropython"

#define MIN_CONN_INTERVAL           MSEC_TO_UNITS(20, UNIT_1_25_MS)   /**< Minimum acceptable connection interval (0.02 seconds). */
#define MAX_CONN_INTERVAL           MSEC_TO_UNITS(200, UNIT_1_25_MS)   /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY               0                                  /**< Slave latency. */
#define CONN_SUP_TIMEOUT            MSEC_TO_UNITS(3000, UNIT_10_MS)    /**< Connection supervisory timeout (3 seconds). */

#define APP_ADV_INTERVAL            MSEC_TO_UNITS(25, UNIT_0_625_MS)
#define APP_ADV_TIMEOUT_IN_SECONDS  180

#define APP_TIMER_PRESCALER              0                                            /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE          4                                            /**< Size of timer operation queues. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)     /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)    /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     3

#define SEC_PARAM_BOND                  1                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                          /**< Maximum encryption key size. */

#define NUS_RX_FIFO_BUFFER_SIZE         64

static ble_uuid_t       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, 0}}; /**< Universally unique service identifiers. */

static ble_nus_t        m_nus;
static app_fifo_t       m_nus_rx_fifo;
static uint8_t          m_nus_rx_fifo_buffer[NUS_RX_FIFO_BUFFER_SIZE];


static void
ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
}

static void
sys_evt_dispatch(uint32_t sys_evt)
{
    fs_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}

static void
ble_stack_init(void)
{
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    uint32_t err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                            PERIPHERAL_LINK_COUNT,
                                                            &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void
gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void
on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            break;
        case BLE_ADV_EVT_IDLE:
            break;
        default:
            break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 */
static void
advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

static void
nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        // XXX
        app_fifo_put(&m_nus_rx_fifo, p_data[i]);
    }
}

static void
services_init(void)
{
    uint32_t err_code;
    ble_nus_init_t nus_init = {0};
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);

    m_adv_uuids[0].type = m_nus.uuid_type;

    err_code = app_fifo_init(&m_nus_rx_fifo, m_nus_rx_fifo_buffer, NUS_RX_FIFO_BUFFER_SIZE);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void
conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
static void
advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void
pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch(p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
            err_code = pm_peer_rank_highest(p_evt->peer_id);
            if (err_code != NRF_ERROR_BUSY)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;//PM_EVT_BONDED_PEER_CONNECTED

        case PM_EVT_CONN_SEC_START:
            break;//PM_EVT_CONN_SEC_START

        case PM_EVT_CONN_SEC_SUCCEEDED:
            {
                NRF_LOG_PRINTF_DEBUG("Link secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                               ble_conn_state_role(p_evt->conn_handle),
                               p_evt->conn_handle,
                               p_evt->params.conn_sec_succeeded.procedure);
                err_code = pm_peer_rank_highest(p_evt->peer_id);
                if (err_code != NRF_ERROR_BUSY)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;//PM_EVT_CONN_SEC_SUCCEEDED

        case PM_EVT_CONN_SEC_FAILED:
            {
                /** In some cases, when securing fails, it can be restarted directly. Sometimes it can
                 *  be restarted, but only after changing some Security Parameters. Sometimes, it cannot
                 *  be restarted until the link is disconnected and reconnected. Sometimes it is
                 *  impossible, to secure the link, or the peer device does not support it. How to
                 *  handle this error is highly application dependent. */
                switch (p_evt->params.conn_sec_failed.error)
                {
                    case PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING:
                        // Rebond if one party has lost its keys.
                        err_code = pm_conn_secure(p_evt->conn_handle, true);
                        if (err_code != NRF_ERROR_INVALID_STATE)
                        {
                            APP_ERROR_CHECK(err_code);
                        }
                        break;//PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING

                    default:
                        break;
                }
            }
            break;//PM_EVT_CONN_SEC_FAILED

        case PM_EVT_CONN_SEC_CONFIG_REQ:
            {
                // Reject pairing request from an already bonded peer.
                pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
                pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
            }
            break;//PM_EVT_CONN_SEC_CONFIG_REQ

        case PM_EVT_STORAGE_FULL:
            {
                // Run garbage collection on the flash.
                err_code = fds_gc();
                if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
                {
                    // Retry.
                }
                else
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;//PM_EVT_STORAGE_FULL

        case PM_EVT_ERROR_UNEXPECTED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
            break;//PM_EVT_ERROR_UNEXPECTED

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            break;//PM_EVT_PEER_DATA_UPDATE_SUCCEEDED

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            // Assert.
            APP_ERROR_CHECK_BOOL(false);
            break;//PM_EVT_PEER_DATA_UPDATE_FAILED

        case PM_EVT_PEER_DELETE_SUCCEEDED:
            break;//PM_EVT_PEER_DELETE_SUCCEEDED

        case PM_EVT_PEER_DELETE_FAILED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
            break;//PM_EVT_PEER_DELETE_FAILED

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start();
            break;//PM_EVT_PEERS_DELETE_SUCCEEDED

        case PM_EVT_PEERS_DELETE_FAILED:
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
            break;//PM_EVT_PEERS_DELETE_FAILED

        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
            break;//PM_EVT_LOCAL_DB_CACHE_APPLIED

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
            break;//PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED

        case PM_EVT_SERVICE_CHANGED_IND_SENT:
            break;//PM_EVT_SERVICE_CHANGED_IND_SENT

        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
            break;//PM_EVT_SERVICE_CHANGED_IND_CONFIRMED

        default:
            // No implementation needed.
            break;
    }
}

static void
peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond              = SEC_PARAM_BOND;
    sec_param.mitm              = SEC_PARAM_MITM;
    sec_param.lesc              = SEC_PARAM_LESC;
    sec_param.keypress          = SEC_PARAM_KEYPRESS;
    sec_param.io_caps           = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob               = SEC_PARAM_OOB;
    sec_param.min_key_size      = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size      = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc     = 1;
    sec_param.kdist_own.id      = 1;
    sec_param.kdist_peer.enc    = 1;
    sec_param.kdist_peer.id     = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

static void
timers_init()
{
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
}

void
nrf52_ble_init(void)
{
    timers_init();
    ble_stack_init();
    peer_manager_init(false);
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();

    advertising_start();
}

static void
power_manage()
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);    
}

// ########################### MP IO functions ###########################

void
mp_hal_stdout_tx_strn(const char *str, size_t len)
{
    uint32_t err_code;
    uint8_t *buf = (uint8_t *)str;
    size_t send_len;

    while (len > 0) {
        if (len >= BLE_NUS_MAX_DATA_LEN)
            send_len = BLE_NUS_MAX_DATA_LEN;
        else
            send_len = len;
        err_code = ble_nus_string_send(&m_nus, buf, send_len);
        if (err_code == NRF_SUCCESS) {
            len -= send_len;
            buf += send_len;
        } else if (err_code != NRF_ERROR_INVALID_STATE) {
            APP_ERROR_CHECK(err_code);
        }
    }
}

int
mp_hal_stdin_rx_chr()
{
    uint8_t byte;
    for (;;) {
        if (app_fifo_get(&m_nus_rx_fifo, &byte) == NRF_SUCCESS) {
            return byte;
        }
        power_manage();
    }
}
