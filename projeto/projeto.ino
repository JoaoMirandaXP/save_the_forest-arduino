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
 
int mostrador = 0;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  
  Wire.begin();

//Initialize Serial Monitor
  Serial.begin(9600);
  
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

char value = 48;

void loop()
{
//SCD30  
if (airSensor.dataAvailable()){
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }
  else
    Serial.println("Não há dados");

//  delay(1800000);


//BMO280
  Serial.print("Temperatura : ");
   if ( bmp180.readTemperature() < 10){
     Serial.print(bmp180.readTemperature());
     Serial.println(" C");
   }else{
     Serial.print(bmp180.readTemperature(),1);
     Serial.println(" C");
   }    
   if (mostrador == 0){
     Serial.print("Altitude : ");
     Serial.print(bmp180.readAltitude());
     Serial.println(" m");
    }
   if (mostrador == 1){
     Serial.print("Pressao : ");
     Serial.print(bmp180.readPressure());  
     Serial.println(" Pa");
   }
    
   delay(3000);
   mostrador = !mostrador;
  
// LoRa
  Serial.print("Send: ");
  
  char radiopacket = char(value);
  
  rf95.send((uint8_t *)radiopacket, 1);

    
  delay(1000);
  value++;
  if (value > '9')
  value = 48;
}
