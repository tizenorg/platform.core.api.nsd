/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_NETWORK_SERVICE_DISCOVERY_DOC_H__
#define __TIZEN_NETWORK_SERVICE_DISCOVERY_DOC_H__

/**
 * @defgroup CAPI_NETWORK_SSDP_MODULE SSDP
 * @brief The SSDP API provides functions for network service discovery using SSDP.
 * @ingroup CAPI_NETWORK_FRAMEWORK
 *
 * @section CAPI_NETWORK_SSDP_MODULE_HEADER Required Header
 *   \#include <ssdp.h>
 *
 * @section CAPI_NETWORK_SSDP_MODULE_OVERVIEW Overview
 * SSDP allows your application to create and register a local service.
 * You should deregister and destroy the created local service handle if it is not available.
 * In addition, you can browse services on a network using SSDP API.
 * You should stop browsing if you do not need it anymore.
 * @section CAPI_NETWORK_SSDP_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.service_discovery.ssdp
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/getting-started/native-application/understanding-tizen-programming/application-filtering"><b>Feature List</b>.</a>
 *
 */

/**
 * @defgroup CAPI_NETWORK_DNSSD_MODULE DNSSD
 * @brief The DNSSD API provides functions for network service discovery using DNSSD.
 * @ingroup CAPI_NETWORK_FRAMEWORK
 *
 * @section CAPI_NETWORK_DNSSD_MODULE_HEADER Required Header
 *   \#include <ssdp.h>
 *
 * @section CAPI_NETWORK_DNSSD_MODULE_OVERVIEW Overview
 * DNSSD allows your application to create and register a local service.
 * You should deregister and destroy the created local service handle if it is not available.
 * In addition, you can browse services on a network using DNSSD API.
 * You should stop browsing if you do not need it anymore.
 * @section CAPI_NETWORK_DNSSD_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 * - http://tizen.org/feature/network.service_discovery.dnssd
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/getting-started/native-application/understanding-tizen-programming/application-filtering"><b>Feature List</b>.</a>
 *
 */
