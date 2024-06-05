#include <Arduino.h>
#include "esp_adc_cal.h"

const double GAIN = 7.889112903225806451612903225806451612903225806451;
double ModifiedAnalogReadVolts(uint8_t pin, uint16_t tolerance = 10);
inline double linear_regression(double x);
#define ADC_PIN 36
int DACData = 50;
void setup()
{
	Serial.begin(9600);
}

void loop()
{
	if (Serial.available() > 0) {
    	String str = Serial.readString();
    	str.trim();
		DACData = str.toInt();
	}
	dacWrite(25, DACData);
	Serial.println(ModifiedAnalogReadVolts(ADC_PIN, 10));
}

inline double linear_regression(double x)
{
	if((100 <= x) && (x <= 115))
	{
		return (0.000821319907160756 * x + 0.15511300019077814);
	}
	else
	{
		return (0.0008199413026912705 * x + 0.16634694534577377);
	}
}

double ModifiedAnalogReadVolts(uint8_t pin, uint16_t tolerance)
{
	uint32_t data[100] = {0};
	for (int i = 0; i < 100; i++)
	{
		data[i] = analogRead(pin);
	}
	std::sort(data, data + 100);
	int max = 0;
	int left = -1;
	int right = -1;
	for (int i = 0; i < 10; i++)
    {
        uint32_t* it = std::upper_bound(data + i, data + 100, data[i] + tolerance);
        if(it - data - i > max)
        {
            max = it - data - i;
            left = i;
            right = it - data - 1;
        }
    }
	if(left == -1)
	{
		Serial.println("ADC Read Error");
		return 0;
	}
	else
	{
		uint32_t sum = 0;
		for (int i = left; i <= right; i++)
		{
			sum += data[i];
		}
		return linear_regression(double(sum / (right - left + 1)));
	}
}