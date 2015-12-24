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

/*****************************************************************************
 * 	Standard headers
 *****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

/*****************************************************************************
 * 	System headers
 *****************************************************************************/

#include <dlog.h>

/*****************************************************************************
 * 	Network Service Discovery headers
 *****************************************************************************/

#include "ssdp.h"
#include "ssdp-util.h"
#include "nsd-provider-ssdp.h"

/*****************************************************************************
 * 	Macros and Typedefs
 *****************************************************************************/

/*****************************************************************************
 * 	Global Variables
 *****************************************************************************/

/*****************************************************************************
 * 	Local Functions Definition
 *****************************************************************************/

const char* __print_error(int error)
{
	switch (error)
	{
	case SSDP_ERROR_NOT_PERMITTED:
		return "SSDP_ERROR_NOT_PERMITTED";
		break;
	case SSDP_ERROR_OUT_OF_MEMORY:
		return "SSDP_ERROR_OUT_OF_MEMORY";
		break;
	case SSDP_ERROR_PERMISSION_DENIED:
		return "SSDP_ERROR_PERMISSION_DENIED";
		break;
	case SSDP_ERROR_RESOURCE_BUSY:
		return "SSDP_ERROR_RESOURCE_BUSY";
		break;
	case SSDP_ERROR_INVALID_PARAMETER:
		return "SSDP_ERROR_INVALID_PARAMETER";
		break;
	case SSDP_ERROR_CONNECTION_TIME_OUT:
		return "SSDP_ERROR_CONNECTION_TIME_OUT";
		break;
	case SSDP_ERROR_NOT_SUPPORTED:
		return "SSDP_ERROR_NOT_SUPPORTED";
		break;
	case SSDP_ERROR_NOT_INITIALIZED:
		return "SSDP_ERROR_NOT_INITIALIZED";
		break;
	case SSDP_ERROR_ALREADY_INITIALIZED:
		return "SSDP_ERROR_ALREADY_INITIALIZED";
		break;
	case SSDP_ERROR_OPERATION_FAILED:
		return "SSDP_ERROR_OPERATION_FAILED";
		break;
	case SSDP_ERROR_SERVICE_NOT_FOUND:
		return "SSDP_ERROR_SERVICE_NOT_FOUND";
		break;
	case SSDP_ERROR_SERVICE_DUPLICATED:
		return "SSDP_ERROR_SERVICE_DUPLICATED";
		break;
	case SSDP_ERROR_SSDP_SERVICE_FAILURE:
		return "SSDP_ERROR_SSDP_SERVICE_FAILURE";
		break;
	default:
		break;
	}
	return "Unknown error";
}

int ssdp_initialize()
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	status = nsd_provider_ssdp_initialize();
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_deinitialize()
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	status = nsd_provider_ssdp_deinitialize();
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_create_service(ssdp_type_e op_type, const char *target, ssdp_service_h *ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	status = nsd_provider_ssdp_create_service(op_type, target, ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_destroy_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_destroy_service(ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_set_usn(ssdp_service_h ssdp_service, const char *usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);
	if(ssdp_service == 0 || usn == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);
	SSDP_LOGD("USN [%s]", usn);

	status = nsd_provider_ssdp_set_usn(usn, ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_set_url(ssdp_service_h ssdp_service, const char *url)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0 || url == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);
	SSDP_LOGD("Location [%s]", loc);

	status = nsd_provider_ssdp_set_url(url, ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_target(ssdp_service_h ssdp_service, char **target)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0 || !target) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_get_target(ssdp_service, target);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_usn(ssdp_service_h ssdp_service, char **usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0 || !usn) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_get_usn(ssdp_service, usn);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_url(ssdp_service_h ssdp_service, char **url)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0 || !url) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_get_url(ssdp_service, url);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}


int ssdp_register_service(ssdp_service_h ssdp_service, ssdp_register_cb register_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_register_service(ssdp_service,
			register_cb, user_data);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_deregister_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_deregister_service(ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_browse_service(const char *target, ssdp_service_h *ssdp_service,
			ssdp_browse_cb browse_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(target == NULL || ssdp_service == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP target [%s]", target);

	status = nsd_provider_ssdp_browse_service(target, ssdp_service, browse_cb, user_data);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_stop_browse_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	status = nsd_provider_ssdp_stop_browse_service(ssdp_service);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}
/*
int ssdp_service_foreach_found(ssdp_service_h browse_service,
		ssdp_found_service_cb callback, void * user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if(browse_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP browse service ID [%u]", browse_service);

	status = nsd_provider_ssdp_service_foreach_found(browse_service,
			callback, user_data);
	if(status != SSDP_ERROR_NONE)
		SSDP_LOGE("Print Error [%s]", __print_error(status));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}
*/
