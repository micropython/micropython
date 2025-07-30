/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of phase 2 authentication types for EAP-TTLS.
 *
 * This enumeration defines the supported phase 2 authentication methods
 * that can be used in the EAP-TTLS (Extensible Authentication Protocol -
 * Tunneled Transport Layer Security) protocol for the second authentication
 * phase.
 */
typedef enum {
    ESP_EAP_TTLS_PHASE2_EAP,        /**< EAP (Extensible Authentication Protocol) */
    ESP_EAP_TTLS_PHASE2_MSCHAPV2,   /**< MS-CHAPv2 (Microsoft Challenge Handshake Authentication Protocol - Version 2) */
    ESP_EAP_TTLS_PHASE2_MSCHAP,     /**< MS-CHAP (Microsoft Challenge Handshake Authentication Protocol) */
    ESP_EAP_TTLS_PHASE2_PAP,        /**< PAP (Password Authentication Protocol) */
    ESP_EAP_TTLS_PHASE2_CHAP        /**< CHAP (Challenge Handshake Authentication Protocol) */
} esp_eap_ttls_phase2_types;

/**
 * @brief Configuration settings for EAP-FAST
 *        (Extensible Authentication Protocol - Flexible Authentication via Secure Tunneling).
 *
 * This structure defines the configuration options that can be used to customize the behavior of the
 * EAP-FAST authentication protocol, specifically for Fast Provisioning and PAC (Protected Access Credential) handling.
 */
typedef struct {
    int fast_provisioning;        /**< Enable or disable Fast Provisioning in EAP-FAST (0 = disabled, 1 = enabled) */
    int fast_max_pac_list_len;    /**< Maximum length of the PAC (Protected Access Credential) list */
    bool fast_pac_format_binary;  /**< Set to true for binary format PAC, false for ASCII format PAC */
} esp_eap_fast_config;


/**
 * @brief Enable EAP authentication(WiFi Enterprise) for the station mode.
 *
 * This function enables Extensible Authentication Protocol (EAP) authentication
 * for the Wi-Fi station mode. When EAP authentication is enabled, the ESP device
 * will attempt to authenticate with the configured EAP credentials when connecting
 * to a secure Wi-Fi network.
 *
 * @note Before calling this function, ensure that the Wi-Fi configuration and EAP
 * credentials (such as username and password) have been properly set using the
 * appropriate configuration APIs.
 *
 * @return
 *    - ESP_OK: EAP authentication enabled successfully.
 *    - ESP_ERR_NO_MEM: Failed to enable EAP authentication due to memory allocation failure.
 */
esp_err_t esp_wifi_sta_enterprise_enable(void);


/**
 * @brief Disable EAP authentication(WiFi Enterprise) for the station mode.
 *
 * This function disables Extensible Authentication Protocol (EAP) authentication
 * for the Wi-Fi station mode. When EAP authentication is disabled, the ESP device
 * will not attempt to authenticate using EAP credentials when connecting to a
 * secure Wi-Fi network.
 *
 * @note Disabling EAP authentication may cause the device to connect to the Wi-Fi
 * network using other available authentication methods, if configured using esp_wifi_set_config().
 *
 * @return
 *    - ESP_OK: EAP authentication disabled successfully.
 *    - ESP_ERR_INVALID_STATE: EAP client is in an invalid state for disabling.
 */
esp_err_t esp_wifi_sta_enterprise_disable(void);

/**
 * @brief Set identity for PEAP/TTLS authentication method.
 *
 * This function sets the identity to be used during PEAP/TTLS authentication.
 *
 * @param[in] identity Pointer to the identity data.
 * @param[in] len      Length of the identity data (limited to 1~127 bytes).
 *
 * @return
 *    - ESP_OK: The identity was set successfully.
 *    - ESP_ERR_INVALID_ARG: Invalid argument (len <= 0 or len >= 128).
 *    - ESP_ERR_NO_MEM: Memory allocation failure.
 */
esp_err_t esp_eap_client_set_identity(const unsigned char *identity, int len);

/**
 * @brief Clear the previously set identity for PEAP/TTLS authentication.
 *
 * This function clears the identity that was previously set for the EAP client.
 * After calling this function, the EAP client will no longer use the previously
 * configured identity during the authentication process.
 */
void esp_eap_client_clear_identity(void);

/**
 * @brief Set username for PEAP/TTLS authentication method.
 *
 * This function sets the username to be used during PEAP/TTLS authentication.
 *
 * @param[in] username Pointer to the username data.
 * @param[in] len      Length of the username data (limited to 1~127 bytes).
 *
 * @return
 *    - ESP_OK: The username was set successfully.
 *    - ESP_ERR_INVALID_ARG: Failed due to an invalid argument (len <= 0 or len >= 128).
 *    - ESP_ERR_NO_MEM: Failed due to memory allocation failure.
 */
esp_err_t esp_eap_client_set_username(const unsigned char *username, int len);

/**
 * @brief Clear username for PEAP/TTLS method.
 *
 * This function clears the previously set username for the EAP client.
 */
void esp_eap_client_clear_username(void);

/**
 * @brief Set password for PEAP/TTLS authentication method.
 *
 * This function sets the password to be used during PEAP/TTLS authentication.
 *
 * @param[in] password Pointer to the password data.
 * @param[in] len      Length of the password data (len > 0).
 *
 * @return
 *    - ESP_OK: The password was set successfully.
 *    - ESP_ERR_INVALID_ARG: Failed due to an invalid argument (len <= 0).
 *    - ESP_ERR_NO_MEM: Failed due to memory allocation failure.
 */
esp_err_t esp_eap_client_set_password(const unsigned char *password, int len);

/**
 * @brief Clear password for PEAP/TTLS method.
 *
 * This function clears the previously set password for the EAP client.
 */
void esp_eap_client_clear_password(void);

/**
 * @brief Set a new password for MSCHAPv2 authentication method.
 *
 * This function sets the new password to be used during MSCHAPv2 authentication.
 * The new password is used to substitute the old password when an eap-mschapv2 failure request
 * message with error code ERROR_PASSWD_EXPIRED is received.
 *
 * @param[in] new_password Pointer to the new password data.
 * @param[in] len          Length of the new password data.
 *
 * @return
 *    - ESP_OK: The new password was set successfully.
 *    - ESP_ERR_INVALID_ARG: Failed due to an invalid argument (len <= 0).
 *    - ESP_ERR_NO_MEM: Failed due to memory allocation failure.
 */
esp_err_t esp_eap_client_set_new_password(const unsigned char *new_password, int len);

/**
 * @brief Clear new password for MSCHAPv2 method.
 *
 * This function clears the previously set new password for the EAP client.
 */
void esp_eap_client_clear_new_password(void);

/**
 * @brief Set CA certificate for EAP authentication.
 *
 * This function sets the Certificate Authority (CA) certificate to be used during EAP authentication.
 * The CA certificate is passed to the EAP client module through a global pointer.
 *
 * @param[in] ca_cert     Pointer to the CA certificate data.
 * @param[in] ca_cert_len Length of the CA certificate data.
 *
 * @return
 *    - ESP_OK: The CA certificate was set successfully.
 */
esp_err_t esp_eap_client_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len);

/**
 * @brief Clear the previously set Certificate Authority (CA) certificate for EAP authentication.
 *
 * This function clears the CA certificate that was previously set for the EAP client.
 * After calling this function, the EAP client will no longer use the previously
 * configured CA certificate during the authentication process.
 */
void esp_eap_client_clear_ca_cert(void);

/**
 * @brief Set client certificate and private key for EAP authentication.
 *
 * This function sets the client certificate and private key to be used during authentication.
 * Optionally, a private key password can be provided for encrypted private keys.
 *
 * @attention 1. The client certificate, private key, and private key password are provided as pointers
 *              to the respective data arrays.
 * @attention 2. The client_cert, private_key, and private_key_password should be zero-terminated.
 *
 * @param[in] client_cert           Pointer to the client certificate data.
 * @param[in] client_cert_len       Length of the client certificate data.
 * @param[in] private_key           Pointer to the private key data.
 * @param[in] private_key_len       Length of the private key data (limited to 1~4096 bytes).
 * @param[in] private_key_password  Pointer to the private key password data (optional).
 * @param[in] private_key_passwd_len Length of the private key password data (can be 0 for no password).
 *
 * @return
 *    - ESP_OK: The certificate, private key, and password (if provided) were set successfully.
 */
esp_err_t esp_eap_client_set_certificate_and_key(const unsigned char *client_cert, int client_cert_len,
                                                 const unsigned char *private_key, int private_key_len,
                                                 const unsigned char *private_key_password, int private_key_passwd_len);

/**
 * @brief Clear the previously set client certificate and private key for EAP authentication.
 *
 * This function clears the client certificate and private key that were previously set
 * for the EAP client. After calling this function, the EAP client will no longer use the
 * previously configured certificate and private key during the authentication process.
 */
void esp_eap_client_clear_certificate_and_key(void);

/**
 * @brief Set EAP client certificates time check (disable or not).
 *
 * This function enables or disables the time check for EAP client certificates.
 * When disabled, the certificates' expiration time will not be checked during the authentication process.
 *
 * @param[in] disable True to disable EAP client certificates time check, false to enable it.
 *
 * @return
 *    - ESP_OK: The EAP client certificates time check setting was updated successfully.
 */
esp_err_t esp_eap_client_set_disable_time_check(bool disable);

/**
 * @brief Get EAP client certificates time check status.
 *
 * This function retrieves the current status of the EAP client certificates time check.
 *
 * @param[out] disable Pointer to a boolean variable to store the disable status.
 *
 * @return
 *    - ESP_OK: The status of EAP client certificates time check was retrieved successfully.
 */
esp_err_t esp_eap_client_get_disable_time_check(bool *disable);

/**
 * @brief Set EAP-TTLS phase 2 method.
 *
 * This function sets the phase 2 method to be used during EAP-TTLS authentication.
 *
 * @param[in] type The type of phase 2 method to be used (e.g., EAP, MSCHAPv2, MSCHAP, PAP, CHAP).
 *
 * @return
 *    - ESP_OK: The EAP-TTLS phase 2 method was set successfully.
 */
esp_err_t esp_eap_client_set_ttls_phase2_method(esp_eap_ttls_phase2_types type);

/**
 * @brief Enable or disable Suite-B 192-bit certification checks.
 *
 * This function enables or disables the 192-bit Suite-B certification checks during EAP-TLS authentication.
 * Suite-B is a set of cryptographic algorithms which generally are considered more secure.
 *
 * @param[in] enable True to enable 192-bit Suite-B certification checks, false to disable it.
 *
 * @return
 *    - ESP_OK: The 192-bit Suite-B certification checks were set successfully.
 */
esp_err_t esp_eap_client_set_suiteb_192bit_certification(bool enable);

/**
 * @brief Set the PAC (Protected Access Credential) file for EAP-FAST authentication.
 *
 * EAP-FAST requires a PAC file that contains the client's credentials.
 *
 * @attention  1. For files read from the file system, length has to be decremented by 1 byte.
 * @attention  2. Disabling the ESP_WIFI_MBEDTLS_TLS_CLIENT config is required to use EAP-FAST.
 *
 * @param[in] pac_file     Pointer to the PAC file buffer.
 * @param[in] pac_file_len Length of the PAC file buffer.
 *
 * @return
 *    - ESP_OK: The PAC file for EAP-FAST authentication was set successfully.
 */
esp_err_t esp_eap_client_set_pac_file(const unsigned char *pac_file, int pac_file_len);

/**
 * @brief Set the parameters for EAP-FAST Phase 1 authentication.
 *
 * EAP-FAST supports Fast Provisioning, where clients can be authenticated faster using precomputed keys (PAC).
 * This function allows configuring parameters for Fast Provisioning.
 *
 * @attention  1. Disabling the ESP_WIFI_MBEDTLS_TLS_CLIENT config is required to use EAP-FAST.
 *
 * @param[in] config Configuration structure with Fast Provisioning parameters.
 *
 * @return
 *    - ESP_OK: The parameters for EAP-FAST Phase 1 authentication were set successfully.
 */
esp_err_t esp_eap_client_set_fast_params(esp_eap_fast_config config);

/**
 * @brief Use the default certificate bundle for EAP authentication.
 *
 * By default, the EAP client uses a built-in certificate bundle for server verification.
 * Enabling this option allows the use of the default certificate bundle.
 *
 * @param[in] use_default_bundle True to use the default certificate bundle, false to use a custom bundle.
 *
 * @return
 *    - ESP_OK: The option to use the default certificate bundle was set successfully.
 */
esp_err_t esp_eap_client_use_default_cert_bundle(bool use_default_bundle);

#ifdef __cplusplus
}
#endif
