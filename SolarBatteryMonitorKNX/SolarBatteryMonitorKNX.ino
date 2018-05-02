//This is a test of the comunication between Arduino UART to KNX based on the tutorial https://thingtype.com/blog/hacking-a-knx-bus-with-an-arduino-like-board/ 
//Programer Esteban Arias
//inicial programation: 30/04/2018
//                      02/05/2018 - New Comunications Objects added (Index 13, Index 14, Index 15)
//                                 

#include "Arduino.h"
#include <KnxDevice.h>
#include <SolarBatteryMonitorKNX.h>
#include <Event.h>
#include <Timer.h>

////////GLOBAL VARIABLES//////////////////
word VOLTAGE = 0; 
word CURRENT = 0; 
word TEMP = 0; 
//boolean VOLT_ALARM = 0; 
//boolean CURR_ALARM = 0; 
//boolean TEMP_ALARM = 0; 
byte VOLT_TH = 0; 
byte CURR_TH = 0; 
byte TEMP_TH = 0; 
byte T_VOLT_FACT = 0; 
byte T_CURR_FACT = 0; 
byte T_TEMP_FACT = 0;
int TimeBase = 1; //seconds

boolean Volt_CycSend = false; //Envio ciclico del valor de tension
boolean Curr_CycSend = false; //Envio ciclico del valor de corriente
boolean Temp_CycSend = false; //Envio ciclico del valor de temperatura

////////////TIME SETTINGS///////////////////////
Timer t;

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
 /* Index 8 : */KnxComObject(G_ADDR(0, 0, 10), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Temp threshold
 /* Index 9 : */KnxComObject(G_ADDR(0, 0, 11), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Volt Period
 /* Index 10: */KnxComObject(G_ADDR(0, 0, 12), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Curr Period
 /* Index 11: */KnxComObject(G_ADDR(0, 0, 13), KNX_DPT_5_004 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */), // Sense Temp Period
 /* Index 12: */KnxComObject(G_ADDR(0, 0, 14), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Base Time (0->1s, 1->1min)
 /* Index 13: */KnxComObject(G_ADDR(0, 0, 15), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Voltage Cyclical Send Active
 /* Index 14: */KnxComObject(G_ADDR(0, 0, 16), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Current Cyclical Send Active
 /* Index 15: */KnxComObject(G_ADDR(0, 0, 17), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */), // Temp Cyclical Send Active
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

 default:
 break;
 }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Knx.begin(Serial, P_ADDR(1, 1, 3)); // start a KnxDevice session with physical address 1.1.3 on Serial1 UART
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
  //READING CODE
  if (VOLT_TH >= VOLTAGE){Knx.write(3,1);} //send Over Voltage Alarm
  if (Volt_CycSend){Knx.write(0,VOLTAGE);} //Send Cyclical Voltage
}
void Read_Curr()
{
  //READING CODE
  if (CURR_TH >= CURRENT){Knx.write(4,1);} //Send Over Current Alarm
  if (Curr_CycSend){Knx.write(1,CURRENT);} //Send Cyclical Current
}
void Read_Temp()
{
  //READING CODE
  if (TEMP_TH >= TEMP){Knx.write(5,1);} //Send Over Temperature Alarm
  if (Temp_CycSend){Knx.write(2,TEMP);} //Send Cyclical Temperature
}
