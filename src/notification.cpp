/**
 *  notification.cpp
 *
 *  @author Nathan Campos
 */

#include <string>
#include <cstdio>
#include <cstdlib>

#ifdef __APPLE__
	#include <growl.h>
#else
	#include <growl.h>
#endif

#include "notification.h"
using namespace std;

int Notification::notify(string title, string message) {
	#ifdef __APPLE__
		growl_init();
		int rc = growl("127.0.0.1", "leafirc", "leafirc", title.c_str(), message.c_str(), NULL, NULL, NULL);
		growl_shutdown();
	#else
		// libnotify.
	#endif

	return rc;
}
