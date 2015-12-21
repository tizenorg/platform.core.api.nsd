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

typedef struct _dnssd_register_handle_s {
	char *service_name;
	DNSRecordRef record_ref;
	TXTRecordRef txt_ref;
	const char *host;
	unsigned short port;
	dnssd_register_cb callback;
	void *user_data;
} dnssd_register_handle_s;

typedef struct _dnssd_browse_handle_s {
	dnssd_browse_cb callback;
	void *user_data;
} dnssd_browse_handle_s;

typedef union {
	dnssd_register_handle_s reg;
	dnssd_browse_handle_s browse;
} dnssd_specific_type_info_u;

typedef struct _dnssd_handle_s {
	DNSServiceRef sd_ref;
	dnssd_type_e op_type;
	char *service_type;
	const char *domain;
	unsigned int flags;
	unsigned int if_index;
	dnssd_specific_type_info_u type_info;
	unsigned int service_handler;
} dnssd_handle_s;

typedef struct _addrinfo_reply_data {
	dnssd_handle_s *dnssd_handle;
	DNSServiceRef sd_ref;
	char *service_name;
	char *txt_record;
	unsigned int txt_len;
	int port;
} addrinfo_reply_data;

typedef struct _resolve_reply_data {
	dnssd_handle_s *dnssd_handle;
	DNSServiceRef sd_ref;
} resolve_reply_data;

#endif /* __TIZEN_NET_DNSSD_UTIL_H__ */
