/*
 * DHT22 for Raspberry Pi with WiringPi
 * Original code by Hyun Wook Choi: https://github.com/ccoong7/DHT22
 *
 * dht22_json.c
 * 
 * Version: 0.2.0
 *
 * Features of this adapted version by d5c0d3:
 * - next to temperature and humidity export of
 *     - datetime of reading,
 *     - seconds to successfully read,
 *     - count of tries to read.
 * - export in JSON format for further processing (send to 'stdout')
 * - write other messages to 'stderr'
 * - stop after first correct reading
 *
 */


#include <stdio.h>
#include <time.h>
#include <wiringPi.h>

static const unsigned short signal = 18;
unsigned short data[5] = {0, 0, 0, 0, 0};


short readData()
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
				val <<= 1;		// 0 bit. Just shift left
			}

			else if (signal_length < 30)
			{
				// 26~28us means 0 bit
				val <<= 1;		// 0 bit. Just shift left
			}

			else if (signal_length < 85)
			{
				// 70us means 1 bit
				// Shift left and input 0x01 using OR operator
				val <<= 1;
				val |= 1;
			}

			else
			{
				// Unstable signal
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
	short maxtry;
	struct timespec start, end;
	double elapsedTime;
	FILE *fs;

	// GPIO Initialization
	if (wiringPiSetupGpio() == -1)
	{
		fprintf( stderr, "ERROR: GPIO Initialization FAILED.\n");
		return -1;
	}

	maxtry = 20;
	//start timer
	clock_gettime(CLOCK_REALTIME, &start);
	
	for (unsigned char i = 0; i < maxtry; i++)
	{
		pinMode(signal, OUTPUT);

		// Send out start signal
		digitalWrite(signal, LOW);
		delay(20);			// Stay LOW for 5~30 milliseconds
		pinMode(signal, INPUT);		// 'INPUT' equals 'HIGH' level. And signal read mode
		
		readData();			// Read DHT22 signal

		// The sum is maybe over 8 bit like this: '0001 0101 1010'.
		// Remove the '9 bit' data using AND operator.
		checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;

		// If Check-sum data is correct (NOT 0x00), display humidity and temperature
		if (data[4] == checksum && checksum != 0x00)
		{
			// stop timer
			clock_gettime(CLOCK_REALTIME, &end);
			// compute and print the elapsed time in sec
			elapsedTime = (end.tv_sec - start.tv_sec);
			
			// * 256 is the same thing '<< 8' (shift).
			humidity = ((data[0] * 256) + data[1]) / 10.0;
			celsius = (((data[2] & 0x7F)*256) + data[3]) / 10.0; //changed acc. Juergen Wolf-Hofer

			// If 'data[2]' data like 1000 0000, It means minus temperature
			if (data[2] == 0x80)
			{
				celsius *= -1;
			}
			
			fahrenheit = ((celsius * 9) / 5) + 32;

			// time
			char time_buff[100];
			time_t now = time (0);
			strftime (time_buff, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
			//puts(time_buff);
			//printf ("%s\n", time_buff);

			// Display all data
			//printf("TEMP: %6.2f *C (%6.2f *F) | HUMI: %6.2f %\n\n", celsius, fahrenheit, humidity);
			fprintf(stderr, "Elapsed time (sec): %.0f, DateTime: %s, Temp(C)=%.1f, Hum(%)=%.1f\n", elapsedTime, time_buff, celsius, humidity); //CSV format
			
			//write json to stdout
			printf("{'elapsed_time_sec': %.0f, 'try_count': %d, 'datetime': '%s', 'temp_C': %.2f, 'temp_F': %.2f, 'humid_perc': %.2f}\n", elapsedTime, i, time_buff, celsius, fahrenheit, humidity); //JSON format
			
			// exit program if reading was successful
			return 0;
		} else {
			fprintf(stderr, "Error: Invalid data (attempt %d/%d). Try again...\n", i+1, maxtry);
		}
		// Initialize data array for next loop
		for (unsigned char j = 0; j < 5; j++)
		{
			data[j] = 0;
		}

		delay(2000);	// DHT22 average sensing period is 2 seconds
	}
	fprintf(stderr, "Maximum attempts reached. Stop reading.\n");
	printf("");
	return 0;
}
