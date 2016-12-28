/**
 ******************************************************************************
  Copyright (c) 2014-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */


#pragma once

#include <stdint.h>
#include <time.h>
#include "system_tick_hal.h"
#include "spark_descriptor.h"
#include "events.h"
#include "dsakeygen.h"
#include "eckeygen.h"
#include "file_transfer.h"
#include "protocol_selector.h"
#include "protocol_defs.h"


#ifdef	__cplusplus
extern "C" {
#endif

struct SparkKeys
{
  uint16_t size;
  unsigned char *core_private;
  unsigned char *server_public;
  unsigned char *core_public;
};

STATIC_ASSERT(SparkKeys_size, sizeof(SparkKeys)==16 || sizeof(void*)!=4);

enum ProtocolFactory
{
	PROTOCOL_NONE,
	PROTOCOL_LIGHTSSL,
	PROTOCOL_DTLS,
};

struct SparkCallbacks
{
    uint16_t size;
    /**
     * The type of protocol to instantiate.
     */
    uint8_t protocolFactory;

    uint8_t reserved;

  int (*send)(const unsigned char *buf, uint32_t buflen, void* handle);
  int (*receive)(unsigned char *buf, uint32_t buflen, void* handle);

  /**
   * @param flags 1 dry run only.
   * Return 0 on success.
   */
  int (*prepare_for_firmware_update)(FileTransfer::Descriptor& data, uint32_t flags, void*);

  /**
   *
   * @return 0 on success
   */
  int (*save_firmware_chunk)(FileTransfer::Descriptor& descriptor, const unsigned char* chunk, void*);

  /**
   * Finalize the data storage.
   * #param reset - if the device should be reset to apply the changes.
   * #return 0 on success. Other values indicate an issue with the file.
   */
  int (*finish_firmware_update)(FileTransfer::Descriptor& data, uint32_t flags, void*);

  uint32_t (*calculate_crc)(const unsigned char *buf, uint32_t buflen);

  void (*signal)(bool on, unsigned int param, void* reserved);
  system_tick_t (*millis)();

  /**
   * Sets the time. Time is given in milliseconds since the epoch, UCT.
   */
  void (*set_time)(time_t t, unsigned int param, void* reserved);

  // size == 40

  /**
   * A pointer that is passed back to the send/receive functions.
   */
  void* transport_context;

  // size == 44

  	enum PersistType
	{
  		PERSIST_SESSION = 0
	};
	int (*save)(const void* data, size_t length, uint8_t type, void* reserved);
	/**
	 * Restore to the given buffer. Returns the number of bytes restored.
	 */
	int (*restore)(void* data, size_t max_length, uint8_t type, void* reserved);

	// size == 52
};

STATIC_ASSERT(SparkCallbacks_size, sizeof(SparkCallbacks)==(sizeof(void*)*13));

/**
 * Application-supplied callbacks. (Deliberately distinct from the system-supplied
 * callbacks.)
 */
typedef struct CommunicationsHandlers {
    uint16_t size;

    /**
     * Handle the cryptographically secure random seed from the cloud.
     * @param seed  A random value. This is typically used to seed a pseudo-random generator.
     */
    void (*random_seed_from_cloud)(unsigned int seed);

} CommunicationsHandlers;


STATIC_ASSERT(CommunicationHandlers_size, sizeof(CommunicationsHandlers)==8 || sizeof(void*)!=4);

typedef struct {
    uint16_t size;
    product_id_t product_id;
    product_firmware_version_t product_version;
    uint16_t reserved;  // make the padding explicit
} product_details_t;

STATIC_ASSERT(product_details_size, sizeof(product_details_t)==8);


void spark_protocol_communications_handlers(ProtocolFacade* protocol, CommunicationsHandlers* handlers);

void spark_protocol_init(ProtocolFacade* protocol, const char *id,
          const SparkKeys &keys,
          const SparkCallbacks &callbacks,
          const SparkDescriptor &descriptor, void* reserved=NULL);
int spark_protocol_handshake(ProtocolFacade* protocol, void* reserved=NULL);
bool spark_protocol_event_loop(ProtocolFacade* protocol, void* reserved=NULL);
bool spark_protocol_is_initialized(ProtocolFacade* protocol);
int spark_protocol_presence_announcement(ProtocolFacade* protocol, unsigned char *buf, const unsigned char *id, void* reserved=NULL);
bool spark_protocol_send_event(ProtocolFacade* protocol, const char *event_name, const char *data,
                int ttl, uint32_t flags, void* reserved);
bool spark_protocol_send_subscription_device(ProtocolFacade* protocol, const char *event_name, const char *device_id, void* reserved=NULL);
bool spark_protocol_send_subscription_scope(ProtocolFacade* protocol, const char *event_name, SubscriptionScope::Enum scope, void* reserved=NULL);
bool spark_protocol_add_event_handler(ProtocolFacade* protocol, const char *event_name, EventHandler handler, SubscriptionScope::Enum scope, const char* id, void* handler_data=NULL);
bool spark_protocol_send_time_request(ProtocolFacade* protocol, void* reserved=NULL);
void spark_protocol_send_subscriptions(ProtocolFacade* protocol, void* reserved=NULL);
void spark_protocol_remove_event_handlers(ProtocolFacade* protocol, const char *event_name, void* reserved=NULL);
void spark_protocol_set_product_id(ProtocolFacade* protocol, product_id_t product_id, unsigned int param = 0, void* reserved = NULL);
void spark_protocol_set_product_firmware_version(ProtocolFacade* protocol, product_firmware_version_t product_firmware_version, unsigned int param=0, void* reserved = NULL);
void spark_protocol_get_product_details(ProtocolFacade* protocol, product_details_t* product_details, void* reserved=NULL);

/**
 * Decrypt a buffer using the given public key.
 * @param ciphertext        The ciphertext to decrypt
 * @param private_key       The private key (in DER format).
 * @param plaintext         buffer to hold the resulting plaintext
 * @param max_plaintext_len The size of the plaintext buffer
 * @return The number of plaintext bytes in the plain text buffer, or <0 on error.
 */
extern int decrypt_rsa(const uint8_t* ciphertext, const uint8_t* private_key,
        uint8_t* plaintext, int max_plaintext_len);

void extract_public_rsa_key(uint8_t* device_pubkey, const uint8_t* device_privkey);

/**
 * Retrieves a pointer to a statically allocated instance.
 * @return A statically allocated instance of ProtocolFacade.
 */
extern ProtocolFacade* spark_protocol_instance();

#ifdef	__cplusplus
}
#endif

