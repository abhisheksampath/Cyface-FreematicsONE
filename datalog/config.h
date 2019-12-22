#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

//#define WIFI_SSID "samm"
//#define WIFI_PASSWORD "123456789asd"
//#define SERVER_URL "192.168.137.1"
//#define SERVER_PORT 8080

const int MPU_addr=0x68;  // I2C address of the MPU-6050
const int acc4g = 8192;
bool ready = false;
const int chipSelect = 10;

#define GPS_SERIAL_BAUDRATE 115200L


#endif // CONFIG_H_INCLUDED
