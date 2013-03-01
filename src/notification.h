/**
 *  notification.h
 *
 *  @author Nathan Campos
 */

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <string>
#include <vector>

class Notification {
    public:
        static int notify(std::string title, std::string message);
};

#endif
