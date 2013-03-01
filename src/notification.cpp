/**
 *  notification.cpp
 *
 *  @author Nathan Campos
 */

#include <string>
#include <cstdio>
#include <cstdlib>
#include <growl.h>

#include "notification.h"
using namespace std;

int Notification::notify(string title, string message) {
	growl_init();
	int rc = growl("127.0.0.1", "leafirc", "leafirc", title.c_str(), message.c_str(), NULL, NULL, NULL);
	growl_shutdown();

	return rc;
}
