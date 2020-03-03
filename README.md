# message-thing-device

![message-thing-device](https://raw.githubusercontent.com/brandmooffin/message-thing-device/master/fritzing/message-thing-device_bb.png)

## Project Overview
* ESP8266
* LCD 1602 w/ I2C attachment
* 1 LED
* 3 resistors (about 100ohms)
* 3 buttons (left most will serve as send/confirm message, middle and right buttons will serve to scroll through messages to select)

This project is a sub project of the Message Thing project which works with [message-thing-service](https://github.com/brandmooffin/message-thing-service) and [message-thing-android](https://github.com/brandmooffin/message-thing-android).

## Getting Started 
- Download or clone the repo (or fork it!)

## Setting up
- Download and install Arduino IDE https://www.arduino.cc/en/main/software
- Add support for ESP8266
    - Open up Preferences and look for Additional Boards Manager URLs
    - Add the following URL http://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Next go to Tools and open up Boards Manager from the Board menu item
    - Search for ESP8266 and install esp8266 by ESP8266 Community
- Install additional libraries
    - Go to Tools and open up Library Manager from Manage libraries 
    - Search for ArduinoJson and install ArduinoJson by Benoit Blanchon
    - After that installs completely, search for LiquidCrystal I2C
    - Look for LiquidCrystal I2C by Frank de Brabander and install the library
- Finally open up the sketch located `message-thing-sketch/message-thing-sketch.ino`
- Be sure to update `<api-base-url>` with the correct url based on where you have `message-thing-service` deployed

## Deploy sketch
- After you have the component connected, connect your ESP8266 via USB
- Open up Arduino IDE with the sketch
- Verify the correct board is selected
    - Go to Tools and then Board and make sure you select Generic ESP8266 Module or something similar
- Verify the correct COM port is selected
    - Go to Tools and then COM and make sure you have the correct COM port selected
- Compile the sketch by clicking the checkmark button on the top left
- If the compilation is successful then flash the code! Click on the arrow button next to the compile button

## Troubleshooting
- COM port is failing to connect to device and flash but I am certain it is the correct port!
    - Verify that the connection between the computer and board are good. If you are using a USB-C adapter make sure that it has enough power going to your board.

## Contributing
Interested in helping make changes, adding features, or just want to help maintain this project in any way? 

If you would like to help maintain this project please feel free to reach out on our slack https://join.slack.com/t/sefloridatech/shared_invite/enQtNDcxMDc4MDM1OTg4LTAyOWE1ZDliZWQ0MTkyYTc3NzM4ODEzYzYxODNiNGQwNzQxNWE0MDk5NDRlZDE2NDAwOThjZjExYmJjY2M0YWU

This project is open source and is open to anyone that's interested in looking to learn or would like to improve this project. 