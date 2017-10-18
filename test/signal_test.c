/*
 * DHT22 for Raspberry Pi with WiringPi (Signal test)
 * Author: Hyun Wook Choi
 * Version: 0.0.1
 * https://github.com/ccoong7/DHT22
 */


#include <stdio.h>
#include <wiringPi.h>

static const unsigned short signal = 18;
unsigned short data[5] = {0, 0, 0, 0, 0};


short testSignalData()
{
	unsigned short val = 0x00;
	unsigned short signal_length = 0;
	unsigned short val_counter = 0;
	unsigned short loop_counter = 0;
	
	while (1)
	{
		// Count only HIGH signal
		while (digitalRead(signal) == HIGH)
		{
			signal_length++;
			
			// When sending data ends, high signal occur infinite.
			// So we have to end this infinite loop.
			if (signal_length >= 200)
			{
                printf("========== END DATA ==========\n");

                // Display data
                printf("LOOP: %u\n", loop_counter);
                printf("DATA: %04x  %04x  %04x  %04x  %04x\n", data[0], data[1], data[2], data[3], data[4]);

				return -1;
			}

			delayMicroseconds(1);
		}

		// If signal is HIGH
		if (signal_length > 0)
		{
			loop_counter++;	// HIGH signal counting

			// The DHT22 sends a lot of unstable signals.
			// So extended the counting range.
			if (signal_length < 10)
			{
                // Unstable signal
                printf("SIGNAL [0*]\t%u\n", signal_length);

				val <<= 1;		// 0 bit. Just shift left
			}

			else if (signal_length < 30)
			{
                // 26~28us means 0 bit
                printf("SIGNAL [0]\t%u\n", signal_length);

				val <<= 1;		// 0 bit. Just shift left
			}

			else if (signal_length < 85)
			{
                // 70us means 1 bit
                printf("SIGNAL [1]\t%u\n", signal_length);

				// Shift left and input 0x01 using OR operator
				val <<= 1;
				val |= 1;
			}

			else
			{
                // Unstable signal
                printf("[x_x] HIGH signal occurred too long.\t%u\n", signal_length);
				return -1;
			}

			signal_length = 0;	// Initialize signal length for next signal
			val_counter++;		// Count for 8 bit data
		}

		// The first and second signal is DHT22's start signal.
		// So ignore these data.
		if (loop_counter < 3)
		{
			val = 0x00;
			val_counter = 0;
		}

		// If 8 bit data input complete
		if (val_counter >= 8)
		{
            printf("[DATA] %04x\n", val);

			// 8 bit data input to the data array
			data[(loop_counter / 8) - 1] = val;

			val = 0x00;
			val_counter = 0;
		}
	}
}


int main(void)
{
	float humidity;
	float celsius;
	float fahrenheit;
	short checksum;

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

		testSignalData();		// Read DHT22 signal

		// The sum is maybe over 8 bit like this: '0001 0101 1010'.
		// Remove the '9 bit' data using AND operator.
        checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
        printf("SUM : %04x\n", checksum);
		
		// If Check-sum data is correct (NOT 0x00), display humidity and temperature
		if (data[4] == checksum && checksum != 0x00)
		{
			// * 256 is the same thing '<< 8' (shift).
			humidity = ((data[0] * 256) + data[1]) / 10.0;
			celsius = data[3] / 10.0;

			// If 'data[2]' data like 1000 0000, It means minus temperature
			if (data[2] == 0x80)
			{
				celsius *= -1;
			}

			fahrenheit = ((celsius * 9) / 5) + 32;

			// Display all data
			printf("TEMP:%6.2f *C (%6.2f *F) | HUMI: %6.2f %\n\n", celsius, fahrenheit, humidity);
		}

		else
		{
			printf("[x_x] Invalid Data. Try again.\n\n");
		}

		// Initialize data array for next loop
		for (unsigned char i = 0; i < 5; i++)
		{
			data[i] = 0;
		}

		delay(2000);	// DHT22 average sensing period is 2 seconds
	}

	return 0;
}
