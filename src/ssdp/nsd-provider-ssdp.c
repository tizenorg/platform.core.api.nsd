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
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*****************************************************************************
 * 	System headers
 *****************************************************************************/

#include<glib.h>
#include <gio/gio.h>

#include <libgssdp/gssdp.h>

#include<dlog.h>

/*****************************************************************************
 * 	Network Service Discovery headers
 *****************************************************************************/

#include "ssdp.h"
#include "ssdp-util.h"
#include "nsd-provider-ssdp.h"

/*****************************************************************************
 * 	Macros and Typedefs
 *****************************************************************************/

typedef enum {
	SSDP_SERVICE_TYPE_REGISTERED,
	SSDP_SERVICE_TYPE_BROWSED,
	SSDP_SERVICE_TYPE_FOUND,
}nsd_service_type_e;

typedef struct {
	unsigned int service_id;
	unsigned int service_handler;
	char *target;
	char *usn;
	char *url;

	nsd_service_type_e origin;
	ssdp_type_e op_type;

	ssdp_register_cb register_cb;
	void *register_cb_user_data;
	ssdp_browse_cb browse_cb;
	void *browse_cb_user_data;

	unsigned int resource_id;
	GSSDPResourceGroup *resource_group;
	GSSDPResourceBrowser *resource_browser;
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

static __thread GList *g_ssdp_services;
static __thread GHashTable *g_found_ssdp_services = NULL;

/*****************************************************************************
 * 	Local Functions Definition
 *****************************************************************************/

static void __g_list_free_service(gpointer data, gpointer user_data)
{
	ssdp_service_s *service = NULL;

	service = (ssdp_service_s *)data;
	if(service == NULL)
		return;
/*
	if(service->resource_group != NULL && service->resource_id != 0)
		gssdp_resource_group_remove_resource(service->resource_group, service->resource_id);

	if(service->resource_browser != NULL)
		g_object_unref (service->resource_browser);
*/
	if(service->origin == SSDP_SERVICE_TYPE_REGISTERED)
		SSDP_LOGE("Free registered service");
	else if (service->origin == SSDP_SERVICE_TYPE_BROWSED)
		SSDP_LOGE("Free browsed service");
	else
		SSDP_LOGE("Free found service");

	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);
}

static void __g_hash_free_service(gpointer key, gpointer value, gpointer user_data)
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
	if(service->origin == SSDP_SERVICE_TYPE_REGISTERED)
		SSDP_LOGE("Free registered service");
	else if (service->origin == SSDP_SERVICE_TYPE_BROWSED)
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

	ssdp_service_s *service = NULL;
	GList *list;
	for(list = services; list != NULL; list = list->next) {
		service = list->data;
		if(service && service->service_handler == handler) {
			SSDP_LOGD("target [%s]", service->target);
			break;
		} else {
			service = NULL;
		}
	}
	return service;
}

static void __g_hash_find_service(gpointer key, gpointer value, gpointer user_data)
{
	ssdp_service_s *service = NULL;
	foreach_hash_user_data_s *data = NULL;

	service = (ssdp_service_s *)value;
	data = (foreach_hash_user_data_s *)user_data;
	if(service == NULL || data == NULL)
		return;

	if (service->origin != SSDP_SERVICE_TYPE_BROWSED)
		return;

	if(service->service_handler == data->service_id)
		data->service = service;

	return;
}

static ssdp_service_s *__ssdp_find_remote_service(GHashTable *services, unsigned int handler)
{
	foreach_hash_user_data_s user_data;

	user_data.service_id = handler;
	user_data.service = NULL;
	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_find_service, &user_data);

	return user_data.service;
}

static void __g_hash_remove_related_services(gpointer key, gpointer value, gpointer user_data)
{
	ssdp_service_s *service = NULL;
	int browse_id = *((unsigned int *)user_data);

	service = (ssdp_service_s *)value;
	if(service == NULL)
		return;

	if(service->service_id != browse_id)
		return;

	SSDP_LOGD("Free found service");

	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);
}

static void
__nsd_provider_ssdp_res_available_cb (GSSDPResourceBrowser *resource_browser,
		const char *usn, GList *locations, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s * browsed_service = NULL;
	ssdp_service_s * found_service = NULL;
	char *temp_loc = NULL;
	char *ptr = NULL;
	int location_len = 0;
	GList *l;

	browsed_service = (ssdp_service_s *)user_data;
	if(browsed_service == NULL || usn == NULL || locations == NULL) {
		SSDP_LOGE("Invalid parameters");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	for (l = locations; l; l = l->next) {
		SSDP_LOGD("  Location: %s", (char *) l->data);
		location_len += strlen((char *)l->data);
	}

	temp_loc = g_try_malloc0(location_len + 1);
	if(temp_loc) {
		for (l = locations; l; l = l->next) {
			ptr = temp_loc;
			g_strlcpy(ptr, (char*)l->data, strlen((char *)l->data) + 1);
			ptr+=strlen((char *)l->data);
		}
	}

	found_service = g_hash_table_lookup(g_found_ssdp_services, usn);
	if(found_service != NULL &&
			g_strcmp0(found_service->usn, usn) == 0) {
		SSDP_LOGD("Duplicated service!");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	found_service = (ssdp_service_s *)g_try_malloc0(sizeof(ssdp_service_s));
	if(!found_service) {
		SSDP_LOGE("Failed to get memory for ssdp service structure");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	SSDP_LOGD("resource available\nUSN: %s", usn);

	found_service->usn = g_strdup(usn);
	found_service->url = temp_loc;
	found_service->service_handler = (unsigned int)found_service & 0xffffffff;
	found_service->service_id = browsed_service->service_handler;
	found_service->origin = SSDP_SERVICE_TYPE_FOUND;

	SSDP_LOGD("added service [%u]", found_service->service_handler);
	g_hash_table_insert(g_found_ssdp_services, found_service->usn, found_service);
	if(browsed_service->browse_cb != NULL)
		browsed_service->browse_cb(found_service->service_handler,
				SSDP_SERVICE_AVAILABLE, browsed_service->browse_cb_user_data);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));

	__SSDP_LOG_FUNC_EXIT__;
	return;
}

static void
__nsd_provider_ssdp_res_unavailable_cb (GSSDPResourceBrowser *resource_browser,
		const char *usn, gpointer user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s * browsed_service = NULL;
	ssdp_service_s * found_service = NULL;

	browsed_service = (ssdp_service_s *)user_data;
	if(browsed_service == NULL) {
		SSDP_LOGE("Invalid parameters");
		__SSDP_LOG_FUNC_EXIT__;
		return;
	}

	found_service = g_hash_table_lookup(g_found_ssdp_services, usn);
	if(found_service == NULL) {
		SSDP_LOGD("No service matched!");
		return;
	}

	SSDP_LOGD("resource unavailable\n  USN: %s\n", usn);
	if(browsed_service->browse_cb != NULL)
		browsed_service->browse_cb(found_service->service_handler,
				SSDP_SERVICE_UNAVAILABLE, browsed_service->browse_cb_user_data);

	g_hash_table_remove(g_found_ssdp_services, usn);
	g_free(found_service->target);
	g_free(found_service->usn);
	g_free(found_service->url);
	g_free(found_service);

	SSDP_LOGD("Hash tbl size [%d]", g_hash_table_size(g_found_ssdp_services));
	__SSDP_LOG_FUNC_EXIT__;
	return;
}

int nsd_provider_ssdp_initialize(void)
{
	__SSDP_LOG_FUNC_ENTER__;
	GError *gerror = NULL;

	if(g_is_gssdp_init) {
		SSDP_LOGE("gssdp already initialized");
		return SSDP_ERROR_NONE;
	}

	g_gssdp_client = gssdp_client_new (
			NULL,
			NULL,
			&gerror);
	if (gerror) {
		SSDP_LOGE("Error creating the GSSDP client: %s\n",
				gerror->message);

		g_error_free(gerror);
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}

	if(g_gssdp_client == NULL) {
		SSDP_LOGE("failed to create client\n");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OPERATION_FAILED;
	}

	g_found_ssdp_services = g_hash_table_new(g_str_hash, g_str_equal);

	g_is_gssdp_init = TRUE;
	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_deinitialize(void )
{
	__SSDP_LOG_FUNC_ENTER__;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	g_list_foreach(g_ssdp_services, (GFunc)__g_list_free_service, NULL);
	g_list_free(g_ssdp_services);
	g_ssdp_services = NULL;

	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_free_service, NULL);
	g_hash_table_remove_all(g_found_ssdp_services);


	g_object_unref (g_gssdp_client);
	g_gssdp_client = NULL;
	g_is_gssdp_init = FALSE;
	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_create_service(ssdp_type_e op_type, const char *target, unsigned int *handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	if (op_type != SSDP_TYPE_UNKNOWN && 
		op_type != SSDP_TYPE_REGISTER &&
		op_type != SSDP_TYPE_BROWSE) {
		SSDP_LOGE("Invalid SSDP type");
		return SSDP_ERROR_INVALID_PARAMETER;
	}

	service = (ssdp_service_s *)g_try_malloc0(sizeof(ssdp_service_s));
	if(!service) {
		SSDP_LOGE("Failed to get memory for gssdp service structure");
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OUT_OF_MEMORY;
	}

	service->target = g_strndup(target, strlen(target));
	if(!service->target) {
		SSDP_LOGE("Failed to get memory for gssdp service name");
		g_free(service);
		__SSDP_LOG_FUNC_EXIT__;
		return SSDP_ERROR_OUT_OF_MEMORY;
	}

	*handler = (unsigned int)service & 0xffffffff;
	service->service_handler = *handler;
	service->origin = SSDP_SERVICE_TYPE_REGISTERED;
	service->op_type = op_type;
	g_ssdp_services = g_list_append(g_ssdp_services, service);
	printf("service_handler [%u]\n", *handler);

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_destroy_service(unsigned int handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->resource_group != NULL) {
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
	}
	g_ssdp_services = g_list_remove(g_ssdp_services, service);

	g_free(service->target);
	g_free(service->usn);
	g_free(service->url);
	g_free(service);

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_register_service(unsigned int handler,
		ssdp_register_cb register_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;
	int ret = SSDP_ERROR_NONE;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->origin != SSDP_SERVICE_TYPE_REGISTERED)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->resource_group != NULL) {
		SSDP_LOGE("Resource group is not NULL\ninit first");
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
	}

	service->resource_group = gssdp_resource_group_new (g_gssdp_client);
	if(service->resource_group == NULL) {
		SSDP_LOGE("Resource group is NULL");
		return SSDP_ERROR_OPERATION_FAILED;
	}

	service->resource_id = gssdp_resource_group_add_resource_simple(
			service->resource_group,
			service->target,
			service->usn,
			service->url);

	SSDP_LOGD("Resource group id is [%d]\n", service->resource_id);
	if(service->resource_id == 0)
		ret = SSDP_ERROR_OPERATION_FAILED;

	__SSDP_LOG_FUNC_EXIT__;
	return ret;
}

int nsd_provider_ssdp_deregister_service(unsigned int handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->origin != SSDP_SERVICE_TYPE_REGISTERED)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->resource_group != NULL && service->resource_id != 0) {
		gssdp_resource_group_remove_resource(service->resource_group,
				service->resource_id);
		service->resource_group = NULL;
		service->resource_id = 0;
	}

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_browse_service(const char *target,
		unsigned int *handler, ssdp_browse_cb browse_cb, void *user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

GLIST_ITER_START(g_ssdp_services, service)
	if(strncmp(target, service->target, strlen(target)) &&
			service->origin == SSDP_SERVICE_TYPE_BROWSED) {
		SSDP_LOGD("Browsing request is already registered");
	} else {
		service = NULL;
	}
GLIST_ITER_END()

	if(service == NULL) {
		service = (ssdp_service_s *)g_try_malloc0(sizeof(ssdp_service_s));
		if(!service) {
			SSDP_LOGE("Failed to get memory for gssdp service structure");
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OUT_OF_MEMORY;
		}

		service->target = g_strndup(target, strlen(target));
		if(!service->target) {
			SSDP_LOGE("Failed to get memory for gssdp service target");
			g_free(service);
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OUT_OF_MEMORY;
		}
		*handler = (unsigned int)service & 0xffffffff;
		service->service_handler = (unsigned int)service & 0xffffffff;

		/* Create the service brower */
		if(!(service->resource_browser = gssdp_resource_browser_new(
				g_gssdp_client,
				service->target)))
		{
			SSDP_LOGE("Failed to create service browser\n");
			g_free(service->target);
			g_free(service);
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OPERATION_FAILED;
		}

		g_signal_connect (service->resource_browser,
				"resource-available",
				G_CALLBACK (__nsd_provider_ssdp_res_available_cb),
				service);

		g_signal_connect (service->resource_browser,
				"resource-unavailable",
				G_CALLBACK (__nsd_provider_ssdp_res_unavailable_cb),
				service);
		gssdp_resource_browser_set_active (service->resource_browser, TRUE);

		service->origin = SSDP_SERVICE_TYPE_BROWSED;
		g_ssdp_services = g_list_append(g_ssdp_services, service);
		SSDP_LOGD("service handler [%u]", *handler);
	}	else {
		/* Request rescan */
		if(!(gssdp_resource_browser_rescan(service->resource_browser)))
		{
			SSDP_LOGE("Failed to request rescan");
			__SSDP_LOG_FUNC_EXIT__;
			return SSDP_ERROR_OPERATION_FAILED;
		}
	}

	service->browse_cb = browse_cb;
	service->browse_cb_user_data = user_data;

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_stop_browse_service(unsigned int handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	g_object_unref (service->resource_browser);

	g_ssdp_services = g_list_remove(g_ssdp_services, service);

	g_hash_table_foreach(g_found_ssdp_services,
			(GHFunc)__g_hash_remove_related_services,
			(gpointer)service->service_handler);
	g_free(service->target);
	g_free(service);

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_set_usn(const char *usn, unsigned int handler)
{
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->origin != SSDP_SERVICE_TYPE_REGISTERED)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	g_free(service->usn);
	service->usn = g_strndup(usn, strlen(usn));
	//TODO : renew the advertising service if device already announced

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_set_url(const char *url, unsigned int handler)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	if(service->origin != SSDP_SERVICE_TYPE_REGISTERED)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	g_free(service->url);
	service->url = g_strndup(url, strlen(url));
	//TODO : renew the advertising service if device already announced

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_get_target(unsigned int handler, char **target)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, handler);

	if(service == NULL || service->target == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	*target = g_strndup(service->target, strlen(service->target));

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}
GHashTableIter iter;

int nsd_provider_ssdp_get_usn(unsigned int handler, char **usn)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, handler);

	if(service == NULL || service->usn == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	*usn = g_strndup(service->usn, strlen(service->usn));

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}

int nsd_provider_ssdp_get_url(unsigned int handler, char **url)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	service = __ssdp_find_service(g_ssdp_services, handler);
	if(service == NULL)
		service = __ssdp_find_remote_service(g_found_ssdp_services, handler);

	if(service == NULL || service->url == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	*url = g_strndup(service->url, strlen(service->url));

	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}
/*
int nsd_provider_ssdp_service_foreach_found(unsigned int browse_service,
		ssdp_found_service_cb callback, void * user_data)
{
	__SSDP_LOG_FUNC_ENTER__;
	ssdp_service_s *service = NULL;
	GHashTableIter iter;
	gpointer key, value;

	if(!g_is_gssdp_init) {
		SSDP_LOGE("gssdp not initialized");
		return SSDP_ERROR_NOT_INITIALIZED;
	}

	SSDP_LOGE("Find service");

	service = __ssdp_find_service(g_ssdp_services, browse_service);
	if(service == NULL)
		return SSDP_ERROR_SERVICE_NOT_FOUND;
	if(service->origin != SSDP_SERVICE_TYPE_BROWSED)
		return SSDP_ERROR_SERVICE_NOT_FOUND;

	SSDP_LOGE("Found service");

	g_hash_table_iter_init(&iter, g_found_ssdp_services);
	while(g_hash_table_iter_next(&iter, &key, &value)) {

		SSDP_LOGD("handler [%u]", ((ssdp_service_s *)value)->service_handler);
		SSDP_LOGD("usn [%s]", ((ssdp_service_s *)value)->usn);

		if(((ssdp_service_s *)value)->service_id == browse_service) {
			service = (ssdp_service_s *)value;

			if(callback)
				callback(service->target, service->usn, service->url,
						user_data);
		}
	}
	__SSDP_LOG_FUNC_EXIT__;
	return SSDP_ERROR_NONE;
}
*/
