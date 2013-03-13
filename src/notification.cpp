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
	#include <glib.h>
	#include <unistd.h>
	#include <libnotify/notify.h>
#endif

#include "notification.h"
using namespace std;

int Notification::notify(string title, string message) {
	#ifdef __APPLE__
		// Growl for Mac.
		growl_init();
		int rc = growl("127.0.0.1", "leafirc", "leafirc", title.c_str(), message.c_str(), NULL, NULL, NULL);
		growl_shutdown();

		return rc;
	#else
		// libnotify for UNIX.
		NotifyNotification *notification;
		gboolean success;
		GError *error;

		if (!notify_init("leafIRC")) {
			exit(EXIT_FAILURE);
		}

		error = NULL;
		notification = notify_notification_new(title.c_str(), message.c_str(), NULL);
		success = notify_notification_show(notification, &error);
		notify_uninit();

		return (int)success;
	#endif
}
