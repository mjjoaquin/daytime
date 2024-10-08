#include <I2C_RTC.h>
#include <Adafruit_Sensor.h>

#define WARM_WHITE_PIN 9
#define COOL_WHITE_PIN 10
#define PIR_SENSOR_PIN 2

static DS1307 RTC;

int lastMotionDetected = 0;
bool currentState = 0;
#define TIMEOUT (5 * 60 * 1000) // 5 minutes in milliseconds

void setup() {
    pinMode(WARM_WHITE_PIN, OUTPUT);
    pinMode(COOL_WHITE_PIN, OUTPUT);
    pinMode(PIR_SENSOR_PIN, INPUT);

    Serial.begin(9600);

    if (RTC.isRunning())
        Serial.println("RTC runnining: " + String(RTC.getHours()) + ":" + String(RTC.getMinutes()) + ":" + String(RTC.getSeconds()));
    else
        Serial.println("RTC not running");

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
        Serial.println("No motion detected");
    }

    if ((millis() - lastMotionDetected) > TIMEOUT)
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
}

void fadeLEDs(int newWarmWhiteValue, int oldWarmWhiteValue, int newCoolWhiteValue, int oldCoolWhiteValue)
{
    // Fade the LEDs from the old values to the new values
    for (int i = 0; i <= 255; i++)
    {
        analogWrite(WARM_WHITE_PIN, oldWarmWhiteValue + (newWarmWhiteValue - oldWarmWhiteValue) * i / 255);
        analogWrite(COOL_WHITE_PIN, oldCoolWhiteValue + (newCoolWhiteValue - oldCoolWhiteValue) * i / 255);
        delay(10);
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
        coolWhiteValue = 0.1967 * pow((decimalTime - 3), 4);
    } else {
        coolWhiteValue = 255;
    } 
}

void setTime() {
    // set time to compile time
    RTC.begin();
    int hours = atoi(__TIME__);
    int minutes = atoi(__TIME__ + 3);
    int seconds = atoi(__TIME__ + 6);

    // Set the RTC time
    RTC.setTime(hours, minutes, seconds);
}