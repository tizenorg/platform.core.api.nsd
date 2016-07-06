#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dns-sd.h"

gboolean test_thread(GIOChannel *source, GIOCondition condition, gpointer data);

static bool test_get_user_string(const char *msg, char *buf, int buf_size)
{
	if (msg == NULL || buf == NULL || buf_size < 2)
		return false;

	int i = 0, c;
	printf("%s\n", msg);
	memset(buf, 0, buf_size);
	while (true) {
		c = getchar();
		if (i == 0 && c == '\n')
			continue;

		if (c == '\n' || c == EOF)
			break;

		buf[i++] = c;
		if (i == buf_size-1)
			break;
	}

	buf[i] = '\0';
	return true;
}

static bool test_get_user_bytes(const char *msg, char *buf, int buf_size)
{
	if (msg == NULL || buf == NULL || buf_size < 2)
		return false;

	int i = 0, c;
	printf("%s\n", msg);
	memset(buf, 0, buf_size);
	while (true) {
		c = getchar();
		if (i == 0 && c == '\n')
			continue;

		if (c == '\n' || c == EOF)
			break;

		buf[i++] = c;
		if (i == buf_size-1)
			break;
	}

	return true;
}

static const char *dnssd_error_to_string(dnssd_error_e error)
{
	switch (error) {
	case DNSSD_ERROR_NONE:
		return "DNSSD_ERROR_NONE";
	case DNSSD_ERROR_OUT_OF_MEMORY:
		return "DNSSD_ERROR_OUT_OF_MEMORY";
	case DNSSD_ERROR_INVALID_PARAMETER:
		return "DNSSD_ERROR_INVALID_PARAMETER";
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
	case DNSSD_ERROR_OPERATION_FAILED:
		return "DNSSD_ERROR_OPERATION_FAILED";
	default:
		return "UNSUPPORTED_ERROR";
	}
}

static void show_txt_record(unsigned short txt_len, const unsigned char *txt_record)
{
	const unsigned char *ptr = txt_record;
	const unsigned char *max = txt_record + txt_len;
	while (ptr < max) {
		const unsigned char *const end = ptr + 1 + ptr[0];
		if (end > max) {
			printf("<< invalid data >>");
			break;
		}
		if (++ptr < end)
			printf(" ");   /* As long as string is non-empty, begin with a space */
		while (ptr < end) {
			if (strchr(" &;`'\"|*?~<>^()[]{}$", *ptr))
				printf("\\");
			if (*ptr == '\\')
				printf("\\\\\\\\");
			else if (*ptr >= ' ')
				printf("%c", *ptr);
			else
				printf("\\\\x%02X", *ptr);
			ptr++;
		}
	}
	printf("\n");
}

int test_dnssd_initialize()
{
	int rv;
	printf("Initialize DNS SD CAPI\n");

	rv = dnssd_initialize();
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to Initialize DNS SD CAPI [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Successfully Initialized DNS SD CAPI\n");
	return 1;
}

int test_dnssd_deinitialize()
{
	int rv;
	printf("De-Initialize DNS SD CAPI\n");

	rv = dnssd_deinitialize();
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to De-Initialize DNS SD CAPI [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Successfully De-Initialized DNS SD CAPI\n");
	return 1;
}

int test_dnssd_create_local_service()
{
	int rv = -1;

	dnssd_service_h service = 0;
	char service_type[255];
	printf("Create DNS Service\n");
	test_get_user_string("Enter type:(Example : _http._tcp, "
			"_printer._tcp etc)", service_type, 255);
	rv = dnssd_create_local_service(service_type, &service);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to create DNS SD Service, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Successfully created DNS SD Service[%u]\n", service);
	return 1;
}

int test_dnssd_destroy_local_service()
{
	int rv = -1;
	dnssd_service_h service = 0;
	printf("Destroy DNS Service\n");
	printf("Enter Local Service Handle:\n");
	rv = scanf("%u", &service);
	rv = dnssd_destroy_local_service(service);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to destroy DNS SD Service, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Successfully destroyed DNS SD Service\n");
	return 1;
}

static void dnssd_register_reply(dnssd_error_e result,
		dnssd_service_h local_service, void *user_data)
{
	printf("Register Callback\n");
	printf("Handler  : %u\n", local_service);
	printf("State : ");
	switch (result) {
	case DNSSD_ERROR_NONE:
		printf("DNSSD_REGISTERED\n");
		break;
	case DNSSD_ERROR_ALREADY_REGISTERED:
		printf("DNSSD_ERROR_ALREADY_REGISTERED\n");
		break;
	case DNSSD_ERROR_NAME_CONFLICT:
		printf("DNSSD_ERROR_NAME_CONFLICT\n");
		break;
	default:
		printf("Unknown State\n");
	}
}

int test_dnssd_register_local_service()
{
	dnssd_service_h service = 0;
	int rv;
	char service_name[255] = {0,};
	int port;

	printf("Register DNS Service\n");
	printf("Enter Local Service Handler:\n");
	rv = scanf("%u", &service);

	test_get_user_string("Enter Service Name:", service_name, 255);
	rv = dnssd_service_set_name(service, service_name);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to set name [%s]\n", dnssd_error_to_string(rv));
		return 0;
	}

	printf("Enter port:\n");
	rv = scanf("%d", &port);
	rv = dnssd_service_set_port(service, port);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to set port [%s]\n", dnssd_error_to_string(rv));
		return 0;
	}

	rv = dnssd_register_local_service(service, dnssd_register_reply, NULL);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to register for dns service, error [%s]",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Succeeded to register for dns service");
	return 1;
}

int test_dnssd_deregister_local_service()
{
	dnssd_service_h service = 0;
	int rv;

	printf("De-register DNS Service\n");
	printf("Enter Local Service Handler:\n");
	rv = scanf("%u", &service);

	rv = dnssd_deregister_local_service(service);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to deregister dns service, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Successfully deregistered dns service\n");
	return 1;
}

int test_dnssd_service_get_type()
{
	int rv = -1;
	dnssd_service_h service;
	char * type = NULL;
	printf("Get DNS-SD service type\n");
	printf("Enter Service Handler:\n");
	rv = scanf("%u", &service);
	rv = dnssd_service_get_type(service, &type);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to get DNS SD Service type, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	if (type)
		printf("Service type [%s]\n", type);
	g_free(type);
	printf("Successfully get DNS SD Service type\n");
	return 1;
}

int test_dnssd_service_get_name()
{
	int rv = -1;
	dnssd_service_h service;
	char * name = NULL;
	printf("Get DNS-SD Service Name\n");
	printf("Enter Service Handler:\n");
	rv = scanf("%u", &service);
	rv = dnssd_service_get_name(service, &name);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to get DNS SD Service name, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	if (name)
		printf("Service name [%s]\n", name);
	g_free(name);
	printf("Successfully get DNS SD Service name\n");
	return 1;
}

int test_dnssd_service_get_ip()
{
	int rv = -1;
	dnssd_service_h service;
	char * ip_v4_address = NULL;
	char * ip_v6_address = NULL;
	printf("Get DNS-SD Service IP\n");
	printf("Enter Service Handler:\n");
	rv = scanf("%u", &service);
	rv = dnssd_service_get_ip(service, &ip_v4_address, &ip_v6_address);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to get DNS SD Service IP, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Service IP v4 [%s]\n", ip_v4_address);
	printf("Service IP v6 [%s]\n", ip_v6_address);
	g_free(ip_v4_address);
	g_free(ip_v6_address);
	printf("Successfully get DNS SD Service IP\n");
	return 1;
}

int test_dnssd_service_get_port()
{
	int rv = -1;
	dnssd_service_h service;
	int port = 0;
	printf("Get DNS-SD Service Port\n");
	printf("Enter Service Handler:\n");
	rv = scanf("%u", &service);
	rv = dnssd_service_get_port(service, &port);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to get DNS SD Service port, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Service port [%d]\n", port);
	printf("Successfully get DNS SD Service port\n");
	return 1;
}

int test_dnssd_service_set_record()
{
	dnssd_service_h service;
	int rv;
	unsigned short type, length, buf_len;
	void *data;
	char buffer[255];
	char key[10];
	printf("Add record to DNS SD Service\n");
	printf("Enter Local Service Handle:\n");
	rv = scanf("%u", &service);

	printf("Enter type of Resource Record:\n");
	rv = scanf("%hu", &type);

	printf("Enter length of Resource Record:\n");
	rv = scanf("%hu", &buf_len);

	test_get_user_bytes("Enter record data:", buffer, 255);
	test_get_user_string("Enter TXT Record key:", key, 10);

	rv = dnssd_service_add_txt_record(service, key, buf_len, buffer);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to set txt record value\n");
		return 0;
	}

	rv = dnssd_service_get_all_txt_record(service, &length, &data);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to get txt record value\n");
		return 0;
	}

	rv = dnssd_service_set_record(service, type, length, data);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to add record\n");
		return 0;
	}

	printf("Successfully added record\n");

	return 1;
}

int test_dnssd_service_unset_record()
{
	dnssd_service_h service;
	int rv;
	unsigned short type;
	printf("Remove record from DNS SD Service\n");
	printf("Enter Local Service Handle:\n");
	rv = scanf("%u", &service);

	printf("Enter type of Resource Record:\n");
	rv = scanf("%hu", &type);

	rv = dnssd_service_unset_record(service, type);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to remove record\n");
		return 0;
	}

	printf("Successfully removed record\n");
	return 1;
}

static void dnssd_browse_reply(dnssd_service_state_e service_state,
		dnssd_service_h remote_service, void *user_data)
{
	int rv = 0;
	printf("Browse Service Callback\n");
	printf("Handler       : %u\n", remote_service);
	printf("State         : ");
	switch (service_state) {
	case DNSSD_SERVICE_STATE_AVAILABLE:
		printf("Available\n");
		break;
	case DNSSD_SERVICE_STATE_UNAVAILABLE:
		printf("Un-Available\n");
		break;
	case DNSSD_SERVICE_STATE_NAME_LOOKUP_FAILED:
		printf("Lookup failure for service name\n");
		break;
	case DNSSD_SERVICE_STATE_HOST_NAME_LOOKUP_FAILED:
		printf("Lookup failure for host name and port number\n");
		break;
	case DNSSD_SERVICE_STATE_ADDRESS_LOOKUP_FAILED:
		printf("Lookup failure for IP address\n");
		break;
	default:
		printf("Unknown Browse State\n");
		break;
	}

	char *name = NULL;
	char *type = NULL;

	rv = dnssd_service_get_name(remote_service, &name);
	if (rv == DNSSD_ERROR_NONE && name != NULL)
		printf("Service Name  : %s\n", name);

	rv = dnssd_service_get_type(remote_service, &type);
	if (rv == DNSSD_ERROR_NONE && type != NULL)
		printf("Service Type  : %s\n", type);

	if (service_state == DNSSD_SERVICE_STATE_AVAILABLE) {
		char *ip_v4_address = NULL;
		char *ip_v6_address = NULL;
		unsigned char *txt_record = NULL;
		unsigned short txt_len = 0;
		int port = 0;

		rv = dnssd_service_get_ip(remote_service, &ip_v4_address, &ip_v6_address);
		if (rv  == DNSSD_ERROR_NONE) {
			if (ip_v4_address)
			printf("IPv4 Address  : %s\n", ip_v4_address);
			if (ip_v6_address)
			printf("IPv6 Address  : %s\n", ip_v6_address);
		}

		rv = dnssd_service_get_port(remote_service, &port);
		printf("Port          : %d\n", port);

		printf("TXT Record    : ");
		dnssd_service_get_all_txt_record(remote_service, &txt_len,
				(void *)&txt_record);
		show_txt_record(txt_len, txt_record);

		g_free(ip_v4_address);
		g_free(ip_v6_address);
		g_free(txt_record);
	}
	g_free(name);
	g_free(type);
}

int test_dnssd_start_browsing_service()
{
	dnssd_service_h service;
	char service_type[255];
	int rv;

	printf("Browse DNS Service\n");
	test_get_user_string("Enter type:(Example : _http._tcp, "
			"_printer._tcp etc)", service_type, 255);

	rv = dnssd_start_browsing_service(service_type, &service,
			dnssd_browse_reply, NULL);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to browse for dns service, error %s",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Succeeded to browse for dns service[%u]\n", service);
	return 1;
}

int test_dnssd_stop_browsing_service()
{
	dnssd_service_h service;
	int rv;
	printf("Enter Browse Service Handle:\n");
	rv = scanf("%u", &service);

	rv = dnssd_stop_browsing_service(service);
	if (rv != DNSSD_ERROR_NONE) {
		printf("Failed to stop browse dns service %s\n",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Successfully stopped browsing dns service\n");
	return 1;
}

static void MenuScreen()
{
	printf("_____________________________________\n");
	printf("|_____________MENU-SCREEN___________|\n");
	printf("| 0 - EXIT                          |\n");
	printf("| 1 - Initialize DNS SD             |\n");
	printf("| 2 - De-Initialize DNS SD          |\n");
	printf("| 3 - DNS SD Create Service         |\n");
	printf("| 4 - DNS SD Destroy Service        |\n");
	printf("| 5 - Register DNS Service          |\n");
	printf("| 6 - De-Register DNS Service       |\n");
	printf("| 7 - Add Record to DNS Service     |\n");
	printf("| 8 - Remove Registered Record      |\n");
	printf("| 9 - Browse DNS Service            |\n");
	printf("| a - Stop Browse DNS Service       |\n");
	printf("|___________________________________|\n");
}

int test_thread(GIOChannel *source, GIOCondition condition, gpointer data)
{
	int rv;
	char a[10];
	GMainLoop *mainloop = data;

	printf("Event received from stdin\n");

	rv = read(0, a, 10);
	if (rv <= 0 || a[0] == '0') {
		printf("Terminating  Test Application");
		g_main_loop_quit(mainloop);
		rv = 1;
	}

	if (a[0] == '\n' || a[0] == '\r') {
		printf("\n\n mDNSResponder Test App\n\n");
		printf("Options..\n");
		MenuScreen();
		printf("Press ENTER to show options menu.......\n");
	}

	switch (a[0]) {
	case '1':
		rv = test_dnssd_initialize();
		break;
	case '2':
		rv = test_dnssd_deinitialize();
		break;
	case '3':
		rv = test_dnssd_create_local_service();
		break;
	case '4':
		rv = test_dnssd_destroy_local_service();
		break;
	case '5':
		rv = test_dnssd_register_local_service();
		break;
	case '6':
		rv = test_dnssd_deregister_local_service();
		break;
	case '7':
		rv = test_dnssd_service_set_record();
		break;
	case '8':
		rv = test_dnssd_service_unset_record();
		break;
	case '9':
		rv = test_dnssd_start_browsing_service();
		break;
	case 'a':
		rv = test_dnssd_stop_browsing_service();
		break;
	}

	if (rv == 1)
		printf("\nOperation Succeeded!\n");
	else
		printf("\nOperation Failed!\n");

	return TRUE;

}

int main(int argc, char **argv)
{
	GMainLoop *mainloop;
	mainloop = g_main_loop_new(NULL, FALSE);

	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL),
			test_thread, mainloop);

	printf("Test Thread created...\n");

	g_main_loop_run(mainloop);

	printf("Test Application Terminated\n");
	g_main_loop_unref(mainloop);

	return 0;
}
