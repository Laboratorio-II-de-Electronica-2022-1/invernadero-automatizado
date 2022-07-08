#include<Wire.h>
#include <Adafruit_SH1106.h>
#include <SPI.h>
#include "Ucglib.h"
#include <SoftwareSerial.h>
#include <max6675.h>

//pines termocupla
int sck = 11;
int cs = 12;
int so = 13;
MAX6675 termopar(sck, cs, so);

//npk
#define RE 48
#define DE 49

const byte nitro_inquiry_frame[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos_inquiry_frame[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota_inquiry_frame[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
SoftwareSerial modbus(18,19);
 
//pines para la conexión de pantalla RGB
#define cs 53
#define dc 9
#define rst 8

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 53, /*reset=*/ 8);

//auxiliares
int n=2;

const int bomba =6;
const int humedadSuelo=A0;
int fan = 7;
int luz = 4;

void setup() {
  Serial.begin(9600);
  modbus.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  Wire.begin();
  pinMode(fan,OUTPUT);
  pinMode(luz, OUTPUT);
  digitalWrite(luz, HIGH);
  
  SPI.begin();
  delay(1000);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.setFont(ucg_font_ncenR14_hr);
  ucg.setRotate270();
  ucg.clearScreen();
  
  
  pinMode(A0, INPUT);//Configuramos la humedad del suelo como entrada
  pinMode(bomba, OUTPUT);//Configuro bomba como salida
}

void loop(){
  ucg.clearScreen();
//sensor de temperatura
    int temperatura;
    temperatura = termopar.readCelsius();

//sensor NPK
  byte nitrogen_val,phosphorus_val,potassium_val;
 
  nitrogen_val = nitrogen();
  delay(250);
  phosphorus_val = phosphorous();
  delay(250);
  potassium_val = potassium();
  delay(250);
  
  // The following code is used to send the data to the serial monitor
  // but as we have connected the Bluetooth module, so it will send data to the 
  // Android cell phone Application
  
  ucg.setFont(ucg_font_helvB08_tf);
  ucg.setColor(255, 0, 255);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setPrintPos(5,95);
  ucg.print("  N            P           K");
  ucg.setPrintPos(5,105);
  ucg.setColor(255,255,255);
  ucg.print(nitrogen_val);
  ucg.setPrintPos(50,105);
  ucg.print(phosphorus_val);
  ucg.setPrintPos(90,105);
  ucg.print(potassium_val);
  
  delay(2000);

//ventilador   
  if(temperatura <= 25){
      digitalWrite(fan,LOW);
  }
    else{
      digitalWrite(fan,HIGH);
  }

//oled temperatura

  ucg.setFont(ucg_font_helvB08_tf);
  ucg.setColor(255, 0, 255);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setPrintPos(3,25);
  ucg.print("TEMPERATURA:");
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(3,35);
  ucg.print(temperatura);

//oled motor on/off

  ucg.setFont(ucg_font_helvB08_tf);
  ucg.setColor(255, 0, 0);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setPrintPos(3,45);
  ucg.print("VENTILACION: ");
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(3,55);
  if(temperatura <= 25){
    ucg.print("OFF");
  }
  else{
    ucg.print("ON");
    }

//sistema de riego

  float SensorValue = analogRead(humedadSuelo);//Se lee el valor de la humedad y lo guardamos en SensorValue
  int Sensorprc = 100-(((SensorValue-523)/500)*100);
  
  ucg.setFont(ucg_font_helvB08_tf);
  ucg.setColor(0, 0, 255);
  ucg.setColor(1, 255, 0,0);
  
  ucg.setPrintPos(3,65);
  ucg.print("HUMEDAD:");
  ucg.setPrintPos(3,75);
  ucg.setColor(255,255,255);
  ucg.print(Sensorprc);
  if(Sensorprc <= 9){
    ucg.setPrintPos(11,75);
    ucg.setColor(255,255,255);
    ucg.print("%");
    }
   if(Sensorprc > 9 and Sensorprc <100){
   ucg.setPrintPos(15,75);
   ucg.setColor(255,255,255);
   ucg.print("%");
   }
   if(Sensorprc == 100){
   ucg.setPrintPos(22,75);
   ucg.setColor(255,255,255);
   ucg.print("%");
   }
  
  if(SensorValue >= 700)//Valor de prueba para la humedad que se considera seca 
  {
  // Si la tierra esta seca, comenzara a regar
  //  Riega durante 1 segundo y espera a comprobar la humedad otro segundo 
  ucg.setPrintPos(33,75);
  ucg.setColor(255,255,255);
  ucg.print("humedad baja");
  digitalWrite(bomba, HIGH);
  delay(2000);
  digitalWrite(bomba, LOW);
  delay(1000);
  }
  if(SensorValue < 700){
  ucg.setPrintPos(30,75);
  ucg.setColor(255,255,255);
  ucg.print("humedad adecuada");
  delay(1000);
  }
}

byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(nitro_inquiry_frame,sizeof(nitro_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    // When we send the inquiry frame to the NPK sensor, then it replies with the response frame
    // now we will read the response frame, and store the values in the values[] arrary, we will be using a for loop.
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
   // Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4]; // returns the Nigtrogen value only, which is stored at location 4 in the array
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(phos_inquiry_frame,sizeof(phos_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
   // Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(pota_inquiry_frame,sizeof(pota_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
    //Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
