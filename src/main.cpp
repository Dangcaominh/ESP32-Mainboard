#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <AiEsp32RotaryEncoder.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <IRremote.h>
#include <RTClib.h>

#define ROTARY_ENCODER_A_PIN 26
#define ROTARY_ENCODER_B_PIN 25
#define ROTARY_ENCODER_BUTTON_PIN 27
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

// depending on your encoder - try 1,2 or 4 to get expected behaviour
// #define ROTARY_ENCODER_STEPS 1
// #define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

// instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

Adafruit_MCP4725 dac;

const int ReceivePin = 36;

void on_button_click();
void handle_rotary_button();
void rotary_loop(void *parameter);
void IRAM_ATTR readEncoderISR();
void getTime(void *param);

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup()
{
	Serial.begin(115200);
	while (!rtc.begin())
	{
		Serial.println("Couldn't find RTC");
		Serial.flush();
	}

	if (rtc.lostPower())
	{
		Serial.println("RTC lost power, let's set the time!");
		// When time needs to be set on a new device, or after a power loss, the
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
	IrReceiver.begin(ReceivePin, true, 13); // true: chế độ giám sát
	while (!dac.begin(0x60))
	{
		Serial.println("DAC not found");
	}
	// we must initialize rotary encoder
	rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
	bool circleValues = false;
	rotaryEncoder.setBoundaries(0, 450, circleValues);
	rotaryEncoder.setAcceleration(250); // or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
	xTaskCreatePinnedToCore(rotary_loop, "Rotary Loop", CONFIG_ARDUINO_LOOP_STACK_SIZE, NULL, 4, NULL, 0);
	xTaskCreatePinnedToCore(getTime, "Get Time", CONFIG_ARDUINO_LOOP_STACK_SIZE, NULL, 1, NULL, 1);
}

void loop()
{
	if (IrReceiver.decode() > 0)
	{
		uint32_t dataRemote = IrReceiver.decodedIRData.decodedRawData; // Dữ liệu thô đã giải mã gán vào biến dataRemote
		if (dataRemote > 0)
		{
			Serial.println("Dữ liệu nhận được : " + String(dataRemote));
			Serial.println("Xin hãy nhấn nút !");
		}
		IrReceiver.resume(); // Cho phép nhận giá trị tiếp theo
	}
}

void getTime(void* param)
{
	while(1)
	{
		DateTime now = rtc.now();

		Serial.print(now.year(), DEC);
		Serial.print('/');
		Serial.print(now.month(), DEC);
		Serial.print('/');
		Serial.print(now.day(), DEC);
		Serial.print(" (");
		Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
		Serial.print(") ");
		Serial.print(now.hour(), DEC);
		Serial.print(':');
		Serial.print(now.minute(), DEC);
		Serial.print(':');
		Serial.print(now.second(), DEC);
		Serial.println();
		
		Serial.print("Temperature: ");
		Serial.print(rtc.getTemperature());
		Serial.println(" C");

		Serial.println();
		vTaskDelay(1000);
	}
}

void on_button_click()
{
	Serial.print("button press ");
	Serial.print(millis());
	Serial.println(" milliseconds after restart");
}

void handle_rotary_button()
{
	static bool wasButtonDown = false;

	bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
	// isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

	if (isEncoderButtonDown)
	{
		// we wait since button is still down
		wasButtonDown = true;
		return;
	}

	// button is up
	if (wasButtonDown)
	{
		on_button_click();
	}
	wasButtonDown = false;
}

void rotary_loop(void *parameter)
{
	while (1)
	{
		// dont print anything unless value changed
		if (rotaryEncoder.encoderChanged())
		{
			Serial.print("Value: ");
			Serial.println(rotaryEncoder.readEncoder());
			float preValue = (float(rotaryEncoder.readEncoder()) / 100) / (4.973) * 4095;
			uint16_t value = uint16_t(preValue);
			if (preValue - value >= 0.5)
			{
				value++;
			}
			Serial.println(value);
			dac.setVoltage(value, false);
		}
		handle_rotary_button();
		vTaskDelay(1);
	}
}

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}