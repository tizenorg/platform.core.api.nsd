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

#ifndef __ssdp_util_h__
#define __ssdp_util_h__

#define USE_DLOG
#ifdef USE_DLOG
#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "CAPI_NETWORK_SSDP"

#define SSDP_LOGD(format, args...) LOGD(format, ##args) //LCOV_EXCL_LINE
#define SSDP_LOGI(format, args...) LOGI(format, ##args) //LCOV_EXCL_LINE
#define SSDP_LOGW(format, args...) LOGW(format, ##args) //LCOV_EXCL_LINE
#define SSDP_LOGE(format, args...) LOGE(format, ##args) //LCOV_EXCL_LINE

#define __SSDP_LOG_FUNC_ENTER__ LOGD("Enter") //LCOV_EXCL_LINE
#define __SSDP_LOG_FUNC_EXIT__ LOGD("Quit") //LCOV_EXCL_LINE

#else /* USE_DLOG */

#define SSDP_LOGD(format, args...)
#define SSDP_LOGI(format, args...)
#define SSDP_LOGW(format, args...)
#define SSDP_LOGE(format, args...)

#define __SSDP_LOG_FUNC_ENTER__
#define __SSDP_LOG_FUNC_EXIT__

#endif /* USE_DLOG */

#define NETWORK_SERVICE_DISCOVERY_FEATURE "http://tizen.org/feature/network.service_discovery"

#if 0
#define CHECK_FEATURE_SUPPORTED(feature_name)\
	do {\
		bool feature_supported = FALSE;\
		if (!system_info_get_platform_bool(feature_name, &feature_supported)) {\
			if (feature_supported == FALSE) {\
				LOGE("%s feature is disabled", feature_name);\
				return NSD_ERROR_NOT_SUPPORTED;\
			} \
		} else {\
			LOGE("Error - Feature getting from System Info");\
			return NSD_ERROR_OPERATION_FAILED;\
		} \
	} while (0)

#else
#define CHECK_FEATURE_SUPPORTED(feature_name)
#endif

#define GLIST_ITER_START(arg_list, elem) \
		GList *temp = NULL; \
		temp = g_list_first(arg_list); \
		while (temp) {\
			elem = temp->data;\
			temp = g_list_next(temp); \

#define GLIST_ITER_END() }

#endif
