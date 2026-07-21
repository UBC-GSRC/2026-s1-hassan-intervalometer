#include <Arduino.h>
#include <Wire.h>

#define TCA9554_ADDR 0x20

// All relays OFF = 0xFF
// Relays are active LOW
uint8_t relayState = 0xFF;

unsigned long lastTrigger = 0;
const unsigned long INTERVAL_MS = 60000; // 1 minute

void writeOutputs()
{
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(0x01); // Output register
    Wire.write(relayState);
    Wire.endTransmission();
}

void setRelay(uint8_t relay, bool on)
{
    if (relay < 1 || relay > 8)
        return;

    uint8_t bit = relay - 1;

    if (on)
        relayState &= ~(1 << bit); // active low
    else
        relayState |= (1 << bit);

    writeOutputs();
}

void pulseRelay(uint8_t relay, uint32_t durationMs)
{
    setRelay(relay, true);

    Serial.printf("Relay %d ON\n", relay);

    delay(durationMs);

    setRelay(relay, false);

    Serial.printf("Relay %d OFF\n", relay);
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Wire.begin(42, 41);

    // Configure all TCA9554 pins as outputs
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(0x03); // Configuration register
    Wire.write(0x00); // All outputs
    Wire.endTransmission();

    // Ensure all relays are off
    writeOutputs();

    Serial.println("Relay timer started");
}

void loop()
{
    unsigned long now = millis();

    if (now - lastTrigger >= INTERVAL_MS)
    {
        lastTrigger = now;

        // Turn Relay 1 on for 1 second
        pulseRelay(1, 1000);
    }
}