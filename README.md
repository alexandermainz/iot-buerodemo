# Buerodemo IoT Demo Application
A simple IoT (Internet of Things) demo application running on an ESP32 microcontroller with small 1.3'' I2C OLED display and DS18B20 temperature sensor.
The display shows alterning information like welcome text, office temperature, a door-label-like text of where am I currently, a scroll message and a user message which anyone can enter at a rudimentary PHP minisite.

# Contents of this repository #
1. Arduino project for the Buerodemo ESP32 application.
2. PHP REST service for the communication with the Buerodemo app.

# Arduino project (/buerodemo)
- ```buerodemo.ino```: The Arduino application.
- ```RestCall.*```: An Arduino lib for easy calling REST services (currently HTTP only tested).

# PHP REST Service
Contains the REST endpoints for the Arduino app in ```api/ìndex.php``` as well as a simple front-end page in ```index.html``` for user interaction.

The PHP part uses the Respect/Rest library from https://github.com/Respect/Rest.
