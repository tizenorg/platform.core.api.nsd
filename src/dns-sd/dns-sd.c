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

#include <net/if.h>
#include <dlog.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <dns_sd.h>
#include "dns-sd-util.h"
#include "dns-sd.h"

#define GET_REG_DATA_P(a) ((dnssd_register_data_s *)((a)->data))
#define GET_BROWSE_DATA_P(a) ((dnssd_browse_data_s *)((a)->data))
#define GET_FOUND_DATA_P(a) ((dnssd_found_data_s *)((a)->data))

#define REG_SIZE (sizeof(dnssd_handle_s) + sizeof(dnssd_register_data_s))
#define BROWSE_SIZE (sizeof(dnssd_handle_s) + sizeof(dnssd_browse_data_s))
#define FOUND_SIZE (sizeof(dnssd_handle_s) + sizeof(dnssd_found_data_s))

static __thread GSList *dnssd_handle_list = NULL;
static __thread GSList *resolve_handle_list = NULL;

static const char *dnssd_error_to_string(dnssd_error_e error)
{
	switch (error) {
	case DNSSD_ERROR_NONE:
		return "DNSSD_ERROR_NONE";
	case DNSSD_ERROR_NOT_PERMITTED:
		return "DNSSD_ERROR_NOT_PERMITTED";
	case DNSSD_ERROR_OUT_OF_MEMORY:
		return "DNSSD_ERROR_OUT_OF_MEMORY";
	case DNSSD_ERROR_PERMISSION_DENIED:
		return "DNSSD_ERROR_PERMISSION_DENIED";
	case DNSSD_ERROR_RESOURCE_BUSY:
		return "DNSSD_ERROR_RESOURCE_BUSY";
	case DNSSD_ERROR_INVALID_PARAMETER:
		return "DNSSD_ERROR_INVALID_PARAMETER";
	case DNSSD_ERROR_CONNECTION_TIME_OUT:
		return "DNSSD_ERROR_CONNECTION_TIME_OUT";
	case DNSSD_ERROR_NOT_SUPPORTED:
		return "DNSSD_ERROR_NOT_SUPPORTED";
	case DNSSD_ERROR_NOT_INITIALIZED:
		return "DNSSD_ERROR_NOT_INITIALIZED";
	case DNSSD_ERROR_ALREADY_REGISTERED:
		return "DNSSD_ERROR_ALREADY_REGISTERED";
	case DNSSD_ERROR_NAME_CONFLICT:
		return "DNSSD_ERROR_NAME_CONFLICT";
	case DNSSD_ERROR_SERVICE_NOT_RUNNING:
		return "DNSSD_ERROR_SERVICE_NOT_RUNNING";
	case DNSSD_ERROR_TIMEOUT:
		return "DNSSD_ERROR_TIMEOUT";
	case DNSSD_ERROR_OPERATION_FAILED:
		return "DNSSD_ERROR_OPERATION_FAILED";
	default:
		return "UNSUPPORTED_ERROR";
	}
}

int dnssd_initialize()
{
	__DNSSD_LOG_FUNC_ENTER__;
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_deinitialize()
{
	__DNSSD_LOG_FUNC_ENTER__;
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

static dnssd_handle_s *__dnssd_check_handle_validity(
		dnssd_service_h dnssd_service)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;
	GSList *list;

	for (list = dnssd_handle_list; list; list = list->next) {
		local_handle = (dnssd_handle_s *)list->data;
		if (local_handle && local_handle->service_handler ==
				dnssd_service) {
			DNSSD_LOGD("Service found with handler %u",
					dnssd_service);
			__DNSSD_LOG_FUNC_EXIT__;
			return local_handle;
		}
	}

	__DNSSD_LOG_FUNC_EXIT__;
	return NULL;
}

int dnssd_create_service(dnssd_type_e operation_type, const char *service_type,
		dnssd_service_h *dnssd_service)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle;
	unsigned int handler;
	if(dnssd_service == NULL || service_type == NULL ||
			__dnssd_check_handle_validity(*dnssd_service) != NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if(operation_type == DNSSD_TYPE_REGISTER) {
		local_handle = (dnssd_handle_s *)g_try_malloc0(REG_SIZE);
	} else if (operation_type == DNSSD_TYPE_BROWSE) {
		local_handle = (dnssd_handle_s *)g_try_malloc0(BROWSE_SIZE);
	} else {
		DNSSD_LOGE("Invalid Operation Type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle == NULL) {
		DNSSD_LOGE("Failed to Allocate Memory");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OUT_OF_MEMORY;
	}

	handler = (unsigned int)local_handle & 0xffffffff;
	*dnssd_service = handler;
	local_handle->service_handler = handler;
	local_handle->op_type = operation_type;
	local_handle->service_type = g_strdup(service_type);
	g_strlcpy(local_handle->domain, "", sizeof(local_handle->domain));
	local_handle->flags = 0;
	local_handle->if_index = kDNSServiceInterfaceIndexAny;
	DNSSD_LOGD("New handle created [%p]->[%u] type %s", local_handle,
			*dnssd_service, local_handle->service_type);

	dnssd_handle_list = g_slist_prepend(dnssd_handle_list, local_handle);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_destroy_service(dnssd_service_h dnssd_service)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_BROWSE &&
			local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	DNSSD_LOGD("Destroy handle: [%p]->[%u]", local_handle, dnssd_service);

	dnssd_handle_list = g_slist_remove(dnssd_handle_list, local_handle);
	g_free(local_handle->service_type);
	if (local_handle->op_type == DNSSD_TYPE_REGISTER) {
		dnssd_register_data_s *reg = GET_REG_DATA_P(local_handle);
		g_free(reg->service_name);
	}
	g_free(local_handle);
	local_handle = NULL;

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_set_name(dnssd_service_h dnssd_service,
		const char *service_name)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg = NULL;

	if (service_name == NULL) {
		DNSSD_LOGD("Service name is NULL");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	if(reg->service_name != NULL)
		g_free(reg->service_name);
	reg->service_name = g_strdup(service_name);

	DNSSD_LOGD("Successfully set service name %s", service_name);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_set_port(dnssd_service_h dnssd_service, int port)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg = NULL;

	if (port < 0 || port > 65535) {
		DNSSD_LOGD("Invalid port range");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	reg->port = port;

	DNSSD_LOGD("Successfully set port %d", port);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_get_type(dnssd_service_h dnssd_service, char **service_type)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;

	if (service_type == NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	} else {
		*service_type = g_strdup(local_handle->service_type);
		DNSSD_LOGD("Service Type %s", *service_type);
	}

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_get_name(dnssd_service_h dnssd_service, char **service_name)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;

	if (service_name == NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if(local_handle->op_type == DNSSD_TYPE_FOUND) {
		dnssd_found_data_s *found = NULL;
		found = GET_FOUND_DATA_P(local_handle);
		*service_name = g_strdup(found->service_name);
	} else if(local_handle->op_type == DNSSD_TYPE_REGISTER) {
		dnssd_register_data_s *reg = NULL;
		reg = GET_REG_DATA_P(local_handle);
		*service_name = g_strdup(reg->service_name);
	} else {
		*service_name = NULL;
	}

	DNSSD_LOGD("Service Name %s", *service_name);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}


int dnssd_get_ip(dnssd_service_h dnssd_service,
		char **ip_v4_address, char **ip_v6_address)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;
	dnssd_found_data_s *found = NULL;
	unsigned char *addr = NULL;

	if (ip_v4_address == NULL && ip_v6_address == NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if(local_handle->op_type != DNSSD_TYPE_FOUND) {
		DNSSD_LOGD("Invalid Operation Type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	found = GET_FOUND_DATA_P(local_handle);

	addr = found->ip_v4_addr;
	*ip_v4_address = g_strdup_printf("%d.%d.%d.%d",
			addr[0], addr[1], addr[2], addr[3]);

	addr = found->ip_v6_addr;
	*ip_v6_address = g_strdup_printf("%02X%02X:"
			"%02X%02X:%02X%02X:%02X%02X:"
			"%02X%02X:%02X%02X:%02X%02X:"
			"%02X%02X", addr[0], addr[1],
			addr[2], addr[3], addr[4],
			addr[5], addr[6], addr[7],
			addr[8], addr[9], addr[10],
			addr[11], addr[12], addr[14],
			addr[14], addr[15]);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_get_port(dnssd_service_h dnssd_service, int *port)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;

	if (port == NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if(local_handle->op_type == DNSSD_TYPE_FOUND) {
		dnssd_found_data_s *found = NULL;
		found = GET_FOUND_DATA_P(local_handle);
		*port = found->port;
	} else if(local_handle->op_type == DNSSD_TYPE_REGISTER) {
		dnssd_register_data_s *reg = NULL;
		reg = GET_REG_DATA_P(local_handle);
		*port = reg->port;
	} else {
		*port = 0;
	}
	DNSSD_LOGD("Port %d", *port);

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

static void __dnssd_register_reply_cb(DNSServiceRef sd_ref, unsigned int flags,
		DNSServiceErrorType error_code, const char *service_name,
		const char *service_type, const char *domain, void *user_data)
{
	dnssd_handle_s *dnssd_handle;
	dnssd_register_data_s *reg = NULL;
	GSList *list;
	dnssd_register_cb callback;
	void *data;

	DNSSD_LOGD("Received Registration Reply");

	if (error_code != DNSSD_ERROR_NONE) {
		DNSSD_LOGE("Failed to register, error code %s",
				dnssd_error_to_string(error_code));
		return;
	}

	for (list = dnssd_handle_list; list; list = list->next) {
		dnssd_handle = (dnssd_handle_s *) list->data;
		if (dnssd_handle->sd_ref == sd_ref) {
			DNSSD_LOGD("Registration Callback set for [%p]",
					dnssd_handle);
			/* Update domain and flags for any future use */
			g_strlcpy(dnssd_handle->domain, domain,
					sizeof(dnssd_handle->domain));
			dnssd_handle->flags = flags;
			DNSSD_LOGD("Domain/Interface/Flags : %s/%d/0x%x",
					dnssd_handle->domain,
					dnssd_handle->if_index,
					dnssd_handle->flags);

			reg = GET_REG_DATA_P(dnssd_handle);
			callback = reg->callback;
			data = reg->user_data;
			if (callback == NULL) {
				DNSSD_LOGD("Callback is not set");
				return;
			}
			if (error_code == DNSSD_ERROR_NAME_CONFLICT)
				callback(DNSSD_NAME_CONFLICT, dnssd_handle->service_handler,
						data);
			else if (error_code == DNSSD_ERROR_ALREADY_REGISTERED)
				callback(DNSSD_ALREADY_REGISTERED, dnssd_handle->service_handler,
						data);
			else if (error_code == DNSSD_ERROR_NONE)
				callback(DNSSD_REGISTERED, dnssd_handle->service_handler,
						data);
			else
				DNSSD_LOGE("Failed to register, error code %s",
					dnssd_error_to_string(error_code));
		}
	}
}

int dnssd_register_service(dnssd_service_h dnssd_service,
		dnssd_register_cb register_cb, void *user_data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg = NULL;
	DNSServiceRef *sd_ref;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (register_cb == NULL) {
		DNSSD_LOGE("No callback Provided");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	sd_ref = &(local_handle->sd_ref);

	if (reg->service_name == NULL) {
		DNSSD_LOGD("No Service Name Provided");
		reg->service_name = "";
	} else {
		DNSSD_LOGD("Service Name is %s", reg->service_name);
	}

	DNSSD_LOGD("Domain/Interface/Flags : %s/%d/0x%x", local_handle->domain,
			local_handle->if_index, local_handle->flags);

	ret = DNSServiceRegister(sd_ref, local_handle->flags,
			local_handle->if_index, reg->service_name,
			local_handle->service_type, local_handle->domain,
			NULL, reg->port, 1, "", __dnssd_register_reply_cb,
			NULL);
	if(ret < 0) {
		DNSSD_LOGE("Failed to register for dns service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OPERATION_FAILED;
	}

	reg->callback = register_cb;
	reg->user_data = user_data;
	DNSServiceHandleEvents(sd_ref);

	DNSSD_LOGD("Succeeded to register for dns service");
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_deregister_service(dnssd_service_h dnssd_service)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle = NULL;
	DNSServiceRef sd_ref;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	sd_ref = local_handle->sd_ref;
	if(NULL == sd_ref) {
		DNSSD_LOGE("Invalid DNS SD Client");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	DNSServiceRefDeallocate(sd_ref);
	sd_ref = NULL;
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_add_record(dnssd_service_h dnssd_service, uint16_t rrtype,
		uint16_t rlen, const void *data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg;
	DNSServiceRef sd_ref;
	DNSRecordRef *record_client;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	sd_ref = local_handle->sd_ref;
	record_client = &(reg->record_ref);
	DNSSD_LOGD("Record Type %d Record len %d", rrtype, rlen);

	ret = DNSServiceAddRecord(sd_ref, record_client, local_handle->flags,
			rrtype, rlen, data, 0);
	if(ret < 0) {
		DNSSD_LOGE("Failed to Add Record for DNS Service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OPERATION_FAILED;
	}

	DNSSD_LOGD("Successfully added record for DNS Service");

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_update_record(dnssd_service_h dnssd_service, uint16_t rrtype,
		uint16_t rlen, const void *data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg;
	DNSServiceRef sd_ref;
	DNSRecordRef record_client;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	sd_ref = local_handle->sd_ref;
	record_client = reg->record_ref;

	if (NULL == record_client)
		DNSSD_LOGD("Uninitialized DNS Service Record, updating "
				"service's primary TXT Record");

	ret = DNSServiceUpdateRecord(sd_ref, record_client, local_handle->flags,
			rlen, data, 0);
	if(ret < 0) {
		DNSSD_LOGE("Failed to Update Record for DNS Service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OPERATION_FAILED;
	}

	DNSSD_LOGD("Successfully updated registered record for DNS Service");

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_remove_record(dnssd_service_h dnssd_service, uint16_t rrtype)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	dnssd_handle_s *local_handle;
	dnssd_register_data_s *reg;
	DNSServiceRef sd_ref;
	DNSRecordRef record_client;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	sd_ref = local_handle->sd_ref;
	record_client = reg->record_ref;

	if(NULL == record_client) {
		DNSSD_LOGE("Invalid DNS SD Client");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	ret = DNSServiceRemoveRecord(sd_ref, record_client, local_handle->flags);
	if(ret < 0) {
		DNSSD_LOGE("Failed to Remove Record for DNS Service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OPERATION_FAILED;
	}

	DNSSD_LOGD("Successfully removed record for DNS Service");

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

static void __dnssd_getaddrinfo_reply_cb(DNSServiceRef sd_ref,
		unsigned int flags, unsigned int if_index,
		DNSServiceErrorType error_code, const char *host_name,
		const struct sockaddr *address, unsigned int ttl,
		void *user_data)
{
	dnssd_handle_s *dnssd_handle = NULL;
	dnssd_handle_s *local_handle = NULL;
	dnssd_browse_data_s *browse = NULL;
	dnssd_found_data_s *found = NULL;
	dnssd_browse_cb callback;
	void *data;

	if(user_data == NULL) {
		DNSSD_LOGD("Invalid found handle");
		__DNSSD_LOG_FUNC_EXIT__;
		return;
	}

	local_handle = (dnssd_handle_s *)user_data;
	found = GET_FOUND_DATA_P(local_handle);
	/* Update interface and flags for any future use */
	local_handle->if_index = if_index;
	local_handle->flags = flags;
	DNSSD_LOGD("Domain/Interface/Flags : %s/%d/0x%x",
			local_handle->domain,
			local_handle->if_index,
			local_handle->flags);

	dnssd_handle = __dnssd_check_handle_validity(found->browse_handler);
	if(dnssd_handle == NULL) {
		DNSSD_LOGD("Invalid browse handle");
		__DNSSD_LOG_FUNC_EXIT__;
		return;
	}
	DNSSD_LOGD("GetAddrInfo Callback set for [%p]",
			dnssd_handle);

	browse = GET_BROWSE_DATA_P(dnssd_handle);
	callback = browse->callback;
	data = browse->user_data;

	if (address && address->sa_family == AF_INET) {

		const unsigned char *addr =
			(const unsigned char *) &
			((struct sockaddr_in *)address)->sin_addr;
		memcpy(&(found->ip_v4_addr), addr, IPV4_LEN);
	} else if (address && address->sa_family == AF_INET6) {

		const struct sockaddr_in6 *s6 =
			(const struct sockaddr_in6 *)address;
		const unsigned char *addr =
			(const unsigned char *)&s6->sin6_addr;
		memcpy(&(found->ip_v6_addr), addr, IPV6_LEN);
	}

	if (callback) {
		callback(DNSSD_SERVICE_AVAILABLE,
				local_handle->service_handler, data);
		DNSSD_LOGD("Finished executing Browse "
				"Available Callback");
	}

}

static int __dnssd_getaddrinfo(dnssd_handle_s *dnssd_handle, unsigned int flags,
		unsigned int if_index, const char *host_name,
		const char *fullname, const char *txt_record,
		unsigned short txt_len, int port)
{
	int ret;
	DNSServiceRef *sd_ref;
	dnssd_handle_s *local_handle = NULL;
	dnssd_found_data_s *found = NULL;
	char *name;
	char *domain;
	int dns_protocol = kDNSServiceProtocol_IPv4 | kDNSServiceProtocol_IPv6;

	if(dnssd_handle == NULL) {
		DNSSD_LOGD("Invalid browse handle");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OUT_OF_MEMORY;
	}

	/* fullname is of the form <servicename>.<protocol>.<domain>
	 * Extract servicename and domain from fullname */
	name = g_strdup(fullname);
	domain = strtok(name, ".");
	domain = strtok(NULL, ".");
	domain = strtok(NULL, ".");
	domain = strtok(NULL, ".");

	local_handle = g_try_malloc0(FOUND_SIZE);
	if(local_handle == NULL) {
		DNSSD_LOGD("g_try_malloc failed");
		g_free(name);
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OUT_OF_MEMORY;
	}

	found = GET_FOUND_DATA_P(local_handle);

	local_handle->op_type = DNSSD_TYPE_FOUND;
	local_handle->service_handler = (unsigned int)local_handle & 0xffffffff;
	g_strlcpy(local_handle->domain, domain, sizeof(local_handle->domain));
	local_handle->if_index = if_index;
	local_handle->flags= flags;
	if(dnssd_handle->service_type)
		local_handle->service_type = g_strdup(dnssd_handle->service_type);

	found->browse_handler = dnssd_handle->service_handler;
	found->service_name = g_strdup(name);
	g_free(name);
	if(txt_record != NULL) {
		found->txt_record = g_strdup(txt_record);
		found->txt_len = txt_len;
	}
	found->host_name = g_strdup(host_name);
	found->port = port;

	DNSSD_LOGD("Create found handle [%u]", local_handle->service_handler);

	dnssd_handle_list = g_slist_prepend(dnssd_handle_list, local_handle);

	DNSSD_LOGD("g_slist length [%d]", g_slist_length(dnssd_handle_list));

	sd_ref = &(local_handle->sd_ref);

	ret = DNSServiceGetAddrInfo(sd_ref, flags, if_index,
			dns_protocol, host_name,
			__dnssd_getaddrinfo_reply_cb, local_handle);
	if(ret < 0) {
		DNSSD_LOGE("Failed to GetAddrInfo, error[%s]",
				dnssd_error_to_string(ret));
		return ret;
	}

	DNSServiceHandleEvents(sd_ref);

	DNSSD_LOGD("Succeeded to GetAddrInfo");

	return DNSSD_ERROR_NONE;
}

static void __dnssd_resolve_reply_cb(DNSServiceRef sd_ref, unsigned int flags,
		unsigned int if_index, DNSServiceErrorType error_code,
		const char *fullname, const char *host_name,
		unsigned short port, unsigned short txt_len,
		const unsigned char *txt_record, void *user_data)
{
	resolve_reply_data *resolve_data = user_data;
	DNSSD_LOGD("Received Resolve Reply");
	/* Get Address Info details and send browse callback */
	__dnssd_getaddrinfo(resolve_data->dnssd_handle, flags, if_index,
			host_name, fullname, (const char *) txt_record,
			txt_len, port);
}

static int __dnssd_resolve_dns_service(dnssd_handle_s *dnssd_handle,
		unsigned int flags, unsigned int if_index,
		const char *service_name, const char *type, const char *domain)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	resolve_reply_data *data;
	DNSServiceRef *sd_ref;

	data = g_try_malloc0(sizeof(resolve_reply_data));
	if (data == NULL) {
		DNSSD_LOGD("g_try_malloc failed");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_OUT_OF_MEMORY;
	}

	data->dnssd_handle =  dnssd_handle;

	resolve_handle_list = g_slist_prepend(resolve_handle_list, data);

	sd_ref = &(data->sd_ref);

	ret = DNSServiceResolve(sd_ref, flags, if_index, service_name,
			type, domain, __dnssd_resolve_reply_cb, data);
	if(ret < 0) {
		DNSSD_LOGE("Failed to Resolve DNS Service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return ret;
	}

	DNSServiceHandleEvents(sd_ref);

	DNSSD_LOGD("Succeeded to Resolve DNS Service");

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

static void __dnssd_broswe_reply_cb(DNSServiceRef sd_ref, unsigned int flags,
		unsigned int if_index, DNSServiceErrorType error_code,
		const char *service_name, const char *service_type,
		const char *domain, void *user_data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *dnssd_handle;
	GSList *list;
	void *data;

	DNSSD_LOGD("Received Browse Reply");

	if (error_code != DNSSD_ERROR_NONE) {
		DNSSD_LOGE("Failed to browse, error code %s",
				dnssd_error_to_string(error_code));
		__DNSSD_LOG_FUNC_EXIT__;
		return;
	}

	for (list = dnssd_handle_list; list; list = list->next) {
		dnssd_handle = (dnssd_handle_s *) list->data;
		if(dnssd_handle == NULL)
			continue;

		if (dnssd_handle->sd_ref == sd_ref) {
			DNSSD_LOGD("Browse Callback set for [%p]", dnssd_handle);

			/* Update domain, interface and flags */
			g_strlcpy(dnssd_handle->domain, domain,
					sizeof(dnssd_handle->domain));
			dnssd_handle->flags = flags;
			dnssd_handle->if_index = if_index;
			DNSSD_LOGD("Domain/Interface/Flags : %s/%d/0x%x",
					dnssd_handle->domain,
					dnssd_handle->if_index,
					dnssd_handle->flags);

			if (flags & kDNSServiceFlagsAdd) {
				/* Resolve address and send callback later */
				__dnssd_resolve_dns_service(dnssd_handle, flags,
						if_index, service_name,
						service_type, domain);
			} else {

				dnssd_browse_data_s *browse = NULL;
				dnssd_browse_cb callback;

				browse = GET_BROWSE_DATA_P(dnssd_handle);
				callback = browse->callback;
				data = browse->user_data;

				if (callback)
					callback(DNSSD_SERVICE_UNAVAILABLE,
							dnssd_handle->service_handler, data);
			}

			DNSSD_LOGD("Finished executing Browse Callback");
		}
	}
	__DNSSD_LOG_FUNC_EXIT__;
}

int dnssd_browse_service(dnssd_service_h dnssd_service,
		dnssd_browse_cb browse_cb, void *user_data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	dnssd_handle_s *local_handle;
	dnssd_browse_data_s *browse;
	DNSServiceRef *sd_ref;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (browse_cb == NULL) {
		DNSSD_LOGE("Callback Not Provided");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_BROWSE) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	browse = GET_BROWSE_DATA_P(local_handle);
	sd_ref = &(local_handle->sd_ref);

	DNSSD_LOGD("Domain/Interface/Flags : %s/%d/0x%x", local_handle->domain,
			local_handle->if_index, local_handle->flags);

	ret = DNSServiceBrowse(sd_ref, local_handle->flags,
			local_handle->if_index, local_handle->service_type,
			local_handle->domain, __dnssd_broswe_reply_cb,
			NULL);
	if(ret < 0) {
		DNSSD_LOGE("Failed to browse for dns service, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return ret;
	}

	browse->callback = browse_cb;
	browse->user_data = user_data;
	DNSServiceHandleEvents(sd_ref);

	DNSSD_LOGD("Succeeded to browse for dns service");
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

static void __dnssd_remove_found_service(gpointer data, gpointer user_data)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *found_handle;
	dnssd_found_data_s *found_data;
	dnssd_service_h *handler = NULL;

	found_handle = (dnssd_handle_s *)data;
	handler = (dnssd_service_h *)user_data;

	if(found_handle->op_type != DNSSD_TYPE_FOUND)
		return;

	found_data = GET_FOUND_DATA_P(found_handle);
	if(found_data->browse_handler != *handler)
		return;

	DNSSD_LOGD("handle [%u]", found_handle->service_handler);
	dnssd_handle_list = g_slist_remove(dnssd_handle_list,
			found_handle);

	DNSServiceRefDeallocate(found_handle->sd_ref);

	g_free(found_handle->service_type);
	g_free(found_data->service_name);
	g_free(found_data->host_name);
	g_free(found_data->txt_record);
	g_free(found_handle);

	__DNSSD_LOG_FUNC_EXIT__;
	return;
}

int dnssd_stop_browse_service(dnssd_service_h dnssd_service)
{
	__DNSSD_LOG_FUNC_ENTER__;
	dnssd_handle_s *local_handle;
	DNSServiceRef sd_ref;
	GSList *list;
	resolve_reply_data *resolve_data;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler %u not found", dnssd_service);
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_BROWSE) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	sd_ref = local_handle->sd_ref;
	if(NULL == sd_ref) {
		DNSSD_LOGE("Invalid DNS SD Client");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	g_slist_foreach(dnssd_handle_list, (GFunc)__dnssd_remove_found_service,
			&(local_handle->service_handler));

	list = resolve_handle_list;
	while (list) {
		resolve_data = list->data;
		list = list->next;

		if (resolve_data->dnssd_handle == local_handle) {
			resolve_handle_list = g_slist_remove(resolve_handle_list,
					resolve_data);

			DNSServiceRefDeallocate(resolve_data->sd_ref);

			g_free(resolve_data);
		}
	}

	DNSServiceRefDeallocate(sd_ref);
	sd_ref = NULL;

	DNSSD_LOGD("g_slist length [%d]", g_slist_length(dnssd_handle_list));
	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_set_txt_record_value(dnssd_service_h dnssd_service,
		const char *key, unsigned char value_size, const void *value)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	TXTRecordRef *txt_record;
	dnssd_handle_s *local_handle = NULL;
	dnssd_register_data_s *reg = NULL;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (key == NULL) {
		DNSSD_LOGE("key is NULL");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (value_size == 0 || value == NULL)
		DNSSD_LOGD("Value size is 0 or NULL value passed");

	reg = GET_REG_DATA_P(local_handle);
	txt_record = &(reg->txt_ref);

	TXTRecordCreate(txt_record, 0, NULL);

	ret = TXTRecordSetValue(txt_record, key, value_size, value);
	if (ret != DNSSD_ERROR_NONE) {
		DNSSD_LOGE("Failed to add value to key, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return ret;
	} else {
		DNSSD_LOGD("Succeeded to add value to key");
	}

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_unset_txt_record_value(dnssd_service_h dnssd_service, const char *key)
{
	__DNSSD_LOG_FUNC_ENTER__;
	int ret;
	TXTRecordRef *txt_record;
	dnssd_handle_s *local_handle;
	dnssd_register_data_s *reg = NULL;
	unsigned short txt_len;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGD("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (local_handle->op_type != DNSSD_TYPE_REGISTER) {
		DNSSD_LOGD("Invalid DNS SD Operation type");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (key == NULL) {
		DNSSD_LOGE("key is NULL");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	reg = GET_REG_DATA_P(local_handle);
	txt_record = &(reg->txt_ref);

	ret = TXTRecordRemoveValue(txt_record, key);
	if (ret != DNSSD_ERROR_NONE) {
		DNSSD_LOGE("Failed to remove value from key, error[%s]",
				dnssd_error_to_string(ret));
		__DNSSD_LOG_FUNC_EXIT__;
		return ret;
	} else {
		DNSSD_LOGD("Succeeded to remove value from key");
	}

	txt_len = TXTRecordGetLength(txt_record);

	if (TXTRecordGetCount(txt_len, txt_record) == 0) {
		DNSSD_LOGD("No more key exists in TXT Record");
		/* Free TXT Record */
		TXTRecordDeallocate(txt_record);
	}

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}

int dnssd_get_txt_record_value(dnssd_service_h  dnssd_service,
		unsigned short *buf_len, const void **buffer) {
	__DNSSD_LOG_FUNC_ENTER__;
	TXTRecordRef *txt_record;
	dnssd_handle_s *local_handle;

	local_handle = __dnssd_check_handle_validity(dnssd_service);
	if (local_handle == NULL) {
		DNSSD_LOGE("Service Handler not found");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if (buffer == NULL || buf_len == NULL) {
		DNSSD_LOGE("Invalid Parameter");
		__DNSSD_LOG_FUNC_EXIT__;
		return DNSSD_ERROR_INVALID_PARAMETER;
	}

	if(local_handle->op_type == DNSSD_TYPE_FOUND) {

		dnssd_found_data_s *found = NULL;
		found = GET_FOUND_DATA_P(local_handle);

		*buffer = g_strdup(found->txt_record);
		*buf_len = found->txt_len;
	} else {

		dnssd_register_data_s *reg = NULL;
		reg = GET_REG_DATA_P(local_handle);
		txt_record = &(reg->txt_ref);

		*buffer = TXTRecordGetBytesPtr(txt_record);
		*buf_len = TXTRecordGetLength(txt_record);
	}

	__DNSSD_LOG_FUNC_EXIT__;
	return DNSSD_ERROR_NONE;
}
