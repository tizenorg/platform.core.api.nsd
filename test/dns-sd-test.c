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

dnssd_service_h register_service;
dnssd_service_h browse_service;

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

		if (c == '\n')
			break;

		buf[i++] = c;
		if (i == buf_size-1)
			break;
	}

	buf[i]='\0';
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

		if (c == '\n')
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

int test_dnssd_initialize()
{
	int rv;
	printf("Initialize mDNSResponder Daemon\n");

	rv = dnssd_initialize();
	if(rv < 0) {
		printf("Failed to Initialize Daemon\n");
		return rv;
	}
	printf("Successfully Initialized Daemon\n");
	return 1;
}

int test_dnssd_deinitialize()
{
	int rv;
	printf("De-Initialize mDNSResponder Daemon\n");

	rv = dnssd_deinitialize();
	if(rv < 0) {
		printf("Failed to De-Initialize Daemon\n");
		return rv;
	}
	printf("Successfully De-Initialized Daemon\n");
	return 1;
}

int test_dnssd_create_service()
{
	int rv = -1;
	char service_type[255];
	int operation_type;
	printf("Create DNS Service\n");
	printf("Enter operation type:\nRegister -> 1\nBrowse   -> 2\n");
	rv = scanf("%d", &operation_type);
	test_get_user_string("Enter type:(Example : _http._tcp, _printer._tcp etc)",
			service_type, 255);
	if (operation_type == 1)
		rv = dnssd_create_service(operation_type, service_type,
				&register_service);
	else if (operation_type == 2)
		rv = dnssd_create_service(operation_type, service_type,
				&browse_service);
	else {
		printf("Invalid Operation Type\n");
		return -1;
	}
	if (rv < 0) {
		printf("Failed to create DNS SD Service, error [%s]\n",
				dnssd_error_to_string(rv));
		return rv;
	}

	printf("Successfully created DNS SD Service\n");
	return 1;
}

int test_dnssd_destroy_service()
{
	int rv = -1;
	int operation_type;
	printf("Destroy DNS Service\n");
	printf("Enter operation type:\nRegister -> 1\nBrowse   -> 2\n");
	rv = scanf("%d", &operation_type);
	if (operation_type == 1)
		rv = dnssd_destroy_service(register_service);
	else if (operation_type == 2)
		rv = dnssd_destroy_service(browse_service);
	else {
		printf("Invalid Operation Type\n");
		return -1;
	}
	if (rv < 0) {
		printf("Failed to destroy DNS SD Service, error [%s]\n",
				dnssd_error_to_string(rv));
		return rv;
	}

	printf("Successfully destroyed DNS SD Service\n");
	return 1;
}

static void dnssd_register_reply(dnssd_register_state_e register_state,
		dnssd_service_h dnssd_service, void *user_data)
{
	char *service_name;
	printf("Register Callback\n");
	dnssd_get_name(dnssd_service, &service_name);
	printf("Name  : %s\n", service_name);
	printf("State : ");
	switch (register_state) {
	case DNSSD_REGISTERED:
		printf("DNSSD_REGISTERED\n");
		break;
	case DNSSD_ALREADY_REGISTERED:
		printf("DNSSD_ALREADY_REGISTERED\n");
		break;
	case DNSSD_NAME_CONFLICT:
		printf("DNSSD_NAME_CONFLICT\n");
		break;
	default:
		printf("Unknown State\n");
	}
	g_free(service_name);
}

int test_dnssd_register_service()
{
	int rv;
	char service_name[255] = {0,};
	int port;

	printf("Register DNS Service\n");

	test_get_user_string("Enter Service Name:", service_name, 255);
	dnssd_set_name(register_service, service_name);

	printf("Enter port:\n");
	rv = scanf("%d", &port);
	dnssd_set_port(register_service, port);

	rv = dnssd_register_service(register_service, dnssd_register_reply,
			NULL);

	if (rv < 0) {
		printf("Failed to register for dns service, error [%s]",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Succeeded to register for dns service");
	return 1;
}

int test_dnssd_deregister_service()
{
	int rv;

	printf("De-register DNS Service\n");

	rv = dnssd_deregister_service(register_service);
	if(rv < 0) {
		printf("Failed to deregister dns service, error [%s]\n",
				dnssd_error_to_string(rv));
		return 0;
	}
	printf("Successfully deregistered dns service\n");
	return 1;
}

int test_dnssd_add_record()
{
	int rv;
	unsigned short rrtype, rlen, buf_len;
	const void *data;
	char buffer[255];
	char key[10];
	printf("Add record to DNS SD Service\n");

	printf("Enter type of record:\n");
	rv = scanf("%hu", &rrtype);

	printf("Enter length of record:\n");
	rv = scanf("%hu", &buf_len);

	test_get_user_bytes("Enter record data:", buffer, 255);
	test_get_user_string("Enter TXT Record key:", key, 10);

	rv = dnssd_set_txt_record_value(register_service, key, buf_len, buffer);
	if (rv < 0) {
		printf("Failed to set txt record value\n");
		return 0;
	}

	rv = dnssd_get_txt_record_value(register_service, &rlen, &data);
	if (rv < 0) {
		printf("Failed to get txt record value\n");
		return 0;
	}

	rv = dnssd_add_record(register_service, rrtype, rlen, data);
	if (rv < 0) {
		printf("Failed to add record\n");
		return 0;
	}

	rv = dnssd_unset_txt_record_value(register_service, key);
	if (rv < 0) {
		printf("Failed to unset txt record value\n");
		return 0;
	}

	printf("Successfully added record\n");

	return 1;
}

int test_dnssd_remove_record()
{
	int rv;
	unsigned short rrtype;
	printf("Remove record from DNS SD Service\n");

	printf("Enter type of record:\n");
	rv = scanf("%hu", &rrtype);

	rv = dnssd_remove_record(register_service, rrtype);
	if (rv < 0) {
		printf("Failed to remove record\n");
		return 0;
	}

	printf("Successfully removed record\n");
	return 1;
}

int test_dnssd_update_record()
{
	int rv;
	unsigned short rrtype, rlen, buf_len;
	const void *data;
	char buffer[255];
	char key[10];

	printf("Update record in DNS SD Service\n");

	printf("Enter type of record:\n");
	rv = scanf("%hu", &rrtype);

	printf("Enter length of record:\n");
	rv = scanf("%hu", &buf_len);

	test_get_user_bytes("Enter record data:", buffer, 255);
	test_get_user_string("Enter TXT Record key:", key, 10);

	rv = dnssd_set_txt_record_value(register_service, key, buf_len, buffer);
	if (rv < 0) {
		printf("Failed to set txt record value\n");
		return 0;
	}

	rv = dnssd_get_txt_record_value(register_service, &rlen, &data);
	if (rv < 0) {
		printf("Failed to get txt record value\n");
		return 0;
	}

	rv = dnssd_update_record(register_service, rrtype, rlen, data);
	if (rv < 0) {
		printf("Failed to update record\n");
		return 0;
	}

	rv = dnssd_unset_txt_record_value(register_service, key);
	if (rv < 0) {
		printf("Failed to unset txt record value\n");
		return 0;
	}

	printf("Successfully updated record\n");

	return 1;
}

static void show_txt_record(unsigned short txt_len, const char *txt_record)
{
	const char *ptr = txt_record;
	const char *max = txt_record + txt_len;
	while (ptr < max) {
		const char *const end = ptr + 1 + ptr[0];
		if (end > max) {
			printf("<< invalid data >>");
			break;
		}
		if (++ptr < end)
			printf(" ");
		while (ptr < end) {
			if (*ptr >= ' ')
				printf("%c", *ptr);
			ptr++;
		}
	}
	printf("\n");
}

static void dnssd_browse_reply(dnssd_browse_state_e browse_state,
		dnssd_service_h dnssd_service, void *user_data)
{
	printf("Browse Service Callback\n");
#if 0
	printf("Name       : %s\n", service_name);
#endif
	printf("State      : ");
	switch (browse_state) {
	case DNSSD_SERVICE_AVAILABLE:
		printf("Available\n");
		break;
	case DNSSD_SERVICE_UNAVAILABLE:
		printf("Un-Available\n");
		break;
	case DNSSD_BROWSE_FAILURE:
		printf("Browse Failure\n");
		break;
	case DNSSD_RESOLVE_SERVICE_NAME_FAILURE:
		printf("Resolve Service Name Failure\n");
		break;
	case DNSSD_RESOLVE_ADDRESS_FAILURE:
		printf("Resolve Service Address\n");
		break;
	default:
		printf("Unknown Browse State\n");
		break;
	}
#if 0
	if (browse_state == DNSSD_SERVICE_AVAILABLE) {
		printf("IP Address : %s\n", ip_address);
		printf("Port       : %d\n", port);
		printf("Hostname   : %s\n", hostname);
		printf("TXT Record : ");
		show_txt_record(txt_len, txt_record);
	}
#endif
}

int test_dnssd_browse_service()
{
	int rv;

	printf("Browse DNS Service\n");

	rv = dnssd_browse_service(browse_service, dnssd_browse_reply, NULL);
	if (rv < 0) {
		printf("Failed to browse for dns service, error %s",
				dnssd_error_to_string(rv));
		return 0;
	}

	printf("Succeeded to browse for dns service");
	return 1;
}

int test_dnssd_stop_browse_service()
{
	int rv;

	rv = dnssd_stop_browse_service(browse_service);
	if(rv < 0) {
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
	printf("| 1 - DNS SD Create Service         |\n");
	printf("| 2 - DNS SD Destroy Service        |\n");
	printf("| 3 - Register DNS Service          |\n");
	printf("| 4 - De-Register DNS Service       |\n");
	printf("| 5 - Add Record to DNS Service     |\n");
	printf("| 6 - Update Registered Record      |\n");
	printf("| 7 - Remove Registered Record      |\n");
	printf("| 8 - Browse DNS Service            |\n");
	printf("| 9 - Stop Browse DNS Service       |\n");
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
		rv = test_dnssd_create_service();
		break;
	case '2':
		rv = test_dnssd_destroy_service();
		break;
	case '3':
		rv = test_dnssd_register_service();
		break;
	case '4':
		rv = test_dnssd_deregister_service();
		break;
	case '5':
		rv = test_dnssd_add_record();
		break;
	case '6':
		rv = test_dnssd_update_record();
		break;
	case '7':
		rv = test_dnssd_remove_record();
		break;
	case '8':
		rv = test_dnssd_browse_service();
		break;
	case '9':
		rv = test_dnssd_stop_browse_service();
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
	mainloop = g_main_loop_new (NULL, FALSE);

	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL),
			test_thread, mainloop);

	printf("Test Thread created...\n");

	g_main_loop_run (mainloop);

	printf("Test Application Terminated\n");
	g_main_loop_unref(mainloop);

	return 0;
}
