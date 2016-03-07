/*
 * Copyright (c) 2012, 2013 Samsung Electronics Co., Ltd.
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
 */

#ifndef __TIZEN_NET_DNSSD_UTIL_H__
#define __TIZEN_NET_DNSSD_UTIL_H__

#include <dlog.h>
#include <dns_sd.h>
#include <dns-sd.h>

#undef LOG_TAG
#define LOG_TAG "CAPI_NETWORK_DNSSD"

#define DNSSD_LOGD(format, args...) LOGD(format, ##args)
#define DNSSD_LOGI(format, args...) LOGI(format, ##args)
#define DNSSD_LOGW(format, args...) LOGW(format, ##args)
#define DNSSD_LOGE(format, args...) LOGE(format, ##args)

#define __DNSSD_LOG_FUNC_ENTER__ LOGD("Enter")
#define __DNSSD_LOG_FUNC_EXIT__ LOGD("Quit")

#define NETWORK_SERVICE_DISCOVERY_FEATURE	"http://tizen.org/feature/network.service_discovery.dnssd"

#if 0
#define CHECK_FEATURE_SUPPORTED(feature_name)\
	do {\
		bool feature_supported = FALSE;\
		if (!system_info_get_platform_bool(feature_name,\
					&feature_supported)) {\
			if(feature_supported == FALSE){\
				DNSSD_LOGE("%s feature is disabled",\
						feature_name);\
				__DNSSD_LOG_FUNC_EXIT__;\
				return DNSSD_ERROR_NOT_SUPPORTED;\
			}\
		} else {\
			DNSSD_LOGE("Error - Feature getting from System Info");\
			__DNSSD_LOG_FUNC_EXIT__;\
			return DNSSD_ERROR_OPERATION_FAILED;\
		}\
	} while(0)

#else
#define CHECK_FEATURE_SUPPORTED(feature_name)
#endif

#define IPV4_LEN 4
#define IPV6_LEN 16

typedef enum {
	DNSSD_TYPE_UNKNOWN = 0	/**<DNS-SD service type unknown */,
	DNSSD_TYPE_REGISTER = 1	/**<DNS-SD service type register */,
	DNSSD_TYPE_BROWSE = 2	/**<DNS-SD service type browse */,
	DNSSD_TYPE_FOUND = 3	/**<DNS-SD service type found */,
} dnssd_type_e;

typedef struct _dnssd_found_data_s {
	unsigned int browse_handler;
	char *service_name;
	char *txt_record;
	unsigned int txt_len;
	char *host;
	int port;
	unsigned char ip_v4_addr[IPV4_LEN];
	unsigned char ip_v6_addr[IPV6_LEN];
} dnssd_found_data_s;

typedef struct _dnssd_register_data_s {
	DNSRecordRef record_ref;
	TXTRecordRef txt_ref;
	char *service_name;
	char *host;
	int port;
	dnssd_registered_cb callback;
	void *user_data;
} dnssd_register_data_s;

typedef struct _dnssd_browse_data_s {
	dnssd_found_cb callback;
	void *user_data;
} dnssd_browse_data_s;

typedef struct _dnssd_handle_s {
	DNSServiceRef sd_ref;
	dnssd_type_e op_type;
	unsigned int service_handler;
	char *service_type;
	char domain[255];
	unsigned int flags;
	unsigned int if_index;
	char data[0];
} dnssd_handle_s;

typedef struct _resolve_reply_data {
	dnssd_handle_s *dnssd_handle;
	DNSServiceRef sd_ref;
} resolve_reply_data;

#endif /* __TIZEN_NET_DNSSD_UTIL_H__ */
