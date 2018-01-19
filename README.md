Cloud4RPi Client Library for [Arduino](https://www.arduino.cc/) Framework
=========================================================================


This package provides a client library that simplifies the connection of Internet-enabled Arduino devices to the Cloud4RPi service.

Supported hardware:

* [ESP8266](https://en.wikipedia.org/wiki/ESP8266) boards (like [NodeMCU Dev Kit](http://www.nodemcu.com/index_en.html#fr_54747661d775ef1a3600009e) and others).

Supported IDEs:

* [PlatformIO](http://platformio.org/platformio-ide)
* [Arduino IDE](https://www.arduino.cc/en/main/software)

The further instructions depends on your preferred IDE.

# PlatformIO

1. Create the new PlatformIO project for your board as described in the PlatformIO documentation ([Visual Studio Code](http://docs.platformio.org/en/latest/ide/vscode.html#id3), [Atom](http://docs.platformio.org/en/latest/ide/atom.html#setting-up-the-project))
2. Open your `platformio.ini` and add the following lines to the end:
    ```
    lib_deps =
        cloud4rpi-esp-arduino

    build_flags=
        -D MQTT_MAX_PACKET_SIZE=1024
        -D MQTT_MAX_TRANSFER_SIZE=128
        -D CLOUD4RPI_DEBUG=1
    ```
4. Save the file and open the `src\main.cpp` file.
5. Copy-paste the [sample code](https://github.com/cloud4rpi/cloud4rpi-esp-arduino/blob/master/examples/ESP8266/control.ino) into it.
6. Hit the `Build` button.

[Cloud4RPi library in PlatformIO Registry](http://platformio.org/lib/show/2045/cloud4rpi-esp-arduino)

# Arduino IDE

[to be supplied]