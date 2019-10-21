//Google Startups- BUILD 1.0 Arduino

#include <SPI.h> //Import SPI librarey 
#include <RH_RF95.h> // RF95 from RadioHead Librarey 
#include <Wire.h>//BMO280
#include <Adafruit_BMP085.h>//BMO280
#include "SparkFun_SCD30_Arduino_Library.h" 

#define RFM95_CS 10 //CS pin 10
#define RFM95_RST 9 //RST pin 9
#define RFM95_INT 2 //INT pin 2

#define RF95_FREQ 915.0

SCD30 airSensor;

Adafruit_BMP085 bmp180;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  
//Initialize Serial Monitor
  Serial.begin(9600);

  Wire.begin();

//Setup SCD30 OBS:Conectar o SCD na porta analógica
  airSensor.begin(); //This will cause readings to occur every two seconds

//Inicializa BMO280
  if (!bmp180.begin()) {
    Serial.println("Sensor nao encontrado !!");
    while (1) {}
  }
  
// Reset LoRa Module 
  pinMode(RFM95_RST, OUTPUT); 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

//Initialize LoRa Module
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  

 //define a frequencia padrao 915.0MHz
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(18); //Transmission power of the Lora Module
}

//char value = 48;

void loop()
{
char co2[8];
char temp[8];
char hum[8];
char alt[8];
char pa[8];
float tmp;

//SCD30  
  if (airSensor.dataAvailable()){

    sprintf(co2,"%d",airSensor.getCO2()); 
    
    tmp = airSensor.getTemperature();
    //dtostrf(tmp, 5, 2, temp);
    
    dtostrf(airSensor.getHumidity(), 5, 2, hum);
  }
  else
    Serial.println("Não há dados");

//BMO280  
  
  dtostrf((tmp + bmp180.readTemperature())/2, 5 , 2 , temp);//média de temperatura entre os dois sensores
  
  sprintf(alt ,"%d", bmp180.readAltitude());//em metros
  
  sprintf(pa,"%d", bmp180.readPressure());  

//envio de informação coletada
  rf95.send((uint8_t *)co2 , sizeof(co2));
  rf95.send((uint8_t *)temp , sizeof(temp));
  rf95.send((uint8_t *)hum , sizeof(hum));
  rf95.send((uint8_t *)alt , sizeof(alt));
  rf95.send((uint8_t *)pa , sizeof(pa));
    
//  char radiopacket[7] = {1,2,3,3,4,5,5};
  
//  rf95.send((uint8_t *)radiopacket, 1);

  delay(1800000);

}
