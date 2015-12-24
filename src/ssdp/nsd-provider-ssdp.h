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

#ifndef __NSD_PROVIDER_SSDP_H__
#define __NSD_PROVIDER_SSDP_H__

int nsd_provider_ssdp_initialize(void);
int nsd_provider_ssdp_deinitialize(void);

int nsd_provider_ssdp_create_service(ssdp_type_e op_type, const char *target, unsigned int *handler);
int nsd_provider_ssdp_destroy_service(unsigned int handler);

int nsd_provider_ssdp_register_service(unsigned int handler,
		ssdp_register_cb register_cb, void *user_data);
int nsd_provider_ssdp_deregister_service(unsigned int handler);

int nsd_provider_ssdp_browse_service(const char *target,
		unsigned int *handler, ssdp_browse_cb browse_cb, void *user_data);
int nsd_provider_ssdp_stop_browse_service(unsigned int handler);

int nsd_provider_ssdp_set_usn(const char *usn, unsigned int handler);
int nsd_provider_ssdp_set_url(const char *url, unsigned int handler);

int nsd_provider_ssdp_get_target(unsigned int handler, char **target);
int nsd_provider_ssdp_get_usn(unsigned int handler, char **usn);
int nsd_provider_ssdp_get_url( unsigned int handler, char **url);
/*
int nsd_provider_ssdp_service_foreach_found(unsigned int browse_service,
		ssdp_found_service_cb callback, void * user_data);
*/
#endif /* __NSD_PROVIDER_SSDP_H__ */
