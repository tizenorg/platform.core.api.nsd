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
 * @brief Enumeration for Network Service Discovery SSDP service browse state.
 * @since_tizen 3.0
 */
typedef enum
{
	SSDP_SERVICE_AVAILABLE,		/** A new service is available */
	SSDP_SERVICE_UNAVAILABLE,	/** A service is not available */
} ssdp_browse_state_e;

/**
 * @brief The SSDP local service handle
 * @since_tizen 3.0
 */
typedef unsigned int ssdp_local_service_h;

/**
 * @brief The SSDP remote service handle
 * @since_tizen 3.0
 */
typedef unsigned int ssdp_remote_service_h;

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
 * @see ssdp_register_service()
 */
typedef void (*ssdp_registered_cb) (ssdp_error_e result, 
						ssdp_local_service_h ssdp_service, void *user_data);

/**
 * @brief Called when the browsing of SSDP service is finished.
 * @since_tizen 3.0
 * @param[in] ssdp_service The SSDP remote service handle
 * @param[in] state The state of found service
 * @param[in] user_data The user data passed from the request function
 * @see ssdp_browse_service()
 */
typedef void (*ssdp_found_cb) (ssdp_remote_service_h ssdp_service,
								ssdp_browse_state_e state, void *user_data);

/**
 * @brief Initializes SSDP.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_OPERATION_FAILED Operation failed
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_initialize();

/**
 * @brief Deinitializes SSDP.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_deinitialize();

/**
 * @brief Creates a SSDP local service handle.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @remarks You must release @a ssdp_service using ssdp_destroy_service().
 * @param[in] target The SSDP local service's target
 * @param[out] ssdp_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 * @see ssdp_destroy_service()
 * @pre This API needs ssdp_initialize() before use
 */
int ssdp_create_service(const char *target,	ssdp_local_service_h *ssdp_service);

/**
 * @brief Destroys the SSDP local service handle.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 * @see ssdp_create_service()
 */
int ssdp_destroy_service(ssdp_local_service_h ssdp_service);

/**
 * @brief Sets the USN (Unique Service Name) of SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_service The SSDP local service handle
 * @param[in] usn The USN of SSDP local service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_local_service_set_usn(ssdp_local_service_h ssdp_service, const char *usn);

/**
 * @brief Sets the URL (Uniform Resource Locator) of SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_service The SSDP local service handle
 * @param[in] url The URL of SSDP local service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_local_service_set_url(ssdp_local_service_h ssdp_service, const char *url);

/**
 * @brief Gets the target of SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a target using free().
 * @param[in] ssdp_service The SSDP local service handle
 * @param[out] target The target of SSDP local service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_local_service_get_target(ssdp_local_service_h ssdp_service, char **target);

/**
 * @brief Gets the USN of SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a usn using free().
 * @param[in] ssdp_service The SSDP local service handle
 * @param[out] usn The USN of SSDP local service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_local_service_get_usn(ssdp_local_service_h ssdp_service, char **usn);

/**
 * @brief Gets the URL of SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a url using free().
 * @param[in] ssdp_service The SSDP local service handle
 * @param[out] url The URL of SSDP local service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_local_service_get_url(ssdp_local_service_h ssdp_service, char **url);

/**
 * @brief Gets the target of SSDP remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a target using free().
 * @param[in] ssdp_service The SSDP remote service handle
 * @param[out] target The target of SSDP remote service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_remote_service_get_target(ssdp_remote_service_h ssdp_service, char **target);

/**
 * @brief Gets the USN of SSDP remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a usn using free().
 * @param[in] ssdp_service The SSDP remote service handle
 * @param[out] usn The USN of SSDP remote service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_remote_service_get_usn(ssdp_remote_service_h ssdp_service, char **usn);

/**
 * @brief Gets the URL of SSDP remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @details You must release @a url using free().
 * @param[in] ssdp_service The SSDP remote service handle
 * @param[out] url The URL of SSDP remote service
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_remote_service_get_url(ssdp_remote_service_h ssdp_service, char **url);

/**
 * @brief Registers the SSDP local service for publishing.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_service The SSDP local service handle
 * @param[in] registered_cb The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_OPERATION_FAILED Operation failed
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_register_service(ssdp_local_service_h ssdp_service,
			ssdp_registered_cb registered_cb, void *user_data);

/**
 * @brief Deregisters the SSDP local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_service The SSDP local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_deregister_service(ssdp_local_service_h ssdp_service);

/**
 * @brief Starts browsing the SSDP remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
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
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_start_browsing_service(const char* target, ssdp_browser_h* ssdp_browser,
							ssdp_found_cb found_cb, void *user_data);

/**
 * @brief Stops browsing the SSDP remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd 
 * @param[in] ssdp_browser The SSDP browser handle
 * @return 0 on success, otherwise negative error value
 * @retval #SSDP_ERROR_NONE Successful
 * @retval #SSDP_ERROR_NOT_INITIALIZED Not initialized
 * @retval #SSDP_ERROR_SERVICE_NOT_FOUND Service not found
 * @retval #SSDP_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SSDP_ERROR_NOT_SUPPORTED Not supported
 * @retval #SSDP_ERROR_PERMISSION_DENIED Permission Denied
 */
int ssdp_stop_browsing_service(ssdp_browser_h ssdp_browser);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_SSDP_H__ */
