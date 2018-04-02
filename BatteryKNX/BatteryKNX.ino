//Cambiar el siguiente valor segun lo que dispongamos
const float Rf = 9820;  //Resistencia fija, en ohms
const float BETA = 3986.65;//3850;//3435; //Factor Beta del sensor

////////////////////////////////////////////////
//Constantes que no deberia ser necesario modificar
////////////////////////////////////////////////
const float T0 = 295.65;//298.15; //Temperatura nominal, en kelvins (25ºC + 273)
const float R0 = 11370;//10000; //Resistencia nominal den sensor
const float Vs = 3.3;   //Voltaje de alimentacion de Arduino
const float e = 2.71828; //Numero e
const int avgsize = 32;  //Tamaño del buffer de muestras
float ventanaTemp[avgsize];   //Buffer de muestras
float promedioTemp;           //Variable destino para el promedio
float Rt;                 //Resistencia calculada a partir del voltaje
float Rt2;                //Resistencia calculada a partir del valor del ADC
/////////////////////////////////////////////////
//Constantes Sensor corriente
float current;
const int NC = 5;
const int avgsizeC = 16;  //Tamaño del buffer de muestras
float ventanaCu[avgsizeC];   //Buffer de muestras
float promedioCu;           //Variable destino para el promedio

/////////////////////////////////////////////////
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 1000;           // interval at which to blink (milliseconds)
/////////////////////////////////////////////////
//tabla con valores precalculados (alternativa si se necesita mas velocidad)
//#include "temptable.h"
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
void setup() {
  //Inicializacion de puerto serie
  Serial.begin(115200);

  //Usaremos una referencia externa para el ADC
  analogReference(EXTERNAL);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Temp");

  lcd.setCursor(7, 0);
  lcd.print("Current");

//  attachInterrupt(digitalPinToInterrupt(interruptPin),TempIn, CHANGE);

}

void loop() {
    unsigned long currentMillis = millis();
    
    lcd.setCursor(0, 1);
   // print the number of seconds since reset:
    lcd.print(promedioTemp);

    lcd.setCursor(7, 1);
    // print the number of seconds since reset:
    lcd.print(promedioCu);
    
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    TempIn();
    CurrentIn();
  }
    
    //delay(100);
  }
void TempIn() {
  //Realizamos una lectura
  int adcval = analogRead(A0);
  //Segunda opcion para convertir a resistencia sin pasar por el voltaje
  Rt2 = -((adcval * Rf) / (adcval - 1023));

  //Calculo final de temperatura
  float T = 1 / (1 / T0 + (log(Rt2 / R0) / log(e)) / BETA) - 273.15;
  
  windowavg(ventanaTemp, avgsize, &promedioTemp, T);
}

void CurrentIn() {
  //Realizamos una lectura
  int adcval = analogRead(A1);

  //Calculo final de temperatura
  float C = (adcval*2*NC/1023)-NC;
  Serial.print(adcval);
  Serial.print("  ");
  Serial.println(C);
  windowavg(ventanaCu, avgsizeC, &promedioCu, C);
}

//funcion ventana promedio.
void windowavg(float  win[], int len, float* dest, float valor) {
  *dest = 0;
  for (int i = 0; i < len - 1; i++) {
    win[i] = win[i + 1];
    *dest += win[i];
  }
  win[len - 1] = valor;
  *dest += valor;
  *dest /= len;
}
