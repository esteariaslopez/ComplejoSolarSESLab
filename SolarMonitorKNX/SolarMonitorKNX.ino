//This is a test of the comunication between Arduino UART to KNX based on the tutorial https://thingtype.com/blog/hacking-a-knx-bus-with-an-arduino-like-board/ 
//Programer Esteban Arias
//inicial programation: 30/04/2018
//                      02/05/2018 - New Communications Objects added (Index 13, Index 14, Index 15)
//                      06/06/2018 - SOME COMENTS ADDERED                                

#include "Arduino.h"
#include <KnxDevice.h>
//#include <SolarBatteryMonitorKNX.h>
#include <Event.h>
#include <Timer.h>
#include <math.h>

////////GLOBAL VARIABLES//////////////////
/////BATERIA//////////////////
double batVOLTAGE = 0;  //VALOR FISICO DE VOLTAJE EN BATERIA
double batCURRENT = 0;  //VALOR FISICO DE CORRIENTE EN BATERIA
double batTEMP = 0;     //VALOR FISICO DE TEMPERATURA EN BATERIA
boolean batVOLT_ALARM = false;  //ALARMA DE VOLTAJE EN BATERIA
boolean batCURR_ALARM = false;  //ALARMA DE CORRIENTE EN BATERIA
boolean batTEMP_ALARM = false;  //ALARMA DE TEMPERATURA EN BATERIA
double batVOLT_TH = 13;   //UMBRAL PARA ALARMA DE VOLTAJE EN BATERIA
double batCURR_TH = 3;    //UMBRAL PARA ALARMA DE CORRIENTE EN BATERIA
double batTEMP_TH = 40;   //UMBRAL PARA ALARMA DE TEMPERATURA EN BATERIA
int batTimeBaseM = 100;   //MULTIPLICADOR DE BASE DE TIEMPO
int batTimeBase = 1;      //BASE DE TIEMPO

boolean batVolt_CycSend = false; //Envio ciclico del valor de tension
boolean batCurr_CycSend = false; //Envio ciclico del valor de corriente
boolean batTemp_CycSend = false; //Envio ciclico del valor de temperatura

////PANEL SOLAR/////////////
double panVOLTAGE = 0; //VALOR FISICO DE VOLTAJE EN EL PANEL
double panCURRENT = 0; //VALOR FISICO DE CORRIENTE EN EL PANEL
double panTEMP = 40;  //VALOR FISICO DE TEMPERATURA EN EL PANEL
double panIRRAD = 1000;   //VALOR FISICO DE IRRADIANCIA EN EL PANEL
int panTimeBaseM = 100;   //MULTIPLICADOR DE BASE DE TIEMPO DE LECTURA DEL PANEL
int panTimeBase = 1;      //BASE DE TIEMPO DE LECTURA DEL PANEL

boolean panVolt_CycSend = false; //Envio ciclico del valor de tension
boolean panCurr_CycSend = false; //Envio ciclico del valor de corriente
boolean panTemp_CycSend = false; //Envio ciclico del valor de temperatura
boolean panIrrad_CycSend = false; //Envio ciclico del valor de irradiancia

/////////////////////////////////////////////////
//Temperature NTC constats
//NTC bateria
//Cambiar el siguiente valor segun lo que dispongamos
const float batRf = 9839;  //Resistencia fija, en ohms
const float batBETA = 3926.5866;//3850;//3435; //Factor Beta del sensor
///////////////////////////////////////////
//TEMPERATURA
const float batT0 = 297.45;//298.15; //Temperatura nominal, en kelvins (25ºC + 273)
const float batR0 = 10536;//10000; //Resistencia nominal den sensor
const float e = 2.71828; //Numero e CONSTANTE
/////////////////////////////////////////////////



////////////TIME SETTINGS///////////////////////
Timer t; //UTILIZADO PARA REVISION DE ALARMAS
unsigned long batpreviousMillis =0; //UTILIZADO PARA LECTURA DE VALORES EN BATERIA
unsigned long panpreviousMillis =0; //UTILIZADO PARA LECTURA DE VALORES EN PANEL

//////////////////////////////////////////////
//////////COMUNICATION OBJECT
KnxComObject KnxDevice::_comObjectsList[] = 
{
 /* Index 0 : */KnxComObject(G_ADDR(0, 0, 201), KNX_DPT_9_001 /*9.021 DPT_Value_Volt*/     , COM_OBJ_SENSOR  /* CRT */),// Voltaje Bateria
 /* Index 1 : */KnxComObject(G_ADDR(0, 0, 202), KNX_DPT_9_001 /*9.021 DPT_Value_Curr*/     , COM_OBJ_SENSOR /* CRT */), // Corriente Bateria
 /* Index 2 : */KnxComObject(G_ADDR(0, 0, 203), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/     , COM_OBJ_SENSOR /* CRT */), // Temperatura Bateria
 /* Index 3 : */KnxComObject(G_ADDR(0, 0, 204), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Volt Alarma Bateria
 /* Index 4 : */KnxComObject(G_ADDR(0, 0, 205), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Curr Alarma Bateria
 /* Index 5:  */KnxComObject(G_ADDR(0, 0, 206), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Temp Alarma Bateria
 /* Index 6 : */KnxComObject(G_ADDR(0, 0, 207), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Volt threshold Bateria
 /* Index 7 : */KnxComObject(G_ADDR(0, 0, 208), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Curr threshold Bateria
 /* Index 8 : */KnxComObject(G_ADDR(0, 0, 209), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Temp threshold Bateria
 /* Index 9: */KnxComObject(G_ADDR(0, 0, 210), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/ , 0x28 /* CRW */),           // Voltage Cyclical Send Active Bateria
 /* Index 10: */KnxComObject(G_ADDR(0, 0, 211), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Current Cyclical Send Active Bateria
 /* Index 11: */KnxComObject(G_ADDR(0, 0, 212), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Temp Cyclical Send Active  Bateria
 /* Index 12: */KnxComObject(G_ADDR(0, 0, 213), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Base Time (0->1s, 1->1min) Bateria
 /* Index 13: */KnxComObject(G_ADDR(0, 0, 214), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */),           // Multiplicador de tiempo Bateria
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /* Index 14 : */KnxComObject(G_ADDR(0, 1, 201), KNX_DPT_9_001 /*9.021 DPT_Value_Volt*/    , COM_OBJ_SENSOR  /* CRT */),// Voltaje Panel
 /* Index 15 : */KnxComObject(G_ADDR(0, 1, 202), KNX_DPT_9_001 /*9.021 DPT_Value_Curr*/    , COM_OBJ_SENSOR /* CRT */), // Corriente Panel
 /* Index 16 : */KnxComObject(G_ADDR(0, 1, 203), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/    , COM_OBJ_SENSOR /* CRT */), // Temperatura Panel
 /* Index 17 : */KnxComObject(G_ADDR(0, 1, 204), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/    , COM_OBJ_SENSOR /* CRT */), // Irradiancia Panel
 /* Index 18 : */KnxComObject(G_ADDR(0, 1, 205), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Voltage Cyclical Send Active panel
 /* Index 19 : */KnxComObject(G_ADDR(0, 1, 206), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Current Cyclical Send Active panel
 /* Index 20 : */KnxComObject(G_ADDR(0, 1, 207), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Temp Cyclical Send Active panel
 /* Index 21 : */KnxComObject(G_ADDR(0, 1, 208), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Irradiancia Cyclical Send Active panel
 /* Index 22 : */KnxComObject(G_ADDR(0, 1, 209), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Base Time (0->1s, 1->1min) panel
 /* Index 23 : */KnxComObject(G_ADDR(0, 1, 210), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */)           // Multiplicador de tiempo panel
}; 

const byte KnxDevice::_comObjectsNb = sizeof(_comObjectsList) / sizeof(KnxComObject); // do no change this code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////CALLBACK FUNCTION////////////////////////////////////
void knxEvents( byte index )
{
 switch ( index )
 {
 case 0: // OBJETO INDEX 0 HA SIDO ACTUALIZADO
    Serial.println("Index 0");
    IndexFun0();
 break;
 
 case 1: // OBJETO INDEX 1 HA SIDO ACTUALIZADO
    Serial.println("Index 1");
    IndexFun1();
 break;
 
 case 2: // OBJETO INDEX 2 HA SIDO ACTUALIZADO
    Serial.println("Index 2");
    IndexFun2();
 break;
 
 case 3: // OBJETO INDEX 3 HA SIDO ACTUALIZADO
    Serial.println("Index 3");
    IndexFun3();
 break;
 
 case 4: // OBJETO INDEX 4 HA SIDO ACTUALIZADO
    Serial.println("Index 4");
    IndexFun4();
 break;
 
 case 5: // OBJETO INDEX 5 HA SIDO ACTUALIZADO
    Serial.println("Index 5");
    IndexFun5();
 break;
 
 case 6: // OBJETO INDEX 6 HA SIDO ACTUALIZADO
    Serial.println("Index 6");
    IndexFun6();
 break;
 
 case 7: // OBJETO INDEX 7 HA SIDO ACTUALIZADO
    Serial.println("Index 7");
    IndexFun7();
 break;
 
 case 8: // OBJETO INDEX 8 HA SIDO ACTUALIZADO
    Serial.println("Index 8");
    IndexFun8();
 break;
 
 case 9:   // OBJETO INDEX 9 HA SIDO ACTUALIZADO
    Serial.println("Index 9");
    IndexFun9();
 break;
 
 case 10:  // OBJETO INDEX 10 HA SIDO ACTUALIZADO
    Serial.println("Index 10");
    IndexFun10();
 break;
 
 case 11:  // OBJETO INDEX 11 HA SIDO ACTUALIZADO
    Serial.println("Index 11");
    IndexFun11();
 break;
 
 case 12:  // OBJETO INDEX 12 HA SIDO ACTUALIZADO
    Serial.println("Index 12");
    IndexFun12();
 break;

 case 13: //  // OBJETO INDEX 13 HA SIDO ACTUALIZADO
    Serial.println("Index 13");
    IndexFun13();
 break;
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
  case 14:  // OBJETO INDEX 14 HA SIDO ACTUALIZADO
    Serial.println("Index 14");
    IndexFun14();
 break;
 
 case 15:  // OBJETO INDEX 15 HA SIDO ACTUALIZADO
    Serial.println("Index 15");
    IndexFun15();
 break;
 
 case 16:  // OBJETO INDEX 16 HA SIDO ACTUALIZADO
    Serial.println("Index 16");
    IndexFun16();
 break;
 
 case 17:  // OBJETO INDEX 17 HA SIDO ACTUALIZADO
    Serial.println("Index 17");
    IndexFun17();
 break;
 
 case 18:  // OBJETO INDEX 18 HA SIDO ACTUALIZADO
    Serial.println("Index 18");
    IndexFun18();
 break;
 
 case 19: // OBJETO INDEX 19 HA SIDO ACTUALIZADO
    Serial.println("Index 19");
    IndexFun19();
 break;
 
 case 20: // OBJETO INDEX 20 HA SIDO ACTUALIZADO
    Serial.println("Index 20");
    IndexFun20();
 break;
 
 case 21: // OBJETO INDEX 21 HA SIDO ACTUALIZADO
    Serial.println("Index 21");
    IndexFun21();
 break;
 
 case 22: /// OBJETO INDEX 22 HA SIDO ACTUALIZADO
    Serial.println("Index 22");
    IndexFun22();
 break;

 case 23: // OBJETO INDEX 23 HA SIDO ACTUALIZADO
    Serial.println("Index 23");
    IndexFun23();
 break;
 
 default:
 break;
 }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////INDEX FUNCTIONS////////////////////
//////BATERIA FUNTIONS/////////////////////

//ENVIA EL ULTIMO VALOR DE VOLTAJE LEIDO
void IndexFun0(){
    Knx.write(0,batVOLTAGE);
    Serial.print("Write to knx VOLTAGE: ");
    Serial.println(batVOLTAGE);
  };

//ENVIA EL ULTIMO VALOR DE CORRIENTE LEIDO
void IndexFun1(){
    Knx.write(1,batCURRENT);
    Serial.print("Write to knx CURRENT: ");
    Serial.println(batCURRENT);
  };
  
//ENVIA EL ULTIMO VALOR DE TEMPERATURA LEIDO
void IndexFun2(){
    Knx.write(2,batTEMP);
    Serial.print("Write to knx TEMP: ");
    Serial.println(batTEMP);
  };

//HACE NADA
void IndexFun3(){
    //Knx.write(3,VOLT_ALARM);
   };
   
//HACE NADA
void IndexFun4(){
    //Knx.write(3,CURR_ALARM);
  };
  
//HACE NADA
void IndexFun5(){
    //Knx.write(3,TEMP_ALARM);
  };

//ACTUALIZA EL VALOR UMBRAL DE VOLTAJE
void IndexFun6(){
    Knx.read(6,batVOLT_TH);
    Serial.print("Read from knx VOLT_TH: ");
    Serial.println(batVOLT_TH);
  };

//ACTUALIZA EL VALOR UMBRAL DE CORRIENTE
void IndexFun7(){
    Knx.read(7,batCURR_TH);
    Serial.print("Read from knx CURR_TH: ");
    Serial.println(batCURR_TH);
  };

//ACTUALIZA EL VALOR UMBRAL DE TEMPERATURA
void IndexFun8(){
    Knx.read(8,batTEMP_TH);
    Serial.print("Read from knx TEMP_TH: ");
    Serial.println(batTEMP_TH);
  };

//ACTIVA ENVIO CICLICO PARA VOLTAJE DE LA BATERIA
void IndexFun9(){
    if (Knx.read(9)){batVolt_CycSend = true; Serial.println("Bat Volt Cicl true");}
    else{batVolt_CycSend = false; Serial.println("Bat Volt Cicl false");};
   };

//ACTIVA ENVIO CICLICO PARA CORRIENTE DE LA BATERIA
void IndexFun10(){
    if (Knx.read(10)){batCurr_CycSend = true; Serial.println("Bat Curr Cicl true");}
    else{batCurr_CycSend = false; Serial.println("Bat Curr Cicl false");};
   };

//ACTIVA ENVIO CICLICO PARA TEMPERATURA DE LA BATERIA
void IndexFun11(){
    if (Knx.read(11)){batTemp_CycSend = true; Serial.println("Bat Temp Cicl true");}
    else{batTemp_CycSend = false; Serial.println("Bat Temp Cicl false");};
   };

//ACTUALIZA LA BASE DE TIEMPO DE LA BATERIA
void IndexFun12(){
    if (Knx.read(12)){batTimeBase = 60; Serial.println("Bat TB = 1min");}
    else{batTimeBase = 1;Serial.println("Bat TB = 1s");};
   };

//ACTUALZA EL MULTIPLICADOR DE TIEMPO DE LA BATERIA
void IndexFun13(){
    Knx.read(13, batTimeBaseM);
    Serial.print("BATTERY TB MULT: ");
    Serial.println(batTimeBaseM);
   };
//////PANEL FUNTIONS/////////////////////////

//ENVIA EL ULTIMO VALOR DE VOLTAJE LEIDA
void IndexFun14(){
    Knx.write(14,panVOLTAGE);
    Serial.print("Write to knx VOLTAGE: ");
    Serial.println(panVOLTAGE);
  };

//ENVIA EL ULTIMO VALOR DE CORRIENTE LEIDA
void IndexFun15(){
    Knx.write(15,panCURRENT);
    Serial.print("Write to knx CURRENT: ");
    Serial.println(panCURRENT);
  };
  
//ENVIA EL ULTIMO VALOR DE TEMPERATURA LEIDA
void IndexFun16(){
    Knx.write(16,panTEMP);
    Serial.print("Write to knx TEMP: ");
    Serial.println(panTEMP);
  };

//ENVIA EL ULTIMO VALOR DE IRRADIANCIA LEIDA
void IndexFun17(){
    Knx.write(17,panIRRAD); //irradiancia
    Serial.print("Write to knx TEMP: ");
    Serial.println(panIRRAD);
  };

//ACTIVA ENVIO CICLICO PARA VOLTAJE DEL PANEL
void IndexFun18(){
    if (Knx.read(18)){panVolt_CycSend = true; Serial.println("Panel Volt Cicl true");}
    else{panVolt_CycSend = false; Serial.println("Panel Volt Cicl false");};
   };

//ACTIVA ENVIO CICLICO PARA CORRIENTE DEL PANEL
void IndexFun19(){
    if (Knx.read(19)){panCurr_CycSend = true; Serial.println("Panel Curr Cicl true");}
    else{panCurr_CycSend = false; Serial.println("Panel Curr Cicl false");};
   };

//ACTIVA ENVIO CICLICO PARA TEMPERATURA DEL PANEL
void IndexFun20(){
    if (Knx.read(20)){panTemp_CycSend = true; Serial.println("Panel Temp Cicl true");}
    else{panTemp_CycSend = false; Serial.println("Panel Temp Cicl false");};
   };

//ACTIVA ENVIO CICLICO PARA IRRADIANCIA DEL PANEL
void IndexFun21(){
    if (Knx.read(21)){panIrrad_CycSend = true; Serial.println("Panel Irrad Cicl true");}
    else{panIrrad_CycSend = false; Serial.println("Panel Irrad Cicl false");};
   };

//ACTUALIZA BASE DE TIEMPO PARA LECTURA DEL PANEL
void IndexFun22(){
    if (Knx.read(22)){panTimeBase = 60; Serial.println("Panel TB = 1min");}
    else{panTimeBase = 1;Serial.println("Panel TB = 1s");};
   };

//ACTUALIZA EL MULTIPLICADOR DE BASE DE TIEMPO
void IndexFun23(){
    Knx.read(23, panTimeBaseM);
    Serial.print("Panel TB MULTI: ");
    Serial.println(panTimeBaseM);
   };
/////////END INDEX FUNCTIONS//////////////////
////SETUP//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  analogReference(EXTERNAL);
  Serial.begin(115200);//
  Serial.print("Program Started..");
  while (Knx.begin(Serial1, P_ADDR(1,1,50)) == KNX_DEVICE_ERROR) {Serial.println("knx init ERROR");} ////CONEXION CON EL BUS KNX
  Serial.println("knx bus started..."); 
  t.every(60000, ReadBatSensors); //LECTURA DE LAS VARIABLES PARA ALARMAS T=60S
}

void loop()
{
  unsigned long currentMillis = millis();
   if (currentMillis - batpreviousMillis >= 1000*batTimeBase*batTimeBaseM){
        batpreviousMillis = currentMillis;
        BatUpdateValue(); //ACTUALIZA LAS VARIABLES EN LA BATERIA
   }
   if (currentMillis - panpreviousMillis >= 1000*panTimeBase*panTimeBaseM){
        panpreviousMillis = currentMillis;
        PanUpdateValue(); //ACTUALIZA LAS VARIABLES EN EL PANEL
   }  
  Knx.task();
  t.update();
}
/////////KNX BUS UPDATE VALUES
void BatUpdateValue(){
    Serial.print("Bateria: ");
    if (batVolt_CycSend){batRead_Volt(); Knx.write(0,batVOLTAGE);Serial.print("  Voltaje: ");Serial.print(batVOLTAGE);} //Send Cyclical Voltage
    if (batCurr_CycSend){batRead_Curr(); Knx.write(1,batCURRENT);Serial.print("  Corriente: ");Serial.print(batCURRENT);} //Send Cyclical Current
    if (batTemp_CycSend){batRead_Temp(); Knx.write(2,batTEMP);Serial.print("  Temp: ");Serial.print(batTEMP);} //Send Cyclical Temperature
    Serial.println();
  }
void PanUpdateValue(){
    Serial.print("Panel: ");
    if (panVolt_CycSend) {panRead_Volt(); Knx.write(14,panVOLTAGE);Serial.print("  Voltaje ");Serial.print(panVOLTAGE);} //Send Cyclical Voltage
    if (panCurr_CycSend) {panRead_Curr(); Knx.write(15,panCURRENT);Serial.print("  Corriente ");Serial.print(panCURRENT);} //Send Cyclical Current
    if (panTemp_CycSend) {panRead_Temp(); Knx.write(16,panTEMP);Serial.print("  Temp ");Serial.print(panTEMP);} //Send Cyclical Temperature
    if (panIrrad_CycSend){panRead_Irrad(); Knx.write(17,panIRRAD);Serial.print("  Irrad ");Serial.print(panIRRAD);} //Send Cyclical Temperature
    Serial.println();
  }

/////////READ SENSOR FOR ALARM////////////////////
void ReadBatSensors(){
    Serial.println("Check Alarms");
    batRead_Volt();
    batRead_Curr();
    batRead_Temp();
  }

////////////////////////////////////////////////////////////////////////
///////////////////LECTURA DE SENSORES BATERIA//////////////////////////
///LECTURA DE VOLTAJE
void batRead_Volt()
{
  double voltageAv; 
//  if (voltage<0){voltage=0;}
  for (int i=0; i<10;i++){
    int sensorValue = analogRead(1);
    double voltage = sensorValue*0.0518-0.2864; //Curva de mejor ajuste Bateria 28_05_2018  
    voltageAv = voltageAv+voltage;
  }
  voltageAv=voltageAv/10;
  if (voltageAv<0){voltageAv=0;}
  batVOLTAGE = voltageAv;
  ////////////////////////////////////////////////////////////////////
  Serial.println();
  if (!batVOLT_ALARM && (batVOLTAGE >= batVOLT_TH)){batVOLT_ALARM=true; } //send Over Voltage Alarm 
  else{if (batVOLT_ALARM && (batVOLTAGE < batVOLT_TH) ){batVOLT_ALARM=false;} }
  Knx.write(3,batVOLT_ALARM); Serial.print("VoltAlarm:");Serial.println(batVOLT_ALARM);
}
/////LECTURA DE CORRIENTE/////
void batRead_Curr()
{
    double CurrentAv;
    for (int i=0; i<10;i++)
    { 
      int sensorValue = analogRead(0);
      double Current = (sensorValue*0.2767-141.3853)*0.9951+0.4097;//  100A 28/05/2018
      CurrentAv = Current+CurrentAv;
    }
    CurrentAv=CurrentAv/10;
    batCURRENT = CurrentAv;
//CHECK ALARMA////
  Serial.println();
  if (!batCURR_ALARM && (batCURRENT >= batCURR_TH)){batCURR_ALARM=true; } //send Over Voltage Alarm 
  else{if (batCURR_ALARM && (batCURRENT < batCURR_TH) ){batCURR_ALARM=false; } }
  Knx.write(4,batCURR_ALARM);Serial.print("CurrAlarm:");Serial.println(batCURR_ALARM);
}
//////LECTURA DE TEMPERATURA////////
void batRead_Temp()
{
  double TempAv;
  for (int i=0; i<10;i++)
  { 
  int sensorValue = analogRead(2);
  //Segunda opcion para convertir a resistencia sin pasar por el voltaje
  double Rt2 = -((sensorValue * batRf) / (sensorValue - 1023));
  //Calculo final de temperatura
  double Temp = 1 / (1 / batT0 + (log(Rt2 / batR0) / log(e)) / batBETA) - 273.15;
  TempAv=TempAv+Temp;
  }
  TempAv=TempAv/10;
  batTEMP = TempAv;
  ///////////////////////////////////////////////////////////////////////
  /////////////TEMPERATURE ALARM/////////////////////////////////////////
  if (!batTEMP_ALARM && (batTEMP >= batTEMP_TH)){batTEMP_ALARM=true; } 
  else{if (batTEMP_ALARM && (batTEMP < batTEMP_TH) ){batTEMP_ALARM=false; } }
  Knx.write(5,batTEMP_ALARM);Serial.print("TempAlarm:");Serial.println(batTEMP_ALARM);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////LECTURA DE SENSORES PANEL SOLAR///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////LECTURA DE COLTAJE
void panRead_Volt()
{   
  double voltageAv;
  for (int i=0; i<10;i++)
  { 
    int sensorValue = analogRead(9);
    double voltage = sensorValue*0.0526-0.2545; //Curva de mejor ajuste Panel 28_05_2018
    voltageAv=voltageAv+voltage;
  }
    voltageAv=voltageAv/10;
    panVOLTAGE = voltageAv;
}
/////////////LECTURA DE CORRIENTE //////////////////////////////////////////
void panRead_Curr()
{   
  double CurrentAv;
  for (int i=0; i<10;i++)
  { 
    int sensorValue = analogRead(8);
    double Current = (sensorValue*0.0508-26.9287)*0.9653+0.8379;// 20A 28/05/2018
    CurrentAv=CurrentAv+Current;
  }
    CurrentAv=CurrentAv/10;
    panCURRENT = CurrentAv;
}
////////LECTURA DE TEMPERATURA////////
void panRead_Temp()
{

}
////////LECTURA DE IRRADIANCIA////////
void panRead_Irrad()
{

}


