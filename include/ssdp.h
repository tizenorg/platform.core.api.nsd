/*
 * ssdp
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact:
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef __TIZEN_NET_SSDP_H__
#define __TIZEN_NET_SSDP_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIZEN_ERROR_SSDP
#define TIZEN_ERROR_SSDP -0x02F60000
#endif


/**
 * @addtogroup CAPI_NETWORK_SSDP_MODULE
 * @{
 */

/**
 * @brief Enumeration for Network Service Discovery error code.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_ERROR_NONE = TIZEN_ERROR_NONE,  /**< Successful */
	SSDP_ERROR_NOT_PERMITTED = TIZEN_ERROR_NOT_PERMITTED,  /**< Operation not permitted(1) */
	SSDP_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,  /**< Out of memory(12) */
	SSDP_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED,  /**< Permission denied(13) */
	SSDP_ERROR_RESOURCE_BUSY = TIZEN_ERROR_RESOURCE_BUSY,  /**< Device or resource busy(16) */
	SSDP_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid function parameter(22) */
	SSDP_ERROR_CONNECTION_TIME_OUT = TIZEN_ERROR_CONNECTION_TIME_OUT,  /**< Connection timed out(110) */
	SSDP_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,  /**< Not supported */
	SSDP_ERROR_NOT_INITIALIZED = TIZEN_ERROR_SSDP|0x01,  /**< Not initialized */
	SSDP_ERROR_ALREADY_INITIALIZED = TIZEN_ERROR_SSDP|0x02,  /**< Already initialized */
	SSDP_ERROR_OPERATION_FAILED = TIZEN_ERROR_SSDP|0x04,  /**< Operation failed */
	SSDP_ERROR_SERVICE_NOT_FOUND = TIZEN_ERROR_SSDP|0x05,  /**< Service not found */
	SSDP_ERROR_SERVICE_DUPLICATED = TIZEN_ERROR_SSDP|0x06,  /**< Duplicated service */
	SSDP_ERROR_SSDP_SERVICE_FAILURE = TIZEN_ERROR_SSDP|0x07,  /**< SSDP service failure */
} ssdp_error_e;

/**
 * @brief Enumeration for Network Service Discovery SSDP service type.
 * @since_tizen 3.0
 */

typedef enum
{
	SSDP_TYPE_UNKNOWN,
	SSDP_TYPE_REGISTER,
	SSDP_TYPE_BROWSE,
} ssdp_type_e;


/**
 * @brief Enumeration for Network Service Discovery SSDP service register state.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_REGISTERED,
	SSDP_REGISTER_FAILURE,
} ssdp_register_state_e;


/**
 * @brief Enumeration for Network Service Discovery SSDP service browse state.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_SERVICE_AVAILABLE,
	SSDP_SERVICE_UNAVAILABLE,
} ssdp_browse_state_e;

/* handler */
typedef unsigned int ssdp_service_h;

/* callbacks for publishing */
typedef void (*ssdp_register_cb) (ssdp_service_h ssdp_service,
		ssdp_register_state_e register_state,void *user_data);

/* callbacks for browsing */
typedef void (*ssdp_browse_cb) (ssdp_service_h ssdp_service,
		ssdp_browse_state_e state, void *user_data);


int ssdp_initialize();
int ssdp_deinitialize();


/* register (publish) */
int ssdp_create_service(ssdp_type_e op_type, const char *target,
				ssdp_service_h *ssdp_service);
int ssdp_destroy_service(ssdp_service_h ssdp_service);

/* register (publish) */
int ssdp_set_usn(ssdp_service_h ssdp_service, const char *usn);
int ssdp_set_url(ssdp_service_h ssdp_service, const char *url);

int ssdp_get_target(ssdp_service_h ssdp_service, char **target);
int ssdp_get_usn(ssdp_service_h ssdp_service, char **usn);
int ssdp_get_url(ssdp_service_h ssdp_service, char **url);

int ssdp_register_service(ssdp_service_h ssdp_service, ssdp_register_cb register_cb, void *user_data);
int ssdp_deregister_service(ssdp_service_h ssdp_service);

/* browse */
int ssdp_browse_service(const char *target, ssdp_service_h *ssdp_service,
			ssdp_browse_cb browse_cb, void *user_data);
int ssdp_stop_browse_service(ssdp_service_h ssdp_service);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_SSDP_H__ */
