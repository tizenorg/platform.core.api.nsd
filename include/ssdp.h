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
 * @brief Enumeration for Network Service Discovery SSDP error code.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_ERROR_NONE = TIZEN_ERROR_NONE,  /**< Successful */
	SSDP_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,  /**< Out of memory */
	SSDP_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid function parameter */
	SSDP_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,  /**< Not supported */
	SSDP_ERROR_NOT_INITIALIZED = TIZEN_ERROR_SSDP|0x01,  /**< Not initialized */
	SSDP_ERROR_OPERATION_FAILED = TIZEN_ERROR_SSDP|0x04,  /**< Operation failed */
	SSDP_ERROR_SERVICE_NOT_FOUND = TIZEN_ERROR_SSDP|0x05,  /**< Service not found */
	SSDP_ERROR_SERVICE_ALREADY_REGISTERED = TIZEN_ERROR_SSDP|0x06,  /**< Service already registered */
} ssdp_error_e;

/**
 * @brief Enumeration for Network Service Discovery SSDP service browse state.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_SERVICE_STATE_AVAILABLE,		/**< A new service is available */
	SSDP_SERVICE_STATE_UNAVAILABLE,		/**< A service is not available */
} ssdp_service_state_e;

/**
 * @brief The SSDP service handle
 * @since_tizen 3.0
 */
typedef unsigned int ssdp_service_h;

/**
 * @brief The SSDP browser handle
 * @since_tizen 3.0
 */
typedef unsigned int ssdp_browser_h;

/**
 * @brief Called when the registration of SSDP service is finished.
 * @since_tizen 3.0
 * @param[in] result The result of registration
 * @param[in] ssdp_service The SSDP local service handle
 * @param[in] user_data The user data passed from the request function
 * @see ssdp_register_local_service()
 */
typedef void (*ssdp_registered_cb) (ssdp_error_e result, 
						ssdp_service_h ssdp_local_service, void *user_data);

/**
 * @brief Called when a service has become available or unavailable
 * @since_tizen 3.0
 * @param[in] ssdp_service The SSDP remote service handle
 * @param[in] state The state of found service
 * @param[in] user_data The user data passed from the request function
 * @see ssdp_start_browsing_service()
 */
typedef void (*ssdp_found_cb) (ssdp_service_h ssdp_remote_service,
								ssdp_service_state_e state, void *user_data);

/**
 * @brief Initializes SSDP.
 * @since_tizen 3.0
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_OPERATION_FAILED Operation failed
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_initialize(void);

/**
 * @brief Deinitializes SSDP.
 * @since_tizen 3.0
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_deinitialize(void);

/**
 * @brief Creates a SSDP local service handle.
 * @since_tizen 3.0
 * @remarks You must release @a local_service using ssdp_destroy_local_service().
 * @param[in] target The SSDP local service's target. It may be a device type or a service type specified in UPnP forum (http://upnp.org).
 * @param[out] local_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @see ssdp_destroy_local_service()
 * @pre This API needs ssdp_initialize() before use
 */
int ssdp_create_local_service(const char *target, ssdp_service_h *local_service);

/**
 * @brief Destroys the SSDP local service handle.
 * @details You must call ssdp_deregister_local_service() before destroying the local service.
 * @since_tizen 3.0
 * @param[in] local_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @see ssdp_create_local_service()
 */
int ssdp_destroy_local_service(ssdp_service_h local_service);

/**
 * @brief Sets the USN (Unique Service Name) of SSDP local service. The USN format is specified in UPnP forum (http://upnp.org).
 * @since_tizen 3.0
 * @remarks You must pass only @a unregistered local_service created using ssdp_create_local_service(). If @a local_service is already registered, you cannot set @usn
 * @param[in] local_service The SSDP local service handle
 * @param[in] usn The USN of SSDP local service 
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_service_set_usn(ssdp_service_h local_service, const char *usn);

/**
 * @brief Sets the URL (Uniform Resource Locator) for description of SSDP local service. See RFC 3986.
 * @since_tizen 3.0
 * @remarks You must pass only unregistered @a local_service created using ssdp_create_local_service(). If @a local_service is already registered, you cannot set @url
 * @param[in] local_service The SSDP local service handle
 * @param[in] url The URL of SSDP local service 
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_service_set_url(ssdp_service_h local_service, const char *url);

/**
 * @brief Gets the target of SSDP service.
 * @since_tizen 3.0
 * @remarks You must release @a target using free().
 * @param[in] service The SSDP service handle
 * @param[out] target The target of SSDP service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_service_get_target(ssdp_service_h service, char **target);

/**
 * @brief Gets the USN of SSDP service.
 * @since_tizen 3.0
 * @remarks You must release @a usn using free().
 * @param[in] service The SSDP service handle
 * @param[out] usn The USN of SSDP service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_service_get_usn(ssdp_service_h service, char **usn);

/**
 * @brief Gets the URL of SSDP service.
 * @since_tizen 3.0
 * @remarks You must release @a url using free().
 * @param[in] service The SSDP service handle
 * @param[out] url The URL of SSDP service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_service_get_url(ssdp_service_h service, char **url);

/**
 * @brief Registers the SSDP local service for publishing.
 * @details You must set a USN and a URL of the local service before ssdp_register_local_service() is called.
 * @since_tizen 3.0
 * @remarks You must pass only local_service created using ssdp_create_local_service().
 * @param[in] local_service The SSDP local service handle
 * @param[in] registered_cb The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OPERATION_FAILED Operation failed
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_SERVICE_ALREADY_REGISTERED Service already registered
 * @pre This API needs ssdp_service_set_usn() and ssdp_service_set_url() before use
 */
int ssdp_register_local_service(ssdp_service_h local_service,
			ssdp_registered_cb registered_cb, void *user_data);

/**
 * @brief Deregisters the SSDP local service.
 * @since_tizen 3.0
 * @remarks You must pass only local_service created using ssdp_create_local_service().
 * @param[in] local_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_deregister_local_service(ssdp_service_h local_service);

/**
 * @brief Starts browsing the SSDP remote service.
 * @details ssdp_start_browsing_service() keeps browsing services until calling ssdp_stop_browsing_service()
 * @since_tizen 3.0
 * @param[in] target The target to browse
 * @param[out] ssdp_browser The SSDP browser handle
 * @param[in] found_cb The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OPERATION_FAILED Operation failed
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_start_browsing_service(const char* target, ssdp_browser_h* ssdp_browser,
							ssdp_found_cb found_cb, void *user_data);

/**
 * @brief Stops browsing the SSDP remote service.
 * @since_tizen 3.0
 * @param[in] ssdp_browser The SSDP browser handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 */
int ssdp_stop_browsing_service(ssdp_browser_h ssdp_browser);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_SSDP_H__ */
