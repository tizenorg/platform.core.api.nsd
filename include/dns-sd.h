#ifndef __TIZEN_NET_DNSSD_H__
#define __TIZEN_NET_DNSSD_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIZEN_ERROR_DNSSD
#define TIZEN_ERROR_DNSSD -0x02F60000
#endif


/**
 * @addtogroup CAPI_NETWORK_DNSSD_MODULE
 * @{
 */

/**
 * @brief Enumeration for Network Service Discovery DNSSD error code.
 * @since_tizen 3.0
 */
typedef enum
{
	DNSSD_ERROR_NONE = TIZEN_ERROR_NONE,  /**< Successful */
	DNSSD_ERROR_NOT_PERMITTED = TIZEN_ERROR_NOT_PERMITTED,  /**< Operation not permitted(1) */
	DNSSD_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,  /**< Out of memory(12) */
	DNSSD_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED,  /**< Permission denied(13) */
	DNSSD_ERROR_RESOURCE_BUSY = TIZEN_ERROR_RESOURCE_BUSY,  /**< Device or resource busy(16) */
	DNSSD_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid function parameter(22) */
	DNSSD_ERROR_CONNECTION_TIME_OUT = TIZEN_ERROR_CONNECTION_TIME_OUT,  /**< Connection timed out(110) */
	DNSSD_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,  /**< Not supported */
	DNSSD_ERROR_NOT_INITIALIZED = TIZEN_ERROR_DNSSD | 0x00FEFFF7,  /**< Not Initialized (-65545) */
	DNSSD_ERROR_ALREADY_REGISTERED = TIZEN_ERROR_DNSSD | 0x00FEFFF5,  /**< Already registered (-65547) */
	DNSSD_ERROR_NAME_CONFLICT = TIZEN_ERROR_DNSSD | 0x00FEFFF4,  /**< Name conflict (-65548) */
	DNSSD_ERROR_SERVICE_NOT_RUNNING = TIZEN_ERROR_DNSSD | 0x00FEFFE5,  /**< Background daemon not running (-65563) */
	DNSSD_ERROR_TIMEOUT = TIZEN_ERROR_DNSSD | 0x00FEFFE0,  /**< Operation timed out (-65568) */
	DNSSD_ERROR_OPERATION_FAILED = TIZEN_ERROR_DNSSD | 0x00FEFFDF,  /**< Operation failed (-65569) */
} dnssd_error_e;

/**
 * @brief Enumeration for Network Service Discovery DNSSD register state.
 * @since_tizen 3.0
 */
typedef enum
{
	DNSSD_REGISTERED	/**<DNS-SD service successfully registered */,
	DNSSD_ALREADY_REGISTERED	/**<DNS-SD service already registered */,
	DNSSD_NAME_CONFLICT	/**<DNS-SD service name conflicted */,
} dnssd_register_state_e;

/**
 * @brief Enumeration for Network Service Discovery DNSSD browse state.
 * @since_tizen 3.0
 */
typedef enum
{
	DNSSD_SERVICE_AVAILABLE			/**<Available DNS-SD service found */,
	DNSSD_SERVICE_UNAVAILABLE		/**<DNS-SD service not available */,
	DNSSD_BROWSE_FAILURE,			/** Lookup failure for service name */
	DNSSD_RESOLVE_SERVICE_NAME_FAILURE,	/** Lookup failure for host name and port number */
	DNSSD_RESOLVE_ADDRESS_FAILURE,		/** Lookup failure for IP address */
} dnssd_browse_state_e;

/**
 * @brief The DNSSD service handle
 * @since_tizen 3.0
 */
typedef unsigned int dnssd_service_h;

/**
 * @brief The DNSSD Browser handle
 * @since_tizen 3.0
 */
typedef unsigned int dnssd_browser_h;

/**
 * @brief Called when the registration of DNSSD service is finished.
 * @since_tizen 3.0
 * @param[in] state The DNSSD service register state
 * @param[in] local_service The DNSSD local service handle
 * @param[in] user_data The user data passed from the request function
 * @see dnssd_register_service()
 */
typedef void (*dnssd_registered_cb) (dnssd_register_state_e register_state,
		dnssd_service_h local_service, void *user_data);

/**
 * @brief Called when a DNSSD service is found.
 * @since_tizen 3.0
 * @param[in] state The DNSSD service browse state
 * @param[in] remote_service The DNSSD remote service handle
 * @param[in] user_data The user data passed from the request function
 * @see dnssd_start_browsing_service()
 */
typedef void (*dnssd_found_cb) (dnssd_browse_state_e browse_state,
		dnssd_service_h remote_service, void *user_data);

/**
 * @brief Initializes DNSSD.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 */
int dnssd_initialize();

/**
 * @brief Deinitializes DNSSD
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @pre DNS-SD service must be initialized by dnssd_initialize().
 */
int dnssd_deinitialize();

/**
 * @brief Creates a DNSSD local service handle.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must release @a dnssd_service using dnssd_destroy_local_service().
 * @param[in] service_type The DNSSD service type. It is expressed as type
 *            followed by protocol, separated by a dot(e.g. "_ftp._tcp"). It
 *            must begin with an underscore, followed by 1-15 characters which
 *            may be letters, digits or hyphens. The transport protocol must be
 *            "_tcp" or "_udp". New service types should be registered at
 *            http://www.dns-sd.org/ServiceTypes.html.
 * @param[out] dnssd_service The DNSSD local handle
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_destroy_service()
 * @pre This API needs dnssd_initialize() before use
 */
int dnssd_create_local_service(const char *service_type,
		dnssd_service_h *dnssd_service);

/**
 * @brief Destroys the DNSSD local service handle.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must destroy only local services created using dnssd_create_local_service().
 * @param[in] dnssd_service The DNSSD local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_destroy_local_service(dnssd_service_h dnssd_service);

/**
 * @brief Sets the name of DNSSD local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] service_name The name of DNSSD local service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_set_name(dnssd_service_h local_service,
		const char *service_name);

/**
 * @brief Sets the port number of DNSSD local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] port The port number of DNSSD local service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_set_port(dnssd_service_h local_service, int port);

/**
 * @brief Sets the TXT record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] key The key of the TXT record
 * @param[in] value_size The length of the value of the TXT record
 * @param[in] value The value of the TXT record
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_set_txt_record_value(dnssd_service_h local_service,
		const char *key, unsigned char value_size, const void *value);

/**
 * @brief Unsets the TXT record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] key The key of the TXT record to be removed
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_unset_txt_record_value(dnssd_service_h local_service,
		const char *key);

/**
 * @brief Adds the resource record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] dnssd_service The DNSSD local service handle
 * @param[in] rrtype The resource record type
 * @param[in] rlen The length of the resource record
 * @param[in] data The data contained in resource record to be added
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_add_record(dnssd_service_h local_service, uint16_t rrtype,
		uint16_t rlen, const void *data);

/**
 * @brief Removes the resource record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] rrtype The resource record type
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_remove_record(dnssd_service_h local_service, uint16_t rrtype);

/**
 * @brief Updates the resource record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] dnssd_service The DNSSD local service handle
 * @param[in] rrtype The resource record type
 * @param[in] rlen The length of the resource record
 * @param[in] data The data contained in resource record to be updated
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_service_update_record(dnssd_service_h local_service, uint16_t rrtype,
		uint16_t rlen, const void *data);

/**
 * @brief Registers the DNSSD local service for publishing.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] register_cb The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 */
int dnssd_register_local_service(dnssd_service_h local_service,
		dnssd_registered_cb register_cb, void *user_data);

/**
 * @brief Deregisters the DNSSD local service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services registered using dnssd_register_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @see dnssd_create_local_service()
 * @see dnssd_register_local_service()
 */
int dnssd_deregister_local_service(dnssd_service_h local_service);

/**
 * @brief Starts browsing the DNSSD remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] service_type The DNSSD service type to browse. It is expressed
 *            as type followed by protocol, separated by a dot(e.g. "_ftp._tcp")
 *            . It must begin with an underscore, followed by 1-15 characters
 *            which may be letters, digits or hyphens. The transport protocol
 *            must be "_tcp" or "_udp". New service types should be registered
 *            at http://www.dns-sd.org/ServiceTypes.html.
 * @param[out] dnssd_service The DNSSD browse service handle
 * @param[in] found_cb The callback function to be called
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_start_browsing_service(const char *service_type,
		dnssd_browser_h dnssd_service, dnssd_found_cb found_cb,
		void *user_data);

/**
 * @brief Stop browsing the DNSSD remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD browse service handle
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_stop_browsing_service(dnssd_browser_h dnssd_service);

/**
 * @brief Gets the type of DNSSD local/remote service.
 * @details You must release @a service_type using free().
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD local/remote service handle
 * @param[out] service_type The type of DNSSD service. It is expressed as type
 *             followed by protocol, separated by a dot(e.g. "_ftp._tcp"). It
 *             must begin with an underscore, followed by 1-15 characters which
 *             may be letters, digits or hyphens. The transport protocol must be
 *             "_tcp" or "_udp". New service types should be registered at
 *             http://www.dns-sd.org/ServiceTypes.html.
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_service_get_type(dnssd_service_h dnssd_service, char **service_type);

/**
 * @brief Gets the name of DNSSD local/remote service.
 * @details You must release @a service_name using free().
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD local/remote service handle
 * @param[out] service_name The name of DNSSD service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_service_get_name(dnssd_service_h dnssd_service, char **service_name);

/**
 * @brief Gets the ip of DNSSD remote service.
 * @details You must release @a ip_v4_address, @a ip_v6_address using free().
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD remote service handle
 * @param[out] ip_v4_address The IP version 4 address of DNSSD service
 * @param[out] ip_v6_address The IP version 6 address of DNSSD service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_service_get_ip(dnssd_service_h dnssd_service, char **ip_v4_address,
		char **ip_v6_address);

/**
 * @brief Gets the port number of DNSSD local/remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD service local/remote handle
 * @param[out] port The port number of DNSSD service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_service_get_port(dnssd_service_h dnssd_service, int *port);

/**
 * @brief Gets the TXT record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD local/remote service handle
 * @param[out] buf_len The length of the TXT record
 * @param[out] buffer The data of the TXT record
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 */
int dnssd_service_get_txt_record_value(dnssd_service_h dnssd_service,
		unsigned short *buf_len, const void **buffer);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_DNSSD_H__ */
