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
	unsigned short lowMicrosec;
	unsigned short highMicrosec;
	unsigned short lowSignal = 0;
	unsigned short highSignal = 0;
	unsigned char reliability;

	for (lowMicrosec = 0; lowMicrosec < 80; lowMicrosec++)
	{
		if (digitalRead(signal) == LOW)
		{
			lowSignal++;
		}

		delayMicroseconds(1);
	}

	for (highMicrosec = 0; highMicrosec < 80; highMicrosec++)
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

	printf("[Ready Signal]\tLOW: %d/%d | HIGH: %d/%d\t%c\n", lowSignal, lowMicrosec, highSignal, highMicrosec, reliability);
}


void readData()
{
	unsigned short lowMicrosec;
	unsigned short highMicrosec;
	unsigned short lowSignal = 0;
	unsigned short highSignal = 0;
	unsigned char reliability;

	for (lowMicrosec = 0; lowMicrosec < 50; lowMicrosec++)
	{
		if (digitalRead(signal) == LOW)
		{
			lowSignal++;
		}

		delayMicroseconds(1);
	}

	for (highMicrosec = 0; highMicrosec < 30; highMicrosec++)
	{
		if (digitalRead(signal) == HIGH)
		{
			highSignal++;
		}
			
		delayMicroseconds(1);
	}

	printf("[Data Signal]\tLOW: %d/%d | HIGH: %d/%d\n\n", lowSignal, lowMicrosec, highSignal, highMicrosec);
}


// This function just display signal state
// LOW = 0, HIGH = 1
// Check for natural signal length without delay() function
void readDataTest()
{
	unsigned char s;
	unsigned char row = 0;

	for (unsigned short i = 0; i < 500; i++)
	{
		if (digitalRead(signal) == LOW)
		{
			s = 0;
		}

		else
		{
			s = 1;
		}

		printf("%d ", s);
		row++;

		if (row >= 100)
		{
			printf("\n");
			row = 0;
		}
	}

	printf("\n");
}


int main(void)
{
	// GPIO Initialization
	if (wiringPiSetupGpio() == -1)
	{
		printf("[x_x] GPIO Initialization FAILED.\n");
		return -1;
	}

	for (unsigned char i = 0; i < 10; i++)
	{
		pinMode(signal, OUTPUT);

		// Send out start signal
		digitalWrite(signal, LOW);
		delay(20);					// Stay LOW for 5~30 milliseconds
		pinMode(signal, INPUT);		// 'INPUT' equals 'HIGH' level. And signal read mode
		delayMicroseconds(20);		// Wait for DHT22 ready signal

		readReadySignal();			// Read DHT22 ready signal
		readDataTest();
		delay(2500);				// DHT22 average sensing period is 2 seconds
	}

	return 0;
}
