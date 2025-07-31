/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_WPA2_H
#define _ESP_WPA2_H

#pragma message("esp_wpa2.h is deprecated. Use esp_eap_client.h instead.")

#include "esp_eap_client.h"

/**
  * @brief  Enable wpa2 enterprise authentication.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_wifi_sta_enterprise_enable()` instead.
  *
  * @attention 1. wpa2 enterprise authentication can only be used when station mode is enabled.
  * @attention 2. wpa2 enterprise authentication supports EAP-FAST, TLS, PEAP, TTLS(EAP, MSCHAPv2, MSCHAP, PAP, CHAP) methods.
  *
  * @return
  *    - ESP_OK: succeed.
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_wifi_sta_enterprise_enable' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_enable(void);

/**
  * @brief  Disable wpa2 enterprise authentication.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_wifi_sta_enterprise_disable()` instead.
  *
  * @attention 1. wpa2 enterprise authentication can only be used when station mode is enabled.
  * @attention 2. wpa2 enterprise authentication supports EAP-FAST, TLS, PEAP, TTLS(EAP, MSCHAPv2, MSCHAP, PAP, CHAP) methods.
  *
  * @return
  *    - ESP_OK: succeed.
  */
__attribute__((deprecated("Use 'esp_wifi_sta_enterprise_disable' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_disable(void);

/**
  * @brief  Set identity for PEAP/TTLS method.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_identity` instead.
  *
  * @attention The API only passes the parameter identity to the global pointer variable in wpa2 enterprise module.
  *
  * @param  identity: point to address where stores the identity;
  * @param  len: length of identity, limited to 1~127
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0 or len >= 128)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_identity' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_identity(const unsigned char *identity, int len);

/**
  * @brief  Clear identity for PEAP/TTLS method.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_identity` instead.
  *
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_identity' instead")))
void esp_wifi_sta_wpa2_ent_clear_identity(void);

/**
  * @brief  Set username for PEAP/TTLS method.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_username` instead.
  *
  * @param  username: point to address where stores the username;
  * @param  len: length of username, limited to 1~127
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0 or len >= 128)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_username' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_username(const unsigned char *username, int len);

/**
  * @brief  Clear username for PEAP/TTLS method.
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_username` instead.
  *
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_username' instead")))
void esp_wifi_sta_wpa2_ent_clear_username(void);

/**
  * @brief  Set password for PEAP/TTLS method..
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_password` instead.
  *
  * @param  password: point to address where stores the password;
  * @param  len: length of password(len > 0)
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_password' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_password(const unsigned char *password, int len);

/**
  * @brief  Clear password for PEAP/TTLS method..
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_password` instead.
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_password' instead")))
void esp_wifi_sta_wpa2_ent_clear_password(void);

/**
  * @brief  Set new password for MSCHAPv2 method..
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_new_password` instead.
  *
  * @attention 1. The new password is used to substitute the old password when eap-mschapv2 failure request message with error code ERROR_PASSWD_EXPIRED is received.
  *
  * @param  new_password: point to address where stores the password;
  * @param  len: length of password
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_new_password' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_new_password(const unsigned char *new_password, int len);

/**
  * @brief  Clear new password for MSCHAPv2 method..
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_new_password` instead.
  *
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_new_password' instead")))
void esp_wifi_sta_wpa2_ent_clear_new_password(void);

/**
  * @brief  Set CA certificate for PEAP/TTLS method.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_ca_cert` instead.
  *
  * @attention 1. The API only passes the parameter ca_cert to the global pointer variable in wpa2 enterprise module.
  * @attention 2. The ca_cert should be zero terminated.
  *
  * @param  ca_cert: point to address where stores the CA certificate;
  * @param  ca_cert_len: length of ca_cert
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_set_ca_cert' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len);

/**
  * @brief  Clear CA certificate for PEAP/TTLS method.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_ca_cert` instead.
  *
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_ca_cert' instead")))
void esp_wifi_sta_wpa2_ent_clear_ca_cert(void);

/**
  * @brief  Set client certificate and key.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_certificate_and_key` instead.
  *
  * @attention 1. The API only passes the parameter client_cert, private_key and private_key_passwd to the global pointer variable in wpa2 enterprise module.
  * @attention 2. The client_cert, private_key and private_key_passwd should be zero terminated.
  *
  * @param  client_cert: point to address where stores the client certificate;
  * @param  client_cert_len: length of client certificate;
  * @param  private_key: point to address where stores the private key;
  * @param  private_key_len: length of private key, limited to 1~2048;
  * @param  private_key_password: point to address where stores the private key password;
  * @param  private_key_password_len: length of private key password;
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_set_certificate_and_key' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_cert_key(const unsigned char *client_cert, int client_cert_len,
                                    const unsigned char *private_key, int private_key_len,
                                    const unsigned char *private_key_passwd, int private_key_passwd_len);

/**
  * @brief  Clear client certificate and key.
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_clear_certificate_and_key` instead.
  *
  */
__attribute__((deprecated("Use 'esp_eap_client_clear_certificate_and_key' instead")))
void esp_wifi_sta_wpa2_ent_clear_cert_key(void);

/**
  * @brief  Set wpa2 enterprise certs time check(disable or not).
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_disable_time_check` instead.
  *
  * @param  true: disable wpa2 enterprise certs time check
  * @param  false: enable wpa2 enterprise certs time check
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_set_disable_time_check' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_disable_time_check(bool disable);

/**
  * @brief  Get wpa2 enterprise certs time check(disable or not).
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_get_disable_time_check` instead.
  *
  * @param  disable: store disable value
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_get_disable_time_check' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_get_disable_time_check(bool *disable);

/**
  * @brief  Set wpa2 enterprise ttls phase2 method
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_ttls_phase2_method` instead.
  *
  * @param  type: the type of phase 2 method to be used
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_set_ttls_phase2_method' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(esp_eap_ttls_phase2_types type);

/**
  * @brief  enable/disable 192 bit suite b certification checks
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_suiteb_192bit_certification` instead.
  *
  * @param  enable: bool to enable/disable it.
  *
  * @return
  *    - ESP_OK: succeed
  */
__attribute__((deprecated("Use 'esp_eap_client_set_suiteb_192bit_certification' instead")))
esp_err_t esp_wifi_sta_wpa2_set_suiteb_192bit_certification(bool enable);

/**
  * @brief  Set client pac file
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_pac_file` instead.
  *
  * @attention  1. For files read from the file system, length has to be decremented by 1 byte.
  * @attention  2. Disabling the ESP_WIFI_MBEDTLS_TLS_CLIENT config is required to use EAP-FAST.
  *
  * @param  pac_file: pointer to the pac file
  *         pac_file_len: length of the pac file
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_pac_file' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_pac_file(const unsigned char *pac_file, int pac_file_len);

/**
  * @brief  Set Phase 1 parameters for EAP-FAST
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_set_fast_params` instead.
  *
  * @attention  1. Disabling the ESP_WIFI_MBEDTLS_TLS_CLIENT config is required to use EAP-FAST.
  *
  * @param  config: eap fast phase 1 configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(out of bound arguments)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
__attribute__((deprecated("Use 'esp_eap_client_set_fast_params' instead")))
esp_err_t esp_wifi_sta_wpa2_ent_set_fast_phase1_params(esp_eap_fast_config config);

/**
  * @brief  Use default CA cert bundle for server validation
  *
  * @deprecated This function is deprecated and will be removed in the future.
  *             Please use `esp_eap_client_use_default_cert_bundle` instead.
  *
  * @use_default_bundle : whether to use bundle or not
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: fail
  */
__attribute__((deprecated("Use 'esp_eap_client_use_default_cert_bundle' instead")))
esp_err_t esp_wifi_sta_wpa2_use_default_cert_bundle(bool use_default_bundle);

#ifdef __cplusplus
}
#endif
#endif
