/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/
#include <Arduino.h>
#include <Wire.h>

#ifndef STASSID
#define STASSID "Dang Trung Thang" // set your SSID
#define STAPSK "20041106"		   // set your wifi password
#endif

#define SDA_PIN 21
#define SCL_PIN 22

/* Necessary Includes */
#include <SPI.h>
#include <WiFi.h>	   // we need wifi to get internet access
#include <ModifiedNTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
WiFiUDP wifiUdp;
ModifiedNTPClient timeClient(wifiUdp, "at.pool.ntp.org", 7 * 3600, 60000); // Ajust for your location
#include "RTClib.h"
RTC_DS3231 rtc;

QueueHandle_t transmitQueue = xQueueCreate(20, 40 * sizeof(char));
SemaphoreHandle_t transmitMutex = xSemaphoreCreateMutex();

void communicate(void *parameter)
{
	while (true)
	{
		char data[40];
		if (xQueuePeek(transmitQueue, data, 0) == pdTRUE)
		{
			while (xQueueReceive(transmitQueue, data, 0) == pdTRUE)
			{
				String message;
				unsigned int i = 0;
				while (data[i] != '\0')
				{
					message += data[i];
					i++;
				}
				Serial.println(message);
				Wire.beginTransmission(0x55);
				Wire.write(message.c_str());
				Wire.endTransmission();
			}
		}
		Wire.requestFrom(0x55, 30 * sizeof(char));
		if (Wire.available())
		{
			String c = Wire.readStringUntil('|');
			if (c != "NO DATA")
			{
				Serial.println(c);
			}
		}
		vTaskDelay(10);
	}
}

TaskHandle_t communicateISRTaskHandle = NULL;
SemaphoreHandle_t messageISRMutex = xSemaphoreCreateMutex();
String messageISR = "";

void communicateISR(void *parameter)
{
	while (true)
	{
		if (xSemaphoreTake(transmitMutex, 0) == pdTRUE)
		{
			if (messageISR != "")
			{
				Wire.beginTransmission(0x55);
				messageISR = "ISR:" + messageISR ;
				Wire.write(messageISR.c_str());
				Wire.endTransmission();
				messageISR = "";
			}
			xSemaphoreGive(transmitMutex);
		}
		vTaskSuspend(NULL);
	}
}

void updateTime(void *parameter)
{
	while (true)
	{
		DateTime now = rtc.now();
		String zero[5] = {"", "", "", "", ""};
		if (now.month() < 10)
		{
			zero[0] = "0";
		}
		if (now.day() < 10)
		{
			zero[1] = "0";
		}
		if (now.hour() < 10)
		{
			zero[2] = "0";
		}
		if (now.minute() < 10)
		{
			zero[3] = "0";
		}
		if (now.second() < 10)
		{
			zero[4] = "0";
		}
		String DateTimeSend = "TIME&" + String(now.year(), DEC) + '-' + zero[0] + String(now.month(), DEC) + '-' + zero[1] + String(now.day(), DEC) + ' ' + zero[2] + String(now.hour(), DEC) + ':' + zero[3] + String(now.minute(), DEC) + ':' + zero[4] + String(now.second(), DEC) + '|';
		if (xSemaphoreTake(messageISRMutex, 0) == pdTRUE)
		{
			messageISR = DateTimeSend;
			vTaskResume(communicateISRTaskHandle);
			xSemaphoreGive(messageISRMutex);
		}
		vTaskDelay(1000);
	}
}

//-----------------------------------------------------------------------------------------
void lostpower()
{
	timeClient.update(); //
	ModifiedDateTime now = timeClient.getDateTime();

	rtc.adjust(DateTime(now.year, now.month, now.day, now.hour, now.minute, now.second));
	Serial.println("RTC adjusted! " + String(now.year) + " " + String(now.day) + " " + String(now.day) + " " + String(now.hour) + " " + String(now.minute) + " " + String(now.second));
}

/* Globals */
//-----------------------------------------------------------------------------------------
void setup()
{
	Serial.begin(9600);
	xSemaphoreGive(transmitMutex);
	xSemaphoreGive(messageISRMutex);
	Wire.begin(SDA_PIN, SCL_PIN, 400000);
	if (!rtc.begin(&Wire))
	{
		Serial.println("Couldn't find RTC");
		Serial.flush();
		while (1)
			delay(10);
	}
	// start network
	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	WiFi.begin(STASSID, STAPSK);
	Serial.print("\n");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(100);
	}
	Serial.println("\nWiFi connected");
	Serial.println("start NTP");
	timeClient.begin();							   //
	timeClient.update();						   //
	timeClient.forceUpdate();					   //
	Serial.println(timeClient.getFormattedTime()); //
	lostpower();
	xTaskCreatePinnedToCore(communicate, "communicate", 10000, NULL, 3, NULL, 1);
	xTaskCreatePinnedToCore(communicateISR, "communicateISR", 10000, NULL, 5, &communicateISRTaskHandle, 1);
	xTaskCreatePinnedToCore(updateTime, "updateTime", 10000, NULL, 4, NULL, 1);
}

//-----------------------------------------------------------------------------------------
void loop()
{
	xQueueSend(transmitQueue, "UPDATE BUCK VOLTAGE&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK VOLTAGE&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST VOLTAGE&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST VOLTAGE&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK CURRENT&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK CURRENT&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST CURRENT&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST CURRENT&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK POWER&888.88|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK POWER&11.11|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST POWER&888.88|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST POWER&11.11|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK SET VOLTAGE&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK SET VOLTAGE&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST SET VOLTAGE&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST SET VOLTAGE&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK SET CURRENT&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BUCK SET CURRENT&0.05|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST SET CURRENT&11.15|", 0);
	vTaskDelay(100);
	xQueueSend(transmitQueue, "UPDATE BOOST SET CURRENT&0.05|", 0);
	vTaskDelay(100);
}
