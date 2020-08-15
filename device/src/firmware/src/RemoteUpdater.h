#ifndef RemoteUpdater_h
#define RemoteUpdater_h

#include <Log4Esp.h>

class RemoteUpdater {
private:
    log4Esp::Logger logger = log4Esp::Logger("RemoteUpdater");

public:
    void setup(String deviceId);
    void handle();
};
#endif