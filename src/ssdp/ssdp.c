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
	SSDP_SERVICE_STATE_NOT_REGISTERED,
	SSDP_SERVICE_STATE_REGISTERED,
	SSDP_SERVICE_STATE_BROWSED,
	SSDP_SERVICE_STATE_FOUND,
} ssdp_state_e;

typedef struct {
	unsigned int browser_id;		/* for found services */
	unsigned int service_handler;
	char *target;
	char *usn;
	char *url;

	ssdp_state_e origin;

	unsigned int resource_id;
	GSSDPResourceGroup *resource_group;
	GSSDPResourceBrowser *resource_browser;

	ssdp_registered_cb registered_cb;
	ssdp_found_cb found_cb;

	void *cb_user_data;
} ssdp_service_s;

typedef struct {
	    ssdp_service_s *service;
		unsigned int service_id;
} foreach_hash_user_data_s;


/*****************************************************************************
 * 	Global Variables
 *****************************************************************************/
static __thread gboolean g_is_gssdp_init;
static __thread GSSDPClient *g_gssdp_client = NULL;

static __thread GList *g_ssdp_local_services;
static __thread GHashTable *g_found_ssdp_services = NULL;
//static __thread GList *g_ssdp_browsers; TODO
/*****************************************************************************
 * 	Local Functions Definition
 *****************************************************************************/

/**
 * free a local service
 */
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
	if(service->origin == SSDP_SERVICE_STATE_NOT_REGISTERED)
		SSDP_LOGE("Free not registered service");
	else if(service->origin == SSDP_SERVICE_STATE_REGISTERED)
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

/**
 * free a remote service
 */
static void __g_hash_free_service(gpointer key, gpointer value, 
							gpointer user_data)
{
	ssdp_service_s *service = NULL;

	service = (ssdp_service_s *)value;
	if(service == NULL)
		return;
	/*
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

/**
 * remove remove services found by request of a browser (browser_id is passed to by user_data)
 */
static void __g_hash_remove_related_services(gpointer key, gpointer value,
							gpointer user_data)
{
	ssdp_service_s *service = NULL;
	int browse_id = *((unsigned int *)user_data);

	service = (ssdp_service_s *)value;
	if(service == NULL)
		return;

	if(service->browser_id != browse_id)
		return;

	SSDP_LOGD("Free found service");

	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);
}

static ssdp_service_s *__ssdp_find_local_service(GList *services, 
											unsigned int handler)
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

static void __g_hash_find_remote_service(gpointer key, gpointer value, 
											gpointer user_data)
{
	ssdp_service_s *service = NULL;
	foreach_hash_user_data_s *data = NULL;

	service = (ssdp_service_s *)value;
	data = (foreach_hash_user_data_s *)user_data;
	if(service == NULL || data == NULL)
		return;

	if (service->origin != SSDP_SERVICE_STATE_FOUND)
		return;

	if(service->service_handler == data->service_id)
		data->service = service;

	return;
}

static ssdp_service_s *__ssdp_find_remote_service(GHashTable *services, 
													unsigned int handler)
{
	foreach_hash_user_data_s user_data;

	user_data.service_id = handler;
	user_data.service = NULL;
	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_find_remote_service, &user_data);

	return user_data.service;
}

static void
__ssdp_res_available_cb (GSSDPResourceBrowser *resource_browser,
				const char *usn, GList *urls, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *browser = NULL;
	ssdp_service_s *found_service = NULL;
	char *temp_url = NULL;
	char *ptr = NULL;
	int url_len = 0;
	GList *l;

	browser = (ssdp_service_s *)user_data;
	if (browser == NULL) {
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

	SSDP_LOGD("resource available\nUSN: %s", usn);

	found_service->usn = g_strdup(usn);
	found_service->url= temp_url;
	found_service->service_handler = (unsigned int)found_service & 0xffffffff;
	found_service->browser_id = browser->service_handler;
	found_service->origin = SSDP_SERVICE_STATE_FOUND;

	SSDP_LOGD("added service [%u]", found_service->service_handler);
	g_hash_table_insert(g_found_ssdp_services, 
			found_service->usn, found_service);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));

	if (browser->found_cb) {
		browser->found_cb(found_service->service_handler,
				SSDP_SERVICE_STATE_AVAILABLE, browser->cb_user_data);
	}

	__SSDP_LOG_FUNC_EXIT__;
	return;

}

static void
__ssdp_res_unavailable_cb (GSSDPResourceBrowser *resource_browser,
		const char *usn, GList *urls, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *browser = NULL;
	ssdp_service_s *found_service = NULL;

	browser = (ssdp_service_s *)user_data;
	if (browser == NULL) {
		SSDP_LOGE("Service not found");
		return;
	}

	found_service = g_hash_table_lookup(g_found_ssdp_services, usn);
	if (found_service == NULL) {
		SSDP_LOGD("No service matched!");
		return;
	}

	SSDP_LOGD("resource unavailable\n  USN: %s\n", usn);

	g_hash_table_remove(g_found_ssdp_services, usn);
	g_free(found_service->target);
	g_free(found_service->usn);
	g_free(found_service->url);
	g_free(found_service);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));

	if (browser->found_cb) {
		browser->found_cb(found_service->service_handler,
				SSDP_SERVICE_STATE_UNAVAILABLE, browser->cb_user_data);
	}

	__SSDP_LOG_FUNC_EXIT__;
	return;
}

int ssdp_initialize()
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	GError *gerror = NULL;

	if (g_is_gssdp_init) {
		SSDP_LOGE("gssdp already initialized");
		return SSDP_ERROR_NONE;
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

	g_found_ssdp_services = g_hash_table_new(g_str_hash, g_str_equal);

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

	g_list_foreach(g_ssdp_local_services, (GFunc)__g_list_free_service, NULL);
	g_list_free(g_ssdp_local_services);
	g_ssdp_local_services = NULL;

	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_free_service, NULL);
	g_hash_table_remove_all(g_found_ssdp_services);

	g_object_unref(g_gssdp_client);
	g_gssdp_client = NULL;
	g_is_gssdp_init = FALSE;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_create_local_service(const char *target, ssdp_service_h *ssdp_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
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
	service->origin = SSDP_SERVICE_STATE_NOT_REGISTERED;
	g_ssdp_local_services = g_list_append(g_ssdp_local_services, service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_destroy_local_service(ssdp_service_h ssdp_service)
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

	service = __ssdp_find_local_service(g_ssdp_local_services, ssdp_service);
	if (service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if (service->resource_group != NULL)
		service->resource_group = NULL;

	if (service->resource_browser != NULL)
		g_object_unref(service->resource_browser);

	g_ssdp_local_services = g_list_remove(g_ssdp_local_services, service);
	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_service_set_usn(ssdp_service_h local_service, const char* usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (local_service == 0 || usn == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", local_service);
	SSDP_LOGD("USN [%s]", usn);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_local_service(g_ssdp_local_services, local_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	g_free(service->usn);
	service->usn = g_strndup(usn, strlen(usn));


	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_service_set_url(ssdp_service_h local_service, const char *url)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (local_service == 0 || url == NULL) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", local_service);
	SSDP_LOGD("Location [%s]", url);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_local_service(g_ssdp_local_services, local_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	g_free(service->url);
	service->url = g_strndup(url, strlen(url));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_service_get_target(ssdp_service_h ssdp_service, char **target)
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

	service = __ssdp_find_local_service(g_ssdp_local_services, ssdp_service);
	if (service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, 
												ssdp_service);

	if (service == NULL || service->target == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*target = g_strndup(service->target, strlen(service->target));
	
	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_service_get_usn(ssdp_service_h ssdp_service, char **usn)
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

	service = __ssdp_find_local_service(g_ssdp_local_services, ssdp_service);
	if (service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, 
												ssdp_service);

	if (service == NULL || service->usn == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*usn = g_strndup(service->usn, strlen(service->usn));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_service_get_url(ssdp_service_h ssdp_service, char **url)
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

	service = __ssdp_find_local_service(g_ssdp_local_services, ssdp_service);
	if (service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, 
												ssdp_service);

	if (service == NULL || service->url == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	*url = g_strndup(service->url, strlen(service->url));

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_register_local_service(ssdp_service_h local_service,
					ssdp_registered_cb cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (local_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", local_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_local_service(g_ssdp_local_services, local_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
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

	service->origin = SSDP_SERVICE_STATE_REGISTERED;
	service->registered_cb = cb;
	service->cb_user_data = user_data;

	gssdp_resource_group_set_available (service->resource_group, TRUE);
	SSDP_LOGD("Now service is available [%u]", local_service);

	SSDP_LOGD("Resource group id is [%d]\n", service->resource_id);
	if (service->resource_id == 0)
		status = SSDP_ERROR_OPERATION_FAILED;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}


int ssdp_deregister_local_service(ssdp_service_h local_service)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *service = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (local_service == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}
	SSDP_LOGD("SSDP service ID [%u]", local_service);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_local_service(g_ssdp_local_services, local_service);
	if (service == NULL) {
		SSDP_LOGE("Service not found");
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	if (service->resource_group != NULL && service->resource_id != 0) {
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
		service->resource_group = NULL;
		service->resource_id = 0;
	}

	service->origin = SSDP_SERVICE_STATE_NOT_REGISTERED;
	service->registered_cb = NULL;
	service->cb_user_data = NULL;

	gssdp_resource_group_set_available (service->resource_group, FALSE);
	SSDP_LOGD("Now service is unavailable [%u]", local_service);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_start_browsing_service(const char* target, ssdp_browser_h* ssdp_browser, 
					ssdp_found_cb found_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *browser = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	if (g_gssdp_client == NULL) {
		SSDP_LOGE("GSSDPClient is NULL. Init first");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_INVALID_PARAMETER;
	}

GLIST_ITER_START(g_ssdp_local_services, browser)
	if (strncmp(target, browser->target, strlen(target)) &&
			browser->origin == SSDP_SERVICE_STATE_BROWSED) {
		SSDP_LOGD("Browsing request is already registered");
	}
	else {
		browser = NULL;
	}
GLIST_ITER_END()

	if (browser == NULL) {
		browser = (ssdp_service_s*)g_try_malloc0(sizeof(ssdp_service_s));
		if (!browser) {
			SSDP_LOGE("Failed to get memory for gssdp service structure");
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OUT_OF_MEMORY;
		}

		browser->target = g_strndup(target, strlen(target));
		if (!browser->target) {
			SSDP_LOGE("Failed to get memory for gssdp service type");
			g_free(browser);
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OUT_OF_MEMORY;
		}

		/* Create browser handle */
		*ssdp_browser = (unsigned int)browser & 0xFFFFFFFF;
		SSDP_LOGD("Create handler for browser [%u]", *ssdp_browser);
		browser->service_handler = *ssdp_browser;

		/* Create the service browser */
		browser->resource_browser = gssdp_resource_browser_new(
				g_gssdp_client,
				browser->target);
		if (browser->resource_browser == NULL) {
			SSDP_LOGE("Failed to create service browser\n");
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OPERATION_FAILED;
		}

		/* Connect signals */
		g_signal_connect(browser->resource_browser, "resource-available",
				G_CALLBACK(__ssdp_res_available_cb), browser);

		g_signal_connect(browser->resource_browser, "resource-unavailable",
				G_CALLBACK(__ssdp_res_unavailable_cb), browser);

		gssdp_resource_browser_set_active(browser->resource_browser, TRUE);

		/* Set properties */
		browser->origin = SSDP_SERVICE_STATE_BROWSED;
		g_ssdp_local_services = g_list_append(g_ssdp_local_services, browser);
	}
	else {
		if (!(gssdp_resource_browser_rescan(browser->resource_browser)))
		{
			SSDP_LOGE("Failed to request rescan");
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OPERATION_FAILED;
		}
	}

	browser->found_cb = found_cb;
	browser->cb_user_data = user_data;

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}

int ssdp_stop_browsing_service(ssdp_browser_h ssdp_browser)
{
	__SSDP_LOG_FUNC_ENTER__;
	int status = SSDP_ERROR_NONE;
	ssdp_service_s *browser = NULL;

	CHECK_FEATURE_SUPPORTED(NETWORK_SERVICE_DISCOVERY_FEATURE);

	if (ssdp_browser == 0) {
		SSDP_LOGE("Invalid parameter");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	SSDP_LOGD("SSDP browser ID [%u]", ssdp_browser);
	browser = __ssdp_find_local_service(g_ssdp_local_services, ssdp_browser);
	if (browser == NULL) {
		SSDP_LOGE("Service not found");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	}

	gssdp_resource_browser_set_active (browser->resource_browser, FALSE);

	g_object_unref (browser->resource_browser);
	browser->found_cb = NULL;
	browser->cb_user_data = NULL;

	g_ssdp_local_services = g_list_remove(g_ssdp_local_services, browser);

	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_remove_related_services,
			(gpointer)browser->service_handler);

	g_free(browser->target);
	g_free(browser);

	__SSDP_LOG_FUNC_EXIT__;
	return status;
}



