/*
 * DHT22 for Raspberry Pi with WiringPi
 * Author: Hyun Wook Choi
 * Version: 0.0.1
 * https://github.com/ccoong7/DHT22
 */


#include <stdio.h>
#include <wiringPi.h>

static const unsigned char signal = 18;


void readReadySignal()
{
	unsigned short microsec;
	unsigned short lowSignal = 0;
	unsigned short highSignal = 0;
	unsigned char reliability;

	for (microsec = 0; microsec < 80; microsec++)
	{
		if (digitalRead(signal) == LOW)
		{
			lowSignal++;
		}

		delayMicroseconds(1);
	}

	for (microsec = 0; microsec < 80; microsec++)
	{
		if (digitalRead(signal) == HIGH)
		{
			highSignal++;
		}

		delayMicroseconds(1);
	}

	if (lowSignal < 75 || highSignal < 75)
	{
		reliability = 'x';
	}

	else
	{
		reliability = ' ';
	}

	printf("[RESULT] LOW: %d | HIGH: %d | Microsec: %d  %c\n", lowSignal, highSignal, microsec, reliability);
}


int main(void)
{
	unsigned char i;

	// GPIO Initialization
	if (wiringPiSetupGpio() == -1)
	{
		printf("[x_x] GPIO Initialization FAILED.\n");
		return -1;
	}

	for (i = 0; i < 10; i++)
	{
		pinMode(signal, OUTPUT);

		// Send out start signal
		digitalWrite(signal, LOW);
		delay(20);					// Stay LOW for 5~30 milliseconds
		pinMode(signal, INPUT);		// 'INPUT' equals 'HIGH' level. And signal read mode
		delayMicroseconds(20);		// Wait for DHT22 ready signal

		readReadySignal();			// Read DHT22 ready signal
		delay(2500);				// DHT22 average sensing period is 2 seconds
	}

	return 0;
}
