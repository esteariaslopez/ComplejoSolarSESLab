//This is a test of the comunication between Arduino UART to KNX based on the tutorial https://thingtype.com/blog/hacking-a-knx-bus-with-an-arduino-like-board/ 
//Programer Esteban Arias
//inicial programation: 30/04/2018
//                      02/05/2018 - New Communications Objects added (Index 13, Index 14, Index 15)
//                                 

#include "Arduino.h"
#include <KnxDevice.h>
//#include <SolarBatteryMonitorKNX.h>
#include <Event.h>
#include <Timer.h>

////////GLOBAL VARIABLES//////////////////
word VOLTAGE = 240; 
word CURRENT = 0; 
word TEMP = 0; 
//boolean VOLT_ALARM = 0; 
//boolean CURR_ALARM = 0; 
//boolean TEMP_ALARM = 0; 
byte VOLT_TH = 0; 
byte CURR_TH = 0; 
byte TEMP_TH = 0; 
byte T_VOLT_FACT = 10; 
byte T_CURR_FACT = 15; 
byte T_TEMP_FACT = 20;
int TimeBase = 1; //seconds

/////////////////////////////////////////////////
//Temperature NTC constats
//NTC
//Cambiar el siguiente valor segun lo que dispongamos
const float Rf = 9839;  //Resistencia fija, en ohms
const float BETA = 3926.5866;//3850;//3435; //Factor Beta del sensor
///////////////////////////////////////////
//TEMPERATURA
const float T0 = 297.45;//298.15; //Temperatura nominal, en kelvins (25ºC + 273)
const float R0 = 10536;//10000; //Resistencia nominal den sensor
const float Vs = 3.31;   //Voltaje de alimentacion de Arduino
const float e = 2.71828; //Numero e
/////////////////////////////////////////////////

boolean Volt_CycSend = false; //Envio ciclico del valor de tension
boolean Curr_CycSend = true; //Envio ciclico del valor de corriente
boolean Temp_CycSend = false; //Envio ciclico del valor de temperatura

////////////TIME SETTINGS///////////////////////
Timer t;
//boolean toggle = false; //Prueba

//////////////////////////////////////////////
//////////COMUNICATION OBJECT
KnxComObject KnxDevice::_comObjectsList[] = 
{
 /* Index 0 : */KnxComObject(G_ADDR(0, 0, 1), KNX_DPT_9_020 /*9.020 DPT_Value_Volt*/     , 0x34 /* CRT */), // Voltaje
 /* Index 1 : */KnxComObject(G_ADDR(0, 0, 2), KNX_DPT_9_021 /*9.021 DPT_Value_Curr*/     , 0x34 /* CRT */), // Corriente
 /* Index 2 : */KnxComObject(G_ADDR(0, 0, 3), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/     , 0x34 /* CRT */), // Temperatura
 /* Index 3 : */KnxComObject(G_ADDR(0, 0, 4), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , 0x34 /* CRT */), // Volt Alarma
 /* Index 4 : */KnxComObject(G_ADDR(0, 0, 5), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , 0x34 /* CRT */), // Curr Alarma
 /* Index 5:  */KnxComObject(G_ADDR(0, 0, 6), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , 0x34 /* CRT */), // Temp Alarma
 /* Index 6 : */KnxComObject(G_ADDR(0, 0, 7), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */), // Volt threshold
 /* Index 7 : */KnxComObject(G_ADDR(0, 0, 8), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */), // Curr threshold
 /* Index 8 : */KnxComObject(G_ADDR(0, 0, 9), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */), // Temp threshold
 /* Index 9 : */KnxComObject(G_ADDR(0, 0, 10), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Volt Period
 /* Index 10: */KnxComObject(G_ADDR(0, 0, 11), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Curr Period
 /* Index 11: */KnxComObject(G_ADDR(0, 0, 12), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Temp Period
 /* Index 12: */KnxComObject(G_ADDR(0, 0, 13), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Base Time (0->1s, 1->1min)
 /* Index 13: */KnxComObject(G_ADDR(0, 0, 14), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Voltage Cyclical Send Active
 /* Index 14: */KnxComObject(G_ADDR(0, 0, 15), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Current Cyclical Send Active
 /* Index 15: */KnxComObject(G_ADDR(0, 0, 16), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Temp Cyclical Send Active
};

const byte KnxDevice::_comObjectsNb = sizeof(_comObjectsList) / sizeof(KnxComObject); // do no change this code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////CALLBACK FUNCTION////////////////////////////////////
void knxEvents( byte index )
{
 switch ( index )
 {
 case 0: // Object index 0 has been updated
    IndexFun0();
 break;
 
 case 1: // Object index 1 has been updated
    IndexFun1();
 break;
 
 case 2: // Object index 2 has been updated
    IndexFun2();
 break;
 
 case 3: // Object index 3 has been updated
    IndexFun3();
 break;
 
 case 4: // Object index 4 has been updated
    IndexFun4();
 break;
 
 case 5: // Object index 5 has been updated
    IndexFun5();
 break;
 
 case 6: // Object index 6 has been updated
    IndexFun6();
 break;
 
 case 7: // Object index 7 has been updated
    IndexFun7();
 break;
 
 case 8: // Object index 8 has been updated
    IndexFun8();
 break;
 
 case 9: // Object index 9 has been updated
    IndexFun9();
 break;
 
 case 10: // Object index 10 has been updated
    IndexFun10();
 break;
 
 case 11: // Object index 11 has been updated
    IndexFun11();
 break;
 
 case 12: // Object index 12 has been updated
    IndexFun12();
 break;
 case 13: // Object index 12 has been updated
    IndexFun13();
 break;
 case 14: // Object index 12 has been updated
    IndexFun14();
 break;
 case 15: // Object index 12 has been updated
    IndexFun15();
 break;

 default:
 break;
 }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////INDEX FUNCTIONS////////////////////
void IndexFun0(){
    Knx.write(0,VOLTAGE);
  };
void IndexFun1(){
    Knx.write(1,CURRENT);
  };
void IndexFun2(){
    Knx.write(2,TEMP);
  };
void IndexFun3(){
    //Knx.write(3,VOLT_ALARM);
   };
void IndexFun4(){
    //Knx.write(3,CURR_ALARM);
  };
void IndexFun5(){
    //Knx.write(3,TEMP_ALARM);
  };
void IndexFun6(){
    byte VOLT_TH = Knx.read(6);
  };
void IndexFun7(){
    byte CURR_TH = Knx.read(7);
  };
void IndexFun8(){
    byte TEMP_TH = Knx.read(8);
  };
void IndexFun9(){
    byte VOLT_TL = Knx.read(9);
  };
void IndexFun10(){
    byte CURR_TL = Knx.read(10);
  };
void IndexFun11(){
    byte TEMP_TL = Knx.read(11);
  };
void IndexFun12(){
    if (Knx.read(12)){TimeBase = 60;}
    else{TimeBase = 1;};
   };
void IndexFun13(){
    if (Knx.read(13)){Volt_CycSend = true; digitalWrite(LED_BUILTIN, HIGH);}
    else{Volt_CycSend = false; digitalWrite(LED_BUILTIN, LOW);};
   };
void IndexFun14(){
    if (Knx.read(14)){Curr_CycSend = true;}
    else{Curr_CycSend = false;};
   };
void IndexFun15(){
    if (Knx.read(15)){Temp_CycSend = true;}
    else{Temp_CycSend = false;};
   };
/////////END INDEX FUNCTIONS//////////////////
////SETUP//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);//
  //pinMode(LED_BUILTIN, OUTPUT);
  //Serial.print("Program Started..");
  if (Knx.begin(Serial1, P_ADDR(1,1,3)) == KNX_DEVICE_ERROR) {Serial.println("knx init ERROR, stop here!!");while(1);}
  Serial.println("knx bus started...");
  t.every(1000*TimeBase*T_VOLT_FACT, Read_Volt);
  t.every(1000*TimeBase*T_CURR_FACT, Read_Curr);
  t.every(1000*TimeBase*T_TEMP_FACT, Read_Temp);
}

void loop()
{
  Knx.task();
  t.update();
}

/////////SENSOR FUNCTIONS////////////////////
void Read_Volt()
{
  //Serial.println("Reading Voltage");
  //READING VOLTAGE///////////////////////////////////////////////////
  int sensorValue = analogRead(1);
  float voltage = sensorValue*0.0988+0.002; //Curva de mejor ajuste
  Serial.println((voltage));
  VOLTAGE = voltage;
  ////////////////////////////////////////////////////////////////////
  //if (VOLT_TH >= VOLTAGE){Knx.write(3,1);} //send Over Voltage Alarm
  if (Volt_CycSend){Knx.write(0,VOLTAGE);} //Send Cyclical Voltage
}
void Read_Curr()
{
  //READING CODE
  int sensorValue = analogRead(0);
  float Current = sensorValue*0.2932-149.8777;
  float Current1 = Current*0.9975-0.6737;
  ///////////////////////////////////
  Serial.println("Reading Currrent");
  CURRENT = Current1;
  
  //if (CURR_TH >= CURRENT){Knx.write(4,1);} //Send Over Current Alarm
  if (Curr_CycSend){Knx.write(1,CURRENT);} //Send Cyclical Current
}
void Read_Temp()
{
  //READING CODE
  float sensorValue = analogRead(2);
  //Segunda opcion para convertir a resistencia sin pasar por el voltaje
  float Rt2 = -((sensorValue * Rf) / (sensorValue - 1023));
  //Calculo final de temperatura
  float Temp = 1 / (1 / T0 + (log(Rt2 / R0) / log(e)) / BETA) - 273.15;

  TEMP = Temp;
  ///////////////////////////////////////////////////////////////////////
  Serial.println(TEMP);
  //if (TEMP_TH >= TEMP){Knx.write(5,1);} //Send Over Temperature Alarm
  if (Temp_CycSend){Knx.write(2,TEMP);} //Send Cyclical Temperature
}





//  if (toggle){
//    digitalWrite(LED_BUILTIN, HIGH);
//    toggle = false;}
//    else{
//      digitalWrite(LED_BUILTIN, LOW);
//      toggle = true;}


