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
 * @brief Enumeration for Network Service Discovery error code.
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

/* DNSSD Operation Types */
typedef enum {
	DNSSD_TYPE_UNKNOWN = 0,
	DNSSD_TYPE_REGISTER = 1,
	DNSSD_TYPE_BROWSE = 2,
} dnssd_type_e;

typedef enum
{
	DNSSD_REGISTERED,
	DNSSD_ALREADY_REGISTERED,
	DNSSD_NAME_CONFLICT,
} dnssd_register_state_e;

typedef enum
{
	DNSSD_SERVICE_AVAILABLE,
	DNSSD_SERVICE_UNAVAILABLE,
	DNSSD_BROWSE_FAILURE,			/** Lookup failure for service name */
	DNSSD_RESOLVE_SERVICE_NAME_FAILURE,	/** Lookup failure for host name and port number */
	DNSSD_RESOLVE_ADDRESS_FAILURE,		/** Lookup failure for IP address */
	
} dnssd_browse_state_e;
/**
 * @brief Enumeration for Network Service Discovery DNSSD service state.
 * @since_tizen 3.0
 */

/* handler */
typedef unsigned int dnssd_service_h;

/* callbacks for publishing */
typedef void (*dnssd_register_cb) (const char *service_name,
		dnssd_register_state_e register_state, void *user_data);

/* callbacks for browsing */
typedef void (*dnssd_browse_cb) (dnssd_browse_state_e browse_state,
		const char *service_name, const char *ip_address,
		const char *txt_record, int txt_len, int port,
		const char *hostname, void *user_data);


int dnssd_initialize();
int dnssd_deinitialize();

/*  Create Service Handle */
int dnssd_create_service(dnssd_type_e operation_type, const char *service_type,
		dnssd_service_h *dnssd_service);
int dnssd_destroy_service(dnssd_service_h dnssd_service);


/* register (publish) */
int dnssd_set_name(dnssd_service_h dnssd_service,
		const char *service_name);
int dnssd_set_port(dnssd_service_h dnssd_service, int port);

int dnssd_get_type(dnssd_service_h dnssd_service,
		char **service_type);
int dnssd_get_name(dnssd_service_h dnssd_service,
		char **service_name);
int dnssd_get_port(dnssd_service_h dnssd_service, int *port);

int dnssd_register_service(dnssd_service_h dnssd_service,
		dnssd_register_cb register_cb, void *user_data);
int dnssd_deregister_service(dnssd_service_h dnssd_service);

int dnssd_add_record(dnssd_service_h dnssd_service, uint16_t rrtype,
		uint16_t rlen, const void *data);
int dnssd_remove_record(dnssd_service_h dnssd_service, uint16_t rrtype);
int dnssd_update_record(dnssd_service_h dnssd_service, uint16_t rrtype,
		uint16_t rlen, const void *data);

/* browse */
int dnssd_browse_service(dnssd_service_h dnssd_service,
		dnssd_browse_cb browse_cb, void *user_data);
int dnssd_stop_browse_service(dnssd_service_h dnssd_service);

/* TXT Record Construction */
int dnssd_set_txt_record_value(dnssd_service_h dnssd_service,
		const char *key, unsigned char value_size, const void *value);
int dnssd_unset_txt_record_value(dnssd_service_h dnssd_service,
		const char *key);
int dnssd_get_txt_record_value(dnssd_service_h  dnssd_service,
		unsigned short *buf_len, const void **buffer);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NET_DNSSD_H__ */
