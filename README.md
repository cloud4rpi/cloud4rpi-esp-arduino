Cloud4RPi Client Library for [Arduino](https://www.arduino.cc/) Framework
=========================================================================
[![Build Status](https://travis-ci.org/cloud4rpi/cloud4rpi-esp-arduino.svg?branch=master)](https://travis-ci.org/cloud4rpi/loud4rpi-esp-arduino)

The package provides a client library that simplifies connecting Internet-enabled Arduino devices to the Cloud4RPi service.

Currently supported hardware:

* [ESP8266](https://en.wikipedia.org/wiki/ESP8266) boards (like [NodeMCU Dev Kit](http://www.nodemcu.com/index_en.html#fr_54747661d775ef1a3600009e) and others).
* [ESP32](https://en.wikipedia.org/wiki/ESP32) boards (like [ESP32-DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview) and others).

Supported ecosystems:

* [PlatformIO](http://platformio.org/platformio-ide)
* [Arduino IDE](https://www.arduino.cc/en/main/software)

The further instructions depend on your preferred ecosystem.

## PlatformIO

The [Cloud4RPi library](http://platformio.org/lib/show/2045/cloud4rpi-esp-arduino) is published in the [PlatformIO Registry](http://platformio.org/lib). Follow the instructions below to start a new Cloud4RPi-enabled project:

1. Create a new PlatformIO project for your board as described in the PlatformIO documentation ([Visual Studio Code](http://docs.platformio.org/en/latest/ide/vscode.html#id3), [Atom](http://docs.platformio.org/en/latest/ide/atom.html#setting-up-the-project))
2. Open your `platformio.ini` and add the following lines to the end:
    ```
    lib_deps =
        cloud4rpi-esp-arduino

    build_flags=
    ; Required for PubSub library
        -D MQTT_MAX_PACKET_SIZE=1024
        -D MQTT_MAX_TRANSFER_SIZE=128
    ; Enables the verbose output to Serial
        -D CLOUD4RPI_DEBUG=1
    ```
3. Save the `platformio.ini` file and open the `src\main.cpp` file.
4. Copy-paste the [sample code](examples/control/control.ino) into it. <!-- common part begins here -->
5. [Log in to your Cloud4RPi account](https://cloud4rpi.io/signin) or [create a new one](https://cloud4rpi.io/register).
6. Copy [your device](https://cloud4rpi.io/devices)'s **Device Token**. You can create a device on the [Devices](https://cloud4rpi.io/devices) page and copy its **Device Token** if you do not have one.
7. Replace the `__YOUR_DEVICE_TOKEN__` string with your device token.
8. Replace the `__SSID__` and `__PASSWORD__` strings with your Wi-Fi network data.
9. If the `LED_BUILTIN` constant is not defined in your board's library, change this constant to the pin number to which an LED is connected (for example, `2`).
10. Hit the `Build` button and wait until the compilation is done.
11. Connect the board in the flashing mode and hit the `Upload` button.
12. Once flashing is complete, open `Serial Monitor` and watch the sample code's output.
13. Notice that the [device](https://cloud4rpi.io/devices) went online and started sending data.
14. Go to the [Control Panels](https://cloud4rpi.io/control-panels/) page and add a new control panel.
15. Add a new **Number** widget and bind it to the `Uptime` variable.
16. Add a new **Switch** widget and bind it to the `LED On` variable.
17. Add a new **Text** widget and bind it to the `State` variable. Configure different colors for the **"IDLE"**, **"RING"** and **"BOOM!"** strings.
18. Add a new **Slider** widget and bind it to the `DesiredTemp` variable, set its minimum value to 10 and maximum value to 30.
19. Add a new **Gauge** widget and bind it to the `DesiredTemp` variable, set its minimum value to 10 and maximum value to 30.



## Arduino

1. Create a new Arduino sketch and select your board in the **Tools** | **Board** menu. Add [ESP8266](https://github.com/esp8266/Arduino#installing-with-boards-manager) or [ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md) if required.
2. Install the Cloud4RPi library from the **Library Manager**: open the **Sketch** | **Include Library** | **Manage Libraries** menu, enter `cloud4rpi` into the search field and install the **cloud4rpi-esp-arduino** package.
3. Install **ArduinoJson** and **PubSubClient** libraries. You should do it manually because the **Library Manager** does not manage package dependencies.
4. Configure the installed libraries:
    1. Open the `%HOMEPATH%\Documents\Arduino\libraries\PubSubClient\src\PubSubClient.h` (`~/Documents/Arduino/libraries/PubSubClient/src/PubSubClient.h` on Mac) file with any text editor (for instance, [VS Code](https://code.visualstudio.com)).
    2. Add the following define directives at the beginning:
        ```c
        #define MQTT_MAX_PACKET_SIZE 1024
        #define MQTT_MAX_TRANSFER_SIZE 128
        ```
    3. [Optional] Open the `%HOMEPATH%\Documents\Arduino\libraries\cloud4rpi-esp-arduino\src\Cloud4RPi.h` (`~/Documents/Arduino/libraries/cloud4rpi-esp-arduino/src/Cloud4RPi.h` on Mac) file and enable verbose output by adding the `#define CLOUD4RPI_DEBUG 1` line at the beginning.
5. Open the sample code using the **File** | **Examples** | **cloud4rpi-esp-arduino** menu item. Restart Arduino IDE if this item did not appear. <!-- common part begins here -->
6. [Log in to your Cloud4RPi account](https://cloud4rpi.io/signin) or [create a new one](https://cloud4rpi.io/register).
7. Copy [your device](https://cloud4rpi.io/devices)'s **Device Token**. You can create a device on the [Devices](https://cloud4rpi.io/devices) page and copy its **Device Token** if you do not have one.
8. Replace the `__YOUR_DEVICE_TOKEN__` string with your device token.
9. Replace the `__SSID__` and `__PASSWORD__` strings with your Wi-Fi network data.
10. If the `LED_BUILTIN` constant is not defined in your board's library, change this constant to the pin number to which an LED is connected (for example, `2`).
11. Hit the `Verify` button and wait until the compilation is done.
12. Connect the board in the flashing mode and hit the `Upload` button.
13. Once flashing is complete, open `Serial Monitor` and watch the sample code's output.
14. Notice that the [device](https://cloud4rpi.io/devices) went online and started sending data.
15. Go to the [Control Panels](https://cloud4rpi.io/control-panels/) page and add a new control panel.
16. Add a new **Number** widget and bind it to the `Uptime` variable.
17. Add a new **Switch** widget and bind it to the `LED On` variable.
18. Add a new **Text** widget and bind it to the `State` variable. Configure different colors for the **"IDLE"**, **"RING"** and **"BOOM!"** strings.
19. Add a new **Slider** widget and bind it to the `DesiredTemp` variable, set its minimum value to 10 and maximum value to 30.
20. Add a new **Gauge** widget and bind it to the `DesiredTemp` variable, set its minimum value to 10 and maximum value to 30.


## See Also

* [Library and Examples for ESP8266 with MicroPython](https://github.com/cloud4rpi/cloud4rpi-esp8266-micropython)
* [Examples for Raspberry Pi](https://github.com/cloud4rpi/cloud4rpi-raspberrypi-python)
* [Examples for Next Thing Co. C.H.I.P.](https://github.com/cloud4rpi/cloud4rpi-chip-python)
* [Examples for Onion Omega2](https://github.com/cloud4rpi/cloud4rpi-omega2-python)
* [Documentation Repository](https://github.com/cloud4rpi/docs)
