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


void test_registered_cb(ssdp_error_e result, ssdp_service_h ssdp_service, void *user_data)
{
	__FUNC_ENTER__;
	printf("service handler: %u\n", ssdp_service);
	printf("result: %d\n", result);
	__FUNC_EXIT__;
}

int main(int argc, char *argv[])
{
	GMainLoop *main_loop = NULL;
	int ret = -1;
	ssdp_service_h serv_id;
	char *target = "upnp:rootdevice";
	char *usn = "uuid:1234abce-12ab";
	char *url = "localhost";


	/* Initialize required subsystems */
#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif

	if (ssdp_initialize() == 0) {
		printf(MAKE_GREEN"Initialized"RESET_COLOR"\n");
	}
	if (ssdp_create_local_service(target, &serv_id) == 0) {
		printf(MAKE_GREEN"Create service. Type: %s, handle: %u "RESET_COLOR"\n", target, serv_id);
	}
	if (ssdp_service_set_usn(serv_id, usn) == 0) {
		printf(MAKE_GREEN"Set USN %s"RESET_COLOR"\n", usn);
	}
	if (ssdp_service_set_url(serv_id, url) == 0) {
		printf(MAKE_GREEN"Set URL %s"RESET_COLOR"\n", url);
	}
	if (ssdp_register_local_service(serv_id, &test_registered_cb, NULL) == 0) {
		printf(MAKE_GREEN"Start registering"RESET_COLOR"\n");
	}

	/* Crate the GLIB main loop */
	main_loop = g_main_loop_new(NULL, FALSE);

	/* Run the main loop */
	g_main_loop_run(main_loop);

	g_main_loop_unref(main_loop);

	return ret;
}
