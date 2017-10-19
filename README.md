# DHT22
Try use DHT22 with Raspberry Pi 3! Tested by Raspbian Stretch (ver. Sep. 2017) and WiringPi v2.44.

## How to clone
1. Open the terminal in Raspbian.
2. Check your Git installation.
```
$ sudo apt-get install git-core
```
3. Enter a clone command with this repository address.
```
$ git clone https://github.com/ccoong7/DHT22.git
```

## How to create an executable file
1. When clone success, change directory to DHT22.
```
$ cd ~/DHT22
```
2. Compile source code to generate an executable file.
```
$ gcc -o dht22.out dht22.c -lwiringPi
```
3. Launch!
```
$ ./dht22.out
```
![result](/resources/dht22_result.png)

## Wiring
- Vcc 3.3V
- Signal pin is GPIO 18 (With 5k resistor or 10k parallel)
- 3rd pin is not used
- GND
![wiring](/resources/dht22_wiring.png)

## Reference
Visit my blog and get more information. (in Korean)
- [How to use DHT22?](http://blog.naver.com/ccoong7/221119421273)
- [How DHT22 works and sending out data?](http://blog.naver.com/ccoong7/221119937010)
- [How to process temperature and humidity data?](http://blog.naver.com/ccoong7/221120199514)