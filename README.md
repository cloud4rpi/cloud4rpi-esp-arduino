Cloud4RPi Client Library for [Arduino](https://www.arduino.cc/) Framework
=========================================================================

The package provides a client library that simplifies connecting Internet-enabled Arduino devices to the Cloud4RPi service.

Currently supported hardware:

* [ESP8266](https://en.wikipedia.org/wiki/ESP8266) boards (like [NodeMCU Dev Kit](http://www.nodemcu.com/index_en.html#fr_54747661d775ef1a3600009e) and others).

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
4. Save the `platformio.ini` file and open the `src\main.cpp` file.
5. Copy-paste the [sample code](examples/ESP8266/ESP8266.ino) into it. <!-- common part begins here -->
4. [Log in to your Cloud4RPi account](https://cloud4rpi.io/signin) or [create a new one](https://cloud4rpi.io/register).
5. Copy [your device](https://cloud4rpi.io/devices)'s **Device Token**. You can create a device on the [Devices](https://cloud4rpi.io/devices) page and copy its **Device Token** if you do not have one.
6. Replace the `__YOUR_DEVICE_TOKEN__` string with your device token.
7. Replace the `__SSID__` and `__PASSWORD__` strings with your Wi-Fi network data.
7. If you know the pin number connected to an LED on your board, replace the `BUILTIN_LED` constant with it.
6. Hit the `Build` button and wait until the compilation is done.
8. Connect the board in the flashing mode and hit the `Upload` button.
9. Once flashing is complete, open `Serial Monitor` and watch the sample code's output.
8. Notice that the [device](https://cloud4rpi.io/devices) went online and started sending data.
9. Go to the [Control Panels](https://cloud4rpi.io/control-panels/) page and add a new control panel.
10. Add a new **Chart** widget and bind it to the `Uptime` variable.
10. Add a new **Switch** widget and bind it to the `LED On` variable.
11. Add a new **Text** widget and bind it to the `STATUS` variable. Configure different colors for the **"IDLE"**, **"RING"** and **"BOOM!"** strings.


## Arduino

1. Create a new Arduino sketch and select your board in the **Tools** | **Board** menu. [Add ESP8266 support](https://github.com/esp8266/Arduino) if required.
2. Install the Cloud4RPi library from the **Library Manager**: open the **Sketch** | **Include Library** | **Manage Libraries** menu, enter `cloud4rpi` into the search field and install the **cloud4rpi-esp-arduino** package.
3. Install **ArduinoJson** and **PubSubClient** libraries. You should do it manually because the **Library Manager** does not manage package dependencies.
4. Configure the installed libraries:
    1. Open the `%HOMEPATH%\Documents\Arduino\libraries\PubSubClient\src\PubSubClient.h` (`~/Documents/Arduino/libraries/PubSubClient/src/PubSubClient.h` on Mac) file with any text editor (for instance, [VS Code](https://code.visualstudio.com)).
    2. Add the following define statements at the beginning:
        ```c
        #define MQTT_MAX_PACKET_SIZE 1024
        #define MQTT_MAX_TRANSFER_SIZE 128
        ```
    3. Open the `%HOMEPATH%\Documents\Arduino\libraries\cloud4rpi-esp-arduino\src\Cloud4RPi.h` (`~/Documents/Arduino/libraries/cloud4rpi-esp-arduino/src/Cloud4RPi.h` on Mac) file and enable verbose output by adding the `#define CLOUD4RPI_DEBUG 1` line at the beginning.
5. Open the sample code using the **File** | **Examples** | **cloud4rpi-esp-arduino** | **ESP8266** menu item. Restart Arduino IDE if this item did not appear. <!-- common part begins here -->
4. [Log in to your Cloud4RPi account](https://cloud4rpi.io/signin) or [create a new one](https://cloud4rpi.io/register).
5. Copy [your device](https://cloud4rpi.io/devices)'s **Device Token**. You can create a device on the [Devices](https://cloud4rpi.io/devices) page and copy its **Device Token** if you do not have one.
6. Replace the `__YOUR_DEVICE_TOKEN__` string with your device token.
7. Replace the `__SSID__` and `__PASSWORD__` strings with your Wi-Fi network data.
7. If you know the pin number connected to an LED on your board, replace the `BUILTIN_LED` constant with it.
6. Hit the `Verify` button and wait until the compilation is done.
8. Connect the board in the flashing mode and hit the `Upload` button.
9. Once flashing is complete, open `Serial Monitor` and watch the sample code's output.
8. Notice that the [device](https://cloud4rpi.io/devices) went online and started sending data.
9. Go to the [Control Panels](https://cloud4rpi.io/control-panels/) page and add a new control panel.
10. Add a new **Chart** widget and bind it to the `Uptime` variable.
10. Add a new **Switch** widget and bind it to the `LED On` variable.
11. Add a new **Text** widget and bind it to the `STATUS` variable. Configure different colors for the **"IDLE"**, **"RING"** and **"BOOM!"** strings.
