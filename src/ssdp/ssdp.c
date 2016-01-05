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
#include <stdlib.h>

/*****************************************************************************
 * 	System headers
 *****************************************************************************/
#include <glib.h>
#include <gio/gio.h>

#include <libgssdp/gssdp.h>
#include <dlog.h>

/*****************************************************************************
 * 	Network Service Discovery headers
 *****************************************************************************/

#include "ssdp.h"
#include "ssdp-util.h"

/*****************************************************************************
 * 	Macros and Typedefs
 *****************************************************************************/

typedef enum {
	SSDP_SERVICE_STATE_UNKNOWN,
	SSDP_SERVICE_STATE_REGISTERED,
	SSDP_SERVICE_STATE_BROWSED,
	SSDP_SERVICE_STATE_FOUND,
} ssdp_service_state_e;

typedef struct {
	unsigned int service_id;
	unsigned int service_handler;
	char *target;
	char *usn;
	char *url;

	ssdp_service_state_e origin;
	ssdp_type_e op_type;

	unsigned int resource_id;
	GSSDPResourceGroup *resource_group;
	GSSDPResourceBrowser *resource_browser;

	ssdp_register_cb register_cb;
	ssdp_browse_cb browse_cb;
} ssdp_service_s;

/*****************************************************************************
 * 	Global Variables
 *****************************************************************************/
static __thread gboolean g_is_gssdp_init;
static __thread GSSDPClient *g_gssdp_client = NULL;

static __thread GList *g_ssdp_services;
/** moon: Need?
static __thread GHashTable *g_found_ssdp_services = NULL;
*/
/*****************************************************************************
 * 	Local Functions Definition
 *****************************************************************************/


static void __g_list_free_service(gpointer data, gpointer user_data)
{
	ssdp_service_s *service = NULL;

	service = (ssdp_service_s *)data;
	if(service == NULL)
		return;
	/* moon
	   if(service->resource_group != NULL && service->resource_id != 0)
	   gssdp_resource_group_remove_resource(service->resource_group, service->resource_id);

	   if(service->resource_browser != NULL)
	   g_object_unref (service->resource_browser);
	 */
	if(service->origin == SSDP_SERVICE_STATE_REGISTERED)
		SSDP_LOGE("Free registered service");
	else if (service->origin == SSDP_SERVICE_STATE_BROWSED)
		SSDP_LOGE("Free browsed service");
	else
		SSDP_LOGE("Free found service");

	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);
}

static ssdp_service_s *__ssdp_find_service(GList *services, unsigned int handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;
	GList *list;
	for (list = services; list != NULL; list = list->next) {
		service = list->data;
		if (service && service->service_handler == handler) {
			SSDP_LOGD("target [%s]", service->target);
			break;
		}
		else {
			service = NULL;
		}
	}
	__SSDP_LOG_FUNC_EXIT__;
	return service;
}

static void
__ssdp_res_available_cb (GSSDPResourceBrowser *resource_browser,
		const char *usn, GList *urls, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_h handler;
	ssdp_service_s *browsed_service = NULL;
	char *temp_url = NULL;
	char *ptr = NULL;
	int url_len = 0;
	GList *l;

	handler = (ssdp_service_h)user_data;
	if (handler == 0) {
		SSDP_LOGE("Invalid parameters");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	browsed_service = __ssdp_find_service(g_ssdp_services, handler);
	if (browsed_service == NULL) {
		SSDP_LOGE("Service not found");
		return;
	}

	for (l = urls; l != NULL; l = l->next) {
		SSDP_LOGD("  URL: %s", (char *)l->data);
		url_len += strlen((char *)l->data);
	}

	temp_url = g_try_malloc0(url_len + 1);
	if (!temp_url) {
		SSDP_LOGE("Failed to get memory for url");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	for (l = urls; l != NULL; l = l->next) {
		ptr = temp_url;
		g_strlcpy(ptr, (char*)l->data, strlen((char *)l->data) + 1);
		ptr+=strlen((char *)l->data);
	}

	/** moon: Need?
	found_service = g_hash_table_lookup(g_found_ssdp_services, usn);
	if (found_service != NULL &&
			g_strcmp0(found_service->usn, usn) == 0) {
		SSDP_LOGD("Duplicated service!");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	found_service = (ssdp_service_s *)g_try_malloc0(sizeof(ssdp_service_s));
	if (!found_service) {
		SSDP_LOGE("Failed to get memory for ssdp service structure");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}
	*/

	SSDP_LOGD("resource available\nUSN: %s", usn);

	/** moon: Need?
	found_service->usn = g_strdup(usn);
	found_service->location = temp_loc;
	found_service->service_handler = (unsigned int)found_service & 0xffffffff;
	found_service->service_id = browsed_service->service_handler;
	found_service->origin = SSDP_SERVICE_STATE_FOUND;

	SSDP_LOGD("added service [%u]", found_service->service_handler);
	g_hash_table_insert(g_found_ssdp_services, found_service->usn, found_service);
	if (service_found_cb != NULL)
		service_found_cb(SSDP_SERIVCE_AVAILABLE, found_service->usn,
				found_service->location, service_found_data);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));
	*/

	if (browsed_service->browse_cb) {
		/** moon: TODO: user_data? */
		browsed_service->browse_cb(handler, SSDP_SERVICE_AVAILABLE,
					NULL);
	}

	__SSDP_LOG_FUNC_EXIT__;
	return;

}

static void
__ssdp_res_unavailable_cb (GSSDPResourceBrowser *resource_browser,
		const char *usn, GList *urls, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_h handler;
	ssdp_service_s *service = NULL;

	handler = (ssdp_service_h)user_data;
	if (handler == 0) {
		SSDP_LOGE("Invalid parameters");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return;
	}


	/** moon: Need?
	found_service = g_hash_table_lookup(g_found_ssdp_services, usn);
	if (found_service == NULL) {
		SSDP_LOGD("No service matched!");
		return;
	}

	SSDP_LOGD("resource unavailable\n  USN: %s\n", usn);
	if (service_found_cb != NULL)
		service_found_cb(SSDP_SERIVCE_UNAVAILABLE, found_service->usn,
				found_service->location, user_data);

	g_hash_table_remove(g_found_ssdp_services, usn);
	g_free(found_service->target);
	g_free(found_service->usn);
	g_free(found_service->location);
	g_free(found_service);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));
	*/

	if (service->browse_cb) {
		/** moon: TODO: user_data? */
		service->browse_cb(handler, SSDP_SERVICE_UNAVAILABLE,
					NULL);
	}

	__SSDP_LOG_FUNC_EXIT__;
	return;
}

static int __ssdp_rescan(ssdp_service_s *service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	if (!(gssdp_resource_browser_rescan(service->resource_browser)))
	{
		SSDP_LOGE("Failed to request rescan");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}
	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

/*
static const char* __print_error(int error)
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
*/

int ssdp_initialize()
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	GError *gerror = NULL;

	if (g_is_gssdp_init) {
		SSDP_LOGE("gssdp already initialized");
		return SSDP_ERROR_ALREADY_INITIALIZED;
	}

	g_gssdp_client = gssdp_client_new (NULL, NULL, &gerror);
	if (gerror) {
		SSDP_LOGE("Error creating the GSSDP client: %s\n",
				gerror->message);
		g_error_free(gerror);
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}

	if (g_gssdp_client == NULL) {
		SSDP_LOGE("failed to create client\n");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}

	/** moon: Need?
	g_found_ssdp_services = g_hash_table_new(g_str_hash, g_str_equal);
	*/

	g_is_gssdp_init = TRUE;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_deinitialize()
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	g_list_foreach(g_ssdp_services, (GFunc)__g_list_free_service, NULL);
	g_list_free(g_ssdp_services);
	g_ssdp_services = NULL;


	/** moon: Need?
	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_free_service, NULL);
	g_hash_table_remove_all(g_found_ssdp_services);
	*/

	g_object_unref(g_gssdp_client);
	g_gssdp_client = NULL;
	g_is_gssdp_init = FALSE;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_create_service(ssdp_type_e op_type, const char *target, ssdp_service_h *ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	if (op_type != SSDP_TYPE_UNKNOWN && 
		op_type != SSDP_TYPE_REGISTER && 
		op_type != SSDP_TYPE_BROWSE) {
		SSDP_LOGE("Invalid SSDP type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	if (ssdp_service == NULL) {
		SSDP_LOGE("Invalid parameter");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	service = (ssdp_service_s*)g_try_malloc0(sizeof(ssdp_service_s));
	if (!service) {
		SSDP_LOGE("Failed to get memory for gssdp service structure");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OUT_OF_MEMORY;
	}

	service->target = g_strndup(target, strlen(target));
	if (!service->target) {
		SSDP_LOGE("Failed to get memory for gssdp service type");
		g_free(service);
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OUT_OF_MEMORY;
	}

	*ssdp_service = (unsigned int)service & 0xFFFFFFFF;
	SSDP_LOGD("Create handler for service [%u]", *ssdp_service);
	service->service_handler = *ssdp_service;
	service->origin = SSDP_SERVICE_STATE_UNKNOWN;	// moon: origin
	service->op_type = op_type;
	g_ssdp_services = g_list_append(g_ssdp_services, service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_destroy_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	// moon: add
	switch (service->op_type) {
	case SSDP_TYPE_REGISTER:
		ssdp_deregister_service(ssdp_service);
		break;
	case SSDP_TYPE_BROWSE:
		ssdp_stop_browse_service(ssdp_service);
		break;
	default:
		SSDP_LOGE("Invalid operation type");
		return SSDP_ERROR_NOT_SUPPORTED;
	}


	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	/** moon: delete
	if (service->resource_group != NULL) {
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
	}

	// moon: check
	if (service->resource_browser != NULL) {
		g_object_unref(service->resource_browser);
	}
	*/

	g_ssdp_services = g_list_remove(g_ssdp_services, service);
	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_set_usn(ssdp_service_h ssdp_service, const char* usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0 || usn == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);
	SSDP_LOGD("USN [%s]", usn);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->op_type != SSDP_TYPE_REGISTER) {
		SSDP_LOGE("Invalid operation type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	g_free(service->usn);
	service->usn = g_strndup(usn, strlen(usn));

	// moon: TODO : renew the advertising service if device already announced

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_set_url(ssdp_service_h ssdp_service, const char *url)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0 || url == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);
	SSDP_LOGD("Location [%s]", url);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->op_type != SSDP_TYPE_REGISTER) {
		SSDP_LOGE("Invalid operation type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	g_free(service->url);
	service->url = g_strndup(url, strlen(url));
	// moon: TODO : renew the advertising service if device already announced

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_target(ssdp_service_h ssdp_service, char **target)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0 || !target) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL || service->target == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*target = g_strndup(service->target, strlen(service->target));
	
	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_usn(ssdp_service_h ssdp_service, char **usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0 || !usn) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);

	if (service == NULL || service->usn == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*usn = g_strndup(service->usn, strlen(service->usn));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_get_url(ssdp_service_h ssdp_service, char **url)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0 || !url) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL || service->url == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*url = g_strndup(service->url, strlen(service->url));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_register_service(ssdp_service_h ssdp_service,
					ssdp_register_cb cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (cb == NULL) {
		// moon: TODO: ssdp_register_cb
	}

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->op_type != SSDP_TYPE_REGISTER) {
		SSDP_LOGE("Invalid operation type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	if (service->resource_group != NULL) {
		SSDP_LOGE("Resource group is not NULL\ninit first");
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
	}

	if (g_gssdp_client == NULL) {
		SSDP_LOGE("GSSDPClient is NULL. Init first");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	service->resource_group = gssdp_resource_group_new (g_gssdp_client);
	if (service->resource_group == NULL) {
		SSDP_LOGE("Resource group is NULL");
		return SSDP_ERROR_OPERATION_FAILED;
	}

	service->resource_id = gssdp_resource_group_add_resource_simple(
			service->resource_group,
			service->target,
			service->usn,
			service->url);

	SSDP_LOGD("Resource group id is [%d]\n", service->resource_id);
	if (service->resource_id == 0)
		status = SSDP_ERROR_OPERATION_FAILED;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}


int ssdp_deregister_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->op_type != SSDP_TYPE_REGISTER) {
		SSDP_LOGE("Invalid operation type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	if (service->resource_group != NULL && service->resource_id != 0) {
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
		service->resource_group = NULL;
		service->resource_id = 0;
	}

	service->register_cb = NULL;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_browse_service(ssdp_service_h ssdp_service, 
					ssdp_browse_cb browse_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->target == NULL) {
		SSDP_LOGE("Service type is not determined");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_INVALID_PARAMETER; 
	}

	if (service->op_type != SSDP_TYPE_BROWSE) {
		SSDP_LOGE("Invalid operation type");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	/** moon
GLIST_ITER_START(g_ssdp_services, service)
	if (strncmp(target, service->target, strlen(target)) &&
			service->origin == SSDP_SERVICE_STATE_BROWSED) {
		SSDP_LOGD("Browsing request is already registered");
		return __ssdp_rescan(service);
	}
GLIST_ITER_END()
*/

	if (service->origin == SSDP_SERVICE_STATE_FOUND) {
		SSDP_LOGD("Browsing for already found service");
		return __ssdp_rescan(service);
	}

	if (g_gssdp_client == NULL) {
		SSDP_LOGE("GSSDPClient is NULL. Init first");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	service->resource_browser = gssdp_resource_browser_new(
			g_gssdp_client,
			service->target);
	if (service->resource_browser == NULL) {
		SSDP_LOGE("Failed to create service browser\n");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}

	g_signal_connect(service->resource_browser, "resource-available",
			G_CALLBACK(__ssdp_res_available_cb), &ssdp_service);

	g_signal_connect(service->resource_browser, "resource-unavailable",
			G_CALLBACK(__ssdp_res_unavailable_cb), &ssdp_service);

	gssdp_resource_browser_set_active (service->resource_browser, TRUE);
	service->origin = SSDP_SERVICE_STATE_BROWSED;
	SSDP_LOGD("service ssdp_service [%u]", ssdp_service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_stop_browse_service(ssdp_service_h ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	SSDP_LOGD("SSDP service ID [%u]", ssdp_service);
	service = __ssdp_find_service(g_ssdp_services, ssdp_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	g_object_unref (service->resource_browser);

	/* moon: Need?
	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_remove_related_services,
			(gpointer)service->service_handler);
			*/

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}



