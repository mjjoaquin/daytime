
# House LEDs Control with RTC and PIR Sensor
This project controls house LEDs using a Real-Time Clock (RTC) and a Passive Infrared (PIR) sensor. The LEDs are controlled based on motion detection and time.

## Components
1. Arduino board (Nano reccomended)
2. DS1307 RTC module
3. PIR sensor
4. Tunable w/ww LED strip
5. Extra wiring

## Programming
- Install the I2C_RTC and Adafruit_Sensor libraries
- Upload sketch to the arduino
- Set time using `rtc HH:MM` (24 hour format) at serial baud rate 9600

## Wiring
- Connect RTC SDA to A4 on the Arduino.
- Connect RTC SCL to A5 on the Arduino.
- Connect PIR OUT to digital pin 2 on the Arduino.
- Connect Warm White anode to digital pin 9
- Connect Cool White to digital pin 10
- Connect the cathode to GND.
