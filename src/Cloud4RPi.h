#include <Arduino.h>

namespace {
    const char* CLOUD4RPI_SERVER = "mq.cloud4rpi.io";
}

class Cloud4RPi {
public:
    Cloud4RPi(String deviceToken, String server = CLOUD4RPI_SERVER);
    void printLogo();
private:
    String server_;
    String deviceToken_;
};
