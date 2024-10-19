#include <I2C_RTC.h>
#include <Adafruit_Sensor.h>

#define WARM_WHITE_PIN 9
#define COOL_WHITE_PIN 10
#define PIR_SENSOR_PIN 2

static DS1307 RTC;

unsigned long lastMotionDetected = 0;
bool currentState = 0;
unsigned long TIMEOUT = (60UL * 1000); // minutes in milliseconds

void setup() {
    pinMode(WARM_WHITE_PIN, OUTPUT);
    pinMode(COOL_WHITE_PIN, OUTPUT);
    pinMode(PIR_SENSOR_PIN, INPUT);

    Serial.begin(9600);

    if (RTC.begin())
        Serial.println("RTC runnining: " + String(RTC.getHours()) + ":" + String(RTC.getMinutes()) + ":" + String(RTC.getSeconds()));
    else
        Serial.println("RTC failed to start");

    // Test LEDs
    analogWrite(WARM_WHITE_PIN, 127);
    analogWrite(COOL_WHITE_PIN, 127);
    delay(500);
    analogWrite(WARM_WHITE_PIN, 0);
    delay(500);
    analogWrite(WARM_WHITE_PIN, 127);
    analogWrite(COOL_WHITE_PIN, 0);
    delay(500);
    analogWrite(WARM_WHITE_PIN, 0);
    analogWrite(COOL_WHITE_PIN, 127);
    delay(500);
    analogWrite(WARM_WHITE_PIN, 127);
    analogWrite(COOL_WHITE_PIN, 0);
    delay(500);
    analogWrite(WARM_WHITE_PIN, 0);
    analogWrite(COOL_WHITE_PIN, 0);
}

void loop()
{
    int pirState = digitalRead(PIR_SENSOR_PIN);
    int lastState = currentState;

    if (pirState == HIGH)
    { // If motion has been detected
        lastMotionDetected = millis();
        currentState = 1;
        Serial.println("Motion detected");
    } else {
        Serial.println("No motion detected for " + String((millis() - lastMotionDetected) / 1000));
    }

    if (((millis() - lastMotionDetected) > TIMEOUT) && currentState == 1)
    { // If no motion has been detected for TIMEOUT milliseconds
        Serial.println("Timeout");
        currentState = 0;
    }

    if (currentState != lastState)
    { // If the state has changed
        int warmWhiteValue, coolWhiteValue;
        getLEDValues(warmWhiteValue, coolWhiteValue);

        if (currentState == 1)
        { // to on...
            Serial.println("Turning lights on: " + String(warmWhiteValue) + ", " + String(coolWhiteValue));
            fadeLEDs(warmWhiteValue, 0, coolWhiteValue, 0);
        }
        else if (currentState == 0)
        { // to off...
            Serial.println("Turning lights off");
            fadeLEDs(0, warmWhiteValue, 0, coolWhiteValue);
        }
    }
    else if (currentState == 0)
    {
        setLEDs(0, 0);
    }
    delay(1000);

    if (Serial.available()) {
        String message = Serial.readStringUntil('\n'); // Read the incoming data until newline

        // Check if the message starts with "rtc "
        if (message.startsWith("rtc ")) {
            message = message.substring(4); // Remove "rtc " from the message

            // Parse the hours, minutes and seconds from the message
            int hour = message.substring(0, 2).toInt();
            int minute = message.substring(3, 5).toInt();

            // Set the time
            RTC.setTime(hour, minute, 30);
            Serial.println("Time set to: " + String(hour) + ":" + String(minute));
        }
    }
}

void fadeLEDs(int newWarmWhiteValue, int oldWarmWhiteValue, int newCoolWhiteValue, int oldCoolWhiteValue)
{
    // Fade the LEDs from the old values to the new values
    for (int i = 0; i <= 255; i++)
    {
        analogWrite(WARM_WHITE_PIN, oldWarmWhiteValue + (newWarmWhiteValue - oldWarmWhiteValue) * i / 255);
        analogWrite(COOL_WHITE_PIN, oldCoolWhiteValue + (newCoolWhiteValue - oldCoolWhiteValue) * i / 255);
        delay(20);
    }
}

void setLEDs(int warmWhiteValue, int coolWhiteValue)
{
    // Set LED values without fading
    analogWrite(WARM_WHITE_PIN, warmWhiteValue);
    analogWrite(COOL_WHITE_PIN, coolWhiteValue);
}

void getLEDValues(int &warmWhiteValue, int &coolWhiteValue) {
    // Set the LED values based on the time of day
    int hour = RTC.getHours();
    int minute = RTC.getMinutes();

    float decimalTime = hour + minute / 60.0;

    // Darker at night, peak at 13:00
    warmWhiteValue = 96 * sin((0.5 * PI * decimalTime / 24) - (7 * 2 * PI / 24)) + 159;
    if (decimalTime < 9.0 || decimalTime > 21.0) { // Greatly reduce blue light during the night
        if (decimalTime > 8.0 && decimalTime < 9.0) {
            coolWhiteValue = 255 * decimalTime - 255 * 8.0;
        } else if (decimalTime < 22 && decimalTime > 21.0) {
            coolWhiteValue = -255 * decimalTime + 255 * 22.0;
        } else {
            coolWhiteValue = 0;
        }
    } else {
        coolWhiteValue = 255;
    } 
}