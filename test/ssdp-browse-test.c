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

void test_found_cb(ssdp_service_h ssdp_service, ssdp_service_state_e state, void *user_data)
{
	__FUNC_ENTER__;
	char *usn;
	char *url;
	printf("service handler: %u\n", ssdp_service);
	ssdp_service_get_usn(ssdp_service, &usn);
	ssdp_service_get_url(ssdp_service, &url);
	printf("state: %s\n", state==SSDP_SERVICE_STATE_AVAILABLE?"AVAILABLE":"UNAVAILABE");
	printf("usn: %s\n", usn);
	printf("url: %s\n", url);
	__FUNC_EXIT__;
}

int main(int argc, char *argv[])
{
	GMainLoop *main_loop = NULL;
	int ret = -1;
	ssdp_browser_h browser_id;
	char *target = "upnp:rootdevice";

	/* Initialize required subsystems */
#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif

	if (ssdp_initialize() == 0) {
		printf(MAKE_GREEN"Initialized"RESET_COLOR"\n");
	}
	if (ssdp_start_browsing_service(target, &browser_id, &test_found_cb, NULL) == 0) {
		printf(MAKE_GREEN"Start browsing"RESET_COLOR"\n");
	}

	/* Crate the GLIB main loop */
	main_loop = g_main_loop_new(NULL, FALSE);

	/* Run the main loop */
	g_main_loop_run (main_loop);

	g_main_loop_unref(main_loop);

	return ret;
}
