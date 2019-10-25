//Google Startups- BUILD 1.0 Arduino
#include <SPI.h> //Import SPI librarey 
#include <RH_RF95.h> // RF95 from RadioHead Library
#include <RHEncryptedDriver.h> 
#include <Speck.h>
//#include <Wire.h>//BMO280 já incluso no Sparkfun....
#include <Adafruit_BMP085.h>//BMO280
#include "SparkFun_SCD30_Arduino_Library.h"

#define RFM95_CS 10 //CS pin 10
#define RFM95_RST 9 //RST pin 9
#define RFM95_INT 2 //INT pin 2

#define RF95_FREQ 915.0

SCD30 airSensor;

Adafruit_BMP085 bmp180;

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHEncryptedDriver myDriver(rf95,myCypher);
unsigned char key[16]={G,o,o,g,l,e,s,t,a,r,t,u,p,1,2,3}
char sinal[];
uint8_t sinalLen;

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
  
  uint8_t data[sinalLen + 1]={0}
  String co2;
  String temp;
  String hum;
  String alt;
  String pa;
  float tmp;
  
  //SCD30
  if (airSensor.dataAvailable()) {

    co2 = airSensor.getCO2());

    tmp = airSensor.getTemperature();
    //dtostrf(tmp, 5, 2, temp);

    hum = airSensor.getHumidity();
  }
  else
    Serial.println("Não há dados");

  //BMO280

  tmp += bmp180.readTemperature(); //média de temperatura entre os dois sensores

  temp = tmp/2
  
  alt = bmp180.readAltitude(); //em metros

  pa = bmp180.readPressure();


  sinal = "co2,"+co2+",temp,"+temp+",hum,"+hum+",alt,"+alt+",pa,"+pa;
  
  sinalLen = strLen(sinal)

  for(uint8_t i =0;i<=sinalLen;i++){
    data[i] = (uint8_t)sinal[i];
    }
  
  //envio de informação coletada
  myDriver.send(data,sizeof(data));

  //  rf95.send((uint8_t *)radiopacket, 1);

  delay(1800000);

}
