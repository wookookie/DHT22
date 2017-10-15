/*
 * DHT22 for Raspberry Pi with WiringPi
 * Author: Hyun Wook Choi
 * Version: 0.0.3
 * https://github.com/ccoong7/DHT22
 */


#include <stdio.h>
#include <wiringPi.h>

static const unsigned char signal = 18;


char readSignalLength()
{
	unsigned short signal_count = 0;

	while (1)
	{
		// Count only HIGH signal
		while (digitalRead(signal) == HIGH)
		{
			signal_count++;
			
			// When sending data ends, high signal occur infinite.
			// So we have to end this infinite loop.
			if (signal_count >= 200)
			{
				printf("========== END DATA ==========\n\n");
				return -1;
			}

			delayMicroseconds(1);
		}

		// If signal is HIGH
		if (signal_count > 0)
		{
			// The DHT22 sends a lot of unstable signals.
			// So extended the counting range.
			if (signal_count < 10)
			{
				// Unstable signal
				printf("SIGNAL [0*]\t%u\n", signal_count);
				signal_count = 0;
			}

			else if (signal_count < 30)
			{
				// 26~28us means 0 bit
				printf("SIGNAL [0]\t%u\n", signal_count);
				signal_count = 0;
			}

			else if (signal_count < 85)
			{
				// 70us means 1 bit
				printf("SIGNAL [1]\t%u\n", signal_count);
				signal_count = 0;
			}

			else
			{
				// Unstable signal
				printf("[x_x] HIGH signal occurred too long.\t%u\n", signal_count);
				return -1;
			}
		}
	}
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

		readSignalLength();
		delay(2000);				// DHT22 average sensing period is 2 seconds
	}

	return 0;
}
