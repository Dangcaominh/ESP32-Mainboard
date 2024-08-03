#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1X15.h>
class Boost
{
    private:
        Adafruit_MCP4725 dacVolt;
        Adafruit_MCP4725 dacCurr;
        Adafruit_ADS1115 adc;
        uint8_t controlPin;
        TwoWire wire = Wire;
    public:
        Boost(uint8_t controlPin, TwoWire& wire);
        ~Boost();
        bool initialize();
        inline void start();
        inline void stop();
        void setVoltage(float voltage);
        void setCurrent(float current);
        float getVoltage();
        float getCurrent();
};

Boost::Boost(uint8_t controlPin, TwoWire& wire)
{
    this->controlPin = controlPin;
    this->wire = wire;
}

bool Boost::initialize()
{
    if(adc.begin(0x48, &wire))
    {
        return false;
    }
    if(!dacVolt.begin(0x60, &wire))
    {
        return false;
    }
    if(!dacCurr.begin(0x61, &wire))
    {
        return false;
    }
    pinMode(controlPin, OUTPUT);
    return true;
}

void Boost::start()
{
    digitalWrite(controlPin, HIGH);
}

void Boost::stop()
{
    digitalWrite(controlPin, LOW);
}

void Boost::setVoltage(float voltage)
{
    dacVolt.setVoltage(voltage, false);
}

void Boost::setCurrent(float current)
{
    dacCurr.setVoltage(current, false);
}   

float Boost::getVoltage()
{
    return adc.readADC_Differential_0_1();
}   

float Boost::getCurrent()
{
    return adc.readADC_Differential_2_3();
}