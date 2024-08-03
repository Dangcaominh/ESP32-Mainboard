#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1X15.h>

class Buck
{
    private:
        Adafruit_MCP4725 dacVolt;
        Adafruit_MCP4725 dacCurr;
        Adafruit_ADS1115 adc;
        uint8_t controlPin;
        TwoWire wire = Wire;
    public:
        Buck(uint8_t controlPin, TwoWire& wire);
        ~Buck();
        bool initialize();
        void start();
        void stop();
        void setVoltage(float voltage);
        void setCurrent(float current);
        float getVoltage();
        float getCurrent(); 
};

Buck::Buck(uint8_t controlPin, TwoWire& wire)   
{
    this->controlPin = controlPin;
    this->wire = wire;
}

Buck::~Buck()
{

}

bool Buck::initialize()
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
void Buck::start()
{
    digitalWrite(controlPin, HIGH);
}

void Buck::stop()
{
    digitalWrite(controlPin, LOW);
}

void Buck::setVoltage(float voltage)
{
    dacVolt.setVoltage(voltage, false);
}

void Buck::setCurrent(float current)
{
    dacCurr.setVoltage(current, false);
}

float Buck::getVoltage()
{
    return adc.readADC_Differential_0_1();
}

float Buck::getCurrent()
{
    return adc.readADC_Differential_2_3();
}