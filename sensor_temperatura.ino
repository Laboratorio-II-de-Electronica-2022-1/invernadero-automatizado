#include<Adafruit_NeoPixel.h>
#include<Wire.h>
#include <Adafruit_SH1106.h>
#include <SPI.h>
#include "Ucglib.h"
 
//pines para la conexión de pantalla RGB
#define cs 53
#define dc 9
#define rst 8

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ 53, /*reset=*/ 8);

//auxiliares
int n=2;

const int bomba =13;
const int humedadSuelo=A0;
float sensor;
float vsensor=5;
float temperatura;
int fan = 7;
int numpix = 8;
int luz = 4;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numpix, luz, NEO_RGB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(fan,OUTPUT);
  pinMode(luz, OUTPUT);
  digitalWrite(luz, HIGH);
  pixels.begin();
  pixels.show();
  
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
    sensor = analogRead(A1);
    temperatura = (sensor/1023)*vsensor*100-37;

//ventilador   
  if(temperatura <= 20){
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
  if(temperatura <= 20){
    ucg.print("OFF");
  }
  else{
    ucg.print("ON");
    }

//luz

  for(int i=0; i<numpix; i++){
      pixels.setPixelColor(i, pixels.Color(0, 255, 255));
      pixels.show(); 
    }

//sistema de riego

  int SensorValue = analogRead(humedadSuelo);//Se lee el valor de la humedad y lo guardamos en SensorValue
  Serial.print(SensorValue); Serial.print("-");
  //Imprimir el valor de la humedad del suelo
  Serial.print("Humedad del suelo: ");Serial.print(SensorValue);Serial.print("%");
  delay(3000);
  
  
  if(SensorValue >= 700)//Valor de prueba para la humedad que se considera seca 
  {
  // Si la tierra esta seca, comenzara a regar
  //  Riega durante 1 segundo y espera a comprobar la humedad otro segundo 
  Serial.println("El nivel de humedad esta bajo, comienza regado");
  digitalWrite(bomba, HIGH);
  delay(2000);
  digitalWrite(bomba, LOW);
  delay(1000);
  }
  //Si la tierra no esta seca, no riega y da el valor de la  humedad
  delay(1000);
  }
