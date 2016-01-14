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
	DNSSD_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,  /**< Out of memory(12) */
	DNSSD_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED,  /**< Permission denied(13) */
	DNSSD_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,  /**< Invalid function parameter(22) */
	DNSSD_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,  /**< Not supported */
	DNSSD_ERROR_NOT_INITIALIZED = TIZEN_ERROR_DNSSD | 0x00FEFFF7,  /**< Not Initialized (-65545) */
	DNSSD_ERROR_ALREADY_REGISTERED = TIZEN_ERROR_DNSSD | 0x00FEFFF5,  /**< Already registered (-65547) */
	DNSSD_ERROR_NAME_CONFLICT = TIZEN_ERROR_DNSSD | 0x00FEFFF4,  /**< Name conflict (-65548) */
	DNSSD_ERROR_SERVICE_NOT_RUNNING = TIZEN_ERROR_DNSSD | 0x00FEFFE5,  /**< Background daemon not running (-65563) */
	DNSSD_ERROR_OPERATION_FAILED = TIZEN_ERROR_DNSSD | 0x00FEFFDF,  /**< Operation failed (-65569) */
} dnssd_error_e;

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
 * @param[in] result The result of registration
 * @param[in] local_service The DNSSD local service handle
 * @param[in] user_data The user data passed from the request function
 * @result #DNSSD_ERROR_NONE Successful
 * @result #DNSSD_ERROR_NAME_CONFLICT Name Conflict
 * @result #DNSSD_ERROR_ALREADY_REGISTERED Already Registered
 * @see dnssd_register_service()
 */
typedef void (*dnssd_registered_cb) (dnssd_error_e result,
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
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
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
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
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
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @see dnssd_destroy_service()
 * @pre This API needs dnssd_initialize() before use
 */
int dnssd_create_local_service(const char *service_type,
		dnssd_service_h *dnssd_service);

/**
 * @brief Destroys the DNSSD local service handle.
 * @details Destroying a local service doesn't deregister local service. If
 *          local service was registered using dnssd_register_local_service()
 *          then it must be deregistered using dnssd_deregister_local_service()
 *          before destroying the local service.
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
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
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
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
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
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @see dnssd_create_local_service()
 */
int dnssd_service_set_port(dnssd_service_h local_service, int port);

/**
 * @brief Adds the TXT record.
 * @details TXT record gives additional information about the service. Some
 *          services discovered via dnssd_start_browsing_service() may need
 *          more than just IP address and port number to completely identify
 *          the service instance. For example, A web server typically has
 *          multiple pages, each identified by its own URL. So additional data
 *          is stored in a TXT record. Check Section 6 of
 *          http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt for
 *          details. TXT record of known service types can be found at
 *          http://www.dns-sd.org/ServiceTypes.html. TXT record is stored in a
 *          structured form using key/value pairs.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] key The key of the TXT record. A null-terminated string which only
 *            contains printable ASCII values (0x20-0x7E), excluding '=' (0x3D).
 *            Keys should be 9 characters or fewer excluding NULL. Keys are case
 *            insensitivie. Keys for known service types can be found at
 *            http://www.dns-sd.org/ServiceTypes.html. For details see section
 *            6.4 of http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt.
 * @param[in] length The length of the value of the TXT record in bytes. The
 *            total size of a typical DNS-SD TXT record is intended to be small
 *            (upto 200 bytes). For details see section 6.2 of
 *            http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt.
 * @param[in] value The value of the TXT record. It can be any binary value. For
 *            value that represent textual data, UTF-8 is STRONGLY recommended.
 *            For value that represent textual data, value_length should NOT
 *            include the terminating null (if any) at the end of the string. If
 *            NULL, then "key" will be added with no value. If non-NULL but
 *            value_length is zero, then "key=" will be added with empty value.
 *            For details see section 6.5 of
 *            http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt.
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_ALREADY_REGISTERED Already Registered
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @see dnssd_create_local_service()
 */
int dnssd_service_add_txt_record_value(dnssd_service_h local_service,
		const char *key, unsigned short length, const void *value);

/**
 * @brief Removes the TXT record.
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
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @see dnssd_create_local_service()
 * @see dnssd_service_add_txt_record_value()
 */
int dnssd_service_remove_txt_record_value(dnssd_service_h local_service,
		const char *key);

/**
 * @brief Adds the DNS resource record. If the resource record has for the type
 *        has been previously added using dnssd_service_set_record() then the
 *        record will be updated.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] type The resource record type. For details see
 *            https://en.wikipedia.org/wiki/List_of_DNS_record_types.
 * @param[in] length The length of the resource record in bytes.
 * @param[in] data The data contained in resource record to be added
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_ALREADY_REGISTERED Already Registered
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @see dnssd_create_local_service()
 */
int dnssd_service_set_record(dnssd_service_h local_service, unsigned short type,
		unsigned short length, const void *data);

/**
 * @brief Removes the DNS resource record.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must pass only local services created using dnssd_create_local_service().
 * @param[in] local_service The DNSSD local service handle
 * @param[in] type The resource record type. For details see
 *            https://en.wikipedia.org/wiki/List_of_DNS_record_types.
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_OPERATION_FAILED Operation failed
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @see dnssd_create_local_service()
 */
int dnssd_service_unset_record(dnssd_service_h local_service,
		unsigned short type);

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
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
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
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 * @see dnssd_create_local_service()
 * @see dnssd_register_local_service()
 */
int dnssd_deregister_local_service(dnssd_service_h local_service);

/**
 * @brief Starts browsing the DNSSD remote service.
 * @details found_cb would be called only if there are any service available of
 *          service_type provided in the argument. Application will keep
 *          browsing for available/unavailable will services until it calls
 *          dnssd_stop_browsing_service().
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
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_start_browsing_service(const char *service_type,
		dnssd_browser_h *dnssd_service, dnssd_found_cb found_cb,
		void *user_data);

/**
 * @brief Stop browsing the DNSSD remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @param[in] dnssd_service The DNSSD browse service handle returned by
 *            dnssd_start_browsing_service().
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_stop_browsing_service(dnssd_browser_h dnssd_service);

/**
 * @brief Gets the type of DNSSD local/remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must release @a service_type using free().
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
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_service_get_type(dnssd_service_h dnssd_service, char **service_type);

/**
 * @brief Gets the name of DNSSD local/remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must release @a service_name using free().
 * @param[in] dnssd_service The DNSSD local/remote service handle
 * @param[out] service_name The name of DNSSD service
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_service_get_name(dnssd_service_h dnssd_service, char **service_name);

/**
 * @brief Gets the ip of DNSSD remote service.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must release @a ip_v4_address, @a ip_v6_address using free().
 * @param[in] dnssd_service The DNSSD remote service handle
 * @param[out] ip_v4_address The IP version 4 address of DNSSD service. If there
 *             are no IPv4 Address then it would contain NULL and should not be
 *             freed.
 * @param[out] ip_v6_address The IP version 6 address of DNSSD service. If there
 *             are no IPv6 Address then it would contain NULL and should not be
 *             freed.
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
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
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_service_get_port(dnssd_service_h dnssd_service, int *port);

/**
 * @brief Gets the TXT record.
 * @details TXT record gives additional information about the service. Some
 *          services discovered via dnssd_start_browsing_service() may need
 *          more than just IP address and port number to completely identify
 *          the service instance. For example, A web server typically has
 *          multiple pages, each identified by its own URL. So additional data
 *          is stored in a TXT record. Check Section 6 of
 *          http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt for
 *          details. TXT record of known service types can be found at
 *          http://www.dns-sd.org/ServiceTypes.html. TXT record is stored in a
 *          structured form using key/value pairs.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege http://tizen.org/privilege/nsd
 * @remarks You must release @value using free().
 * @param[in] dnssd_service The DNSSD local/remote service handle
 * @param[out] length The length of the value of the TXT record in bytes.
 * @param[out] value The value of the TXT record.
 * @return 0 on success, otherwise negative error value
 * @retval #DNSSD_ERROR_NONE Successful
 * @retval #DNSSD_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DNSSD_ERROR_NOT_SUPPORTED Not Supported
 * @retval #DNSSD_ERROR_PERMISSION_DENIED Permission Denied
 * @retval #DNSSD_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval #DNSSD_ERROR_SERVICE_NOT_RUNNING Service Not Running
 * @retval #DNSSD_ERROR_NOT_INITIALIZED Not Initialized
 */
int dnssd_service_get_txt_record_value(dnssd_service_h dnssd_service,
		unsigned short *length, void **value);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_DNSSD_H__ */
