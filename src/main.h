#include <Arduino.h>

#define CLOUD4RPI_SERVER "mq.cloud4rpi.io"

class Cloud4RPi {
public:
    Cloud4RPi(const char *deviceToken, const char* server = CLOUD4RPI_SERVER);
    void printLogo();
};
