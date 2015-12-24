#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#include <glib.h>
#include <glib-object.h>

#include <ssdp.h>

#define RESET_COLOR "\e[m"
#define MAKE_RED "\e[31m"
#define MAKE_GREEN "\e[32m"

#define MENU_PRINT(str) printf("%s\n", str)

#define __FUNC_ENTER__ printf("\n%s() entering...\n", __func__)
#define __FUNC_EXIT__ printf("\n%s() leaving...\n", __func__)

void __ssdp_register_cb (ssdp_service_h ssdp_service,
		ssdp_register_state_e register_state, void *user_data)
{
	printf(MAKE_GREEN"cb %s "RESET_COLOR"\n", __func__ );
	if(register_state == SSDP_REGISTERED)
		printf("[%u] SSDP_REGISTERED\n", ssdp_service);
	else if(register_state == SSDP_REGISTER_FAILURE)
		printf("[%u]SSDP_REGISTER_FAILURE\n", ssdp_service);

	__FUNC_EXIT__;
	return;
}

void __ssdp_browse_cb (ssdp_service_h ssdp_service,
		ssdp_browse_state_e state, void *user_data)
{
	printf(MAKE_GREEN"\ncb %s "RESET_COLOR"\n", __func__ );


	if(state == SSDP_SERVICE_AVAILABLE)
		printf("[%u] SSDP_SERVICE_AVAILABLE\n", ssdp_service);
	else if(state == SSDP_SERVICE_UNAVAILABLE)
		printf("[%u]SSDP_SERVICE_UNAVAILABLE\n", ssdp_service);

	__FUNC_EXIT__;
	return;
}

int test_ssdp_initialize()
{
	__FUNC_ENTER__;
	int rv = 0;

	rv = ssdp_initialize();
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_deinitialize()
{
	__FUNC_ENTER__;
	int rv = 0;

	rv = ssdp_deinitialize();
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_create_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	char *target = NULL;
	ssdp_service_h serv_id;
	int op_type = 0;

	printf("\nEnter target: (Example : upnp:rootdevice)");
	if(scanf("%ms", &target) < 1)
		return -1;

	printf("\nEnter OP type: (Example : SSDP_TYPE_REGISTER: 1 SSDP_TYPE_BROWSE: 2)");
	if(scanf("%ms", &op_type) < 1)
		return -1;

	rv = ssdp_create_service(op_type, target, &serv_id);
	g_free(target);
	printf("service handler [%u]\n", serv_id);

	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_destroy_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_destroy_service(serv_id);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_set_usn()
{
	__FUNC_ENTER__;
	int rv = 0;
	char *usn = NULL;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	printf("\nEnter usn: (Example : uuid:1234abcd-12ab-12ab-12ab-1234567abc12::upnp:rootdevice)");
	if(scanf("%ms", &usn) < 1)
		return -1;

	rv = ssdp_set_usn(serv_id, usn);
	g_free(usn);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_set_url()
{
	__FUNC_ENTER__;
	int rv = 0;
	char *url = NULL;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	printf("\nEnter url: (Example : http://192.168.0.110/)");
	if(scanf("%ms", &url) < 1)
		return -1;

	rv = ssdp_set_url(serv_id, url);
	g_free(url);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_get_target()
{
	__FUNC_ENTER__;
	int rv = 0;
	char *target = NULL;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_get_target(serv_id, &target);
	if(rv == 0) {
		printf("target [%s]\n", target);
		g_free(target);
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_get_usn()
{
	__FUNC_ENTER__;
		int rv = 0;
	char *usn = NULL;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_get_usn(serv_id, &usn);
	if(rv == 0) {
		printf("usn [%s]\n", usn);
		g_free(usn);
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	return -1;
}

int test_ssdp_get_url()
{
	__FUNC_ENTER__;
	int rv = 0;
	char *url = NULL;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_get_url(serv_id, &url);
	if(rv == 0) {
		printf("URL [%s]\n", url);
		g_free(url);
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_register_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_register_service(serv_id, __ssdp_register_cb, NULL);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_deregister_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_deregister_service(serv_id);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_browse_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	ssdp_service_h serv_id = 0;
	char *target = NULL;

	printf("\nEnter target: (Example : upnp:rootdevice)");
	if(scanf("%ms", &target) < 1)
		return -1;

	rv = ssdp_browse_service(target, &serv_id, __ssdp_browse_cb, NULL);
	g_free(target);
	printf("browse service id [%u]\n", serv_id);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

int test_ssdp_stop_browse_service()
{
	__FUNC_ENTER__;
	int rv = 0;
	ssdp_service_h serv_id = 0;

	printf("\nEnter service id: ");
	if(scanf("%u", &serv_id) < 1)
		return -1;

	rv = ssdp_stop_browse_service(serv_id);
	if(rv == 0) {
		printf(MAKE_GREEN"Success %s "RESET_COLOR"\n", __func__ );
		__FUNC_EXIT__;
		return 1;
	}

	printf(MAKE_RED"FAILED %s"RESET_COLOR"\n", __func__);
	__FUNC_EXIT__;
	return -1;
}

static void MenuScreen()
{
	MENU_PRINT("________________________________________________________________________________________________________");
	MENU_PRINT("|_______________________________________________ MENU-SCREEN __________________________________________|");
	MENU_PRINT("| 0 - EXIT                        |                                  |                                 |");
	MENU_PRINT("| 1 - Initialize SSDP             |                                  |                                 |");
	MENU_PRINT("| 2 - Deinitialize SSDP           |                                  |                                 |");
	MENU_PRINT("| 3 - Register SSDP Service       |                                  |                                 |");
	MENU_PRINT("| 4 - Deregister SSDP Service     |                                  |                                 |");
	MENU_PRINT("| 5 - Set local SSDP USN          |                                  |                                 |");
	MENU_PRINT("| 6 - Set local SSDP Location     |                                  |                                 |");
	MENU_PRINT("| 7 - Get SSDP Target             |                                  |                                 |");
	MENU_PRINT("| 8 - Get SSDP USN                |                                  |                                 |");
	MENU_PRINT("| 9 - Get SSDP Location           |                                  |                                 |");
	MENU_PRINT("| a - Start local SSDP Service    |                                  |                                 |");
	MENU_PRINT("| b - Stop local SSDP Service     |                                  |                                 |");
	MENU_PRINT("| c - Browse SSDP Service         |                                  |                                 |");
	MENU_PRINT("| d - Stop Browse SSDP            |                                  |                                 |");
	MENU_PRINT("| e - Set SSDP Service Found cb   |                                  |                                 |");
	MENU_PRINT("| f - Unset SSDP Service Found cb |                                  |                                 |");
	MENU_PRINT("|_________________________________|__________________________________|_________________________________|");
}


int test_thread()
{
	int rv;
	char a[10];

	printf("Event received from stdin\n");

	rv = read(0, a, 10);

	if (rv <= 0 || a[0] == '0')
		exit(1);

	if (a[0] == '\n' || a[0] == '\r') {
		printf("\n\n SSDP CAPI Test App\n\n");
		printf("Options..\n");
		MenuScreen();
		printf("Press ENTER to show options menu.......\n");
	}

	switch (a[0]) {
	case '1':
		rv = test_ssdp_initialize();
		break;
	case '2':
		rv = test_ssdp_deinitialize();
		break;
	case '3':
		rv = test_ssdp_create_service();
		break;
	case '4':
		rv = test_ssdp_destroy_service();
		break;
	case '5':
		rv = test_ssdp_set_usn();
		break;
	case '6':
		rv = test_ssdp_set_url();
		break;
	case '7':
		rv = test_ssdp_get_target();
		break;
	case '8':
		rv = test_ssdp_get_usn();
		break;
	case '9':
		rv = test_ssdp_get_url();
		break;
	case 'a':
		rv = test_ssdp_register_service();
		break;
	case 'b':
		rv = test_ssdp_deregister_service();
		break;
	case 'c':
		rv = test_ssdp_browse_service();
		break;
	case 'd':
		rv = test_ssdp_stop_browse_service();
		break;
	default:
		break;
	}

	if (rv == 1)
		printf("\nOperation Succeeded!\n");
	else
		printf("\nOperation Failed!\n");

	return TRUE;
}

/* test thread */

int main(int argc, char *argv[])
{
	GMainLoop *main_loop = NULL;
	int ret = -1;

	/* Initialize required subsystems */
#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif

	/* test thread */

	GIOChannel *channel = g_io_channel_unix_new(0);
	g_io_add_watch(channel, (G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL), test_thread, NULL);

	printf("Test Thread Created...\n");

	/* Crate the GLIB main loop */
	main_loop = g_main_loop_new(NULL, FALSE);

	/* Run the main loop */
	g_main_loop_run (main_loop);

	g_main_loop_unref(main_loop);

	return ret;
}
