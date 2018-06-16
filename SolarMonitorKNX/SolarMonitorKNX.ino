//This is a test of the comunication between Arduino UART to KNX based on the tutorial https://thingtype.com/blog/hacking-a-knx-bus-with-an-arduino-like-board/ 
//Programer Esteban Arias
//inicial programation: 30/04/2018
//                      02/05/2018 - New Communications Objects added (Index 13, Index 14, Index 15)
//                      06/06/2018 - SOME COMENTS ADDERED                                

#include "Arduino.h"
#include <KnxDevice.h>
#include <Event.h>
#include <Timer.h>
#include <math.h>

////////GLOBAL VARIABLES//////////////////
///Definicion de pines/////
const int batVoltPIN = 2;
const int batCurrPIN = 1;
const int batTempPIN = 6;

const int panVoltPIN = 3;
const int panCurrPIN = 4;
const int panTempPIN = 5;
const int panIrradPIN = 0;

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
double panTEMP = 0;  //VALOR FISICO DE TEMPERATURA EN EL PANEL
double panIRRAD = 0;   //VALOR FISICO DE IRRADIANCIA EN EL PANEL
int panTimeBaseM = 1;   //MULTIPLICADOR DE BASE DE TIEMPO DE LECTURA DEL PANEL
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
 /* Index 0 : */KnxComObject(G_ADDR(0, 1, 201), KNX_DPT_9_001 /*9.021 DPT_Value_Volt*/     , COM_OBJ_SENSOR  /* CRT */),// Voltaje Bateria
 /* Index 1 : */KnxComObject(G_ADDR(0, 1, 202), KNX_DPT_9_001 /*9.021 DPT_Value_Curr*/     , COM_OBJ_SENSOR /* CRT */), // Corriente Bateria
 /* Index 2 : */KnxComObject(G_ADDR(0, 1, 203), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/     , COM_OBJ_SENSOR /* CRT */), // Temperatura Bateria
 /* Index 3 : */KnxComObject(G_ADDR(0, 1, 204), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Volt Alarma Bateria
 /* Index 4 : */KnxComObject(G_ADDR(0, 1, 205), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Curr Alarma Bateria
 /* Index 5:  */KnxComObject(G_ADDR(0, 1, 206), KNX_DPT_1_005 /*1.005 DPT_Alarm*/          , COM_OBJ_SENSOR /* CRT */), // Temp Alarma Bateria
 /* Index 6 : */KnxComObject(G_ADDR(0, 1, 207), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Volt threshold Bateria
 /* Index 7 : */KnxComObject(G_ADDR(0, 1, 208), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Curr threshold Bateria
 /* Index 8 : */KnxComObject(G_ADDR(0, 1, 209), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/  , 0x28 /* CRW */),           // Temp threshold Bateria
 /* Index 9: */KnxComObject(G_ADDR(0, 1, 210), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/ , 0x28 /* CRW */),           // Voltage Cyclical Send Active Bateria
 /* Index 10: */KnxComObject(G_ADDR(0, 1, 211), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Current Cyclical Send Active Bateria
 /* Index 11: */KnxComObject(G_ADDR(0, 1, 212), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Temp Cyclical Send Active  Bateria
 /* Index 12: */KnxComObject(G_ADDR(0, 1, 213), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Base Time (0->1s, 1->1min) Bateria
 /* Index 13: */KnxComObject(G_ADDR(0, 1, 214), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */),           // Multiplicador de tiempo Bateria
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /* Index 14 : */KnxComObject(G_ADDR(1, 1, 201), KNX_DPT_9_001 /*9.021 DPT_Value_Volt*/    , COM_OBJ_SENSOR  /* CRT */),// Voltaje Panel
 /* Index 15 : */KnxComObject(G_ADDR(1, 1, 202), KNX_DPT_9_001 /*9.021 DPT_Value_Curr*/    , COM_OBJ_SENSOR /* CRT */), // Corriente Panel
 /* Index 16 : */KnxComObject(G_ADDR(1, 1, 203), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/    , COM_OBJ_SENSOR /* CRT */), // Temperatura Panel
 /* Index 17 : */KnxComObject(G_ADDR(1, 1, 204), KNX_DPT_9_001 /*9.001 DPT_Value_Temp*/    , COM_OBJ_SENSOR /* CRT */), // Irradiancia Panel
 /* Index 18 : */KnxComObject(G_ADDR(1, 1, 205), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Voltage Cyclical Send Active panel
 /* Index 19 : */KnxComObject(G_ADDR(1, 1, 206), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Current Cyclical Send Active panel
 /* Index 20 : */KnxComObject(G_ADDR(1, 1, 207), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Temp Cyclical Send Active panel
 /* Index 21 : */KnxComObject(G_ADDR(1, 1, 208), KNX_DPT_1_001 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Irradiancia Cyclical Send Active panel
 /* Index 22 : */KnxComObject(G_ADDR(1, 1, 209), KNX_DPT_1_006 /*1.006 B1 DPT_BinaryValue*/, 0x28 /* CRW */),           // Base Time (0->1s, 1->1min) panel
 /* Index 23 : */KnxComObject(G_ADDR(1, 1, 210), KNX_DPT_9_001 /*5.004 U8 DPT_Percent_U8*/ , 0x28 /* CRW */)           // Multiplicador de tiempo panel
}; 

const byte KnxDevice::_comObjectsNb = sizeof(_comObjectsList) / sizeof(KnxComObject); // do no change this code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////CALLBACK FUNCTION////////////////////////////////////
void knxEvents( byte index )
{
 switch ( index )
 {
 case 0: // OBJETO INDEX 0 HA SIDO ACTUALIZADO
    IndexFun0();
 break;
 
 case 1: // OBJETO INDEX 1 HA SIDO ACTUALIZADO
    IndexFun1();
 break;
 
 case 2: // OBJETO INDEX 2 HA SIDO ACTUALIZADO
    IndexFun2();
 break;
 
 case 3: // OBJETO INDEX 3 HA SIDO ACTUALIZADO
    IndexFun3();
 break;
 
 case 4: // OBJETO INDEX 4 HA SIDO ACTUALIZADO
    IndexFun4();
 break;
 
 case 5: // OBJETO INDEX 5 HA SIDO ACTUALIZADO
    IndexFun5();
 break;
 
 case 6: // OBJETO INDEX 6 HA SIDO ACTUALIZADO
    IndexFun6();
 break;
 
 case 7: // OBJETO INDEX 7 HA SIDO ACTUALIZADO
    IndexFun7();
 break;
 
 case 8: // OBJETO INDEX 8 HA SIDO ACTUALIZADO
    IndexFun8();
 break;
 
 case 9:   // OBJETO INDEX 9 HA SIDO ACTUALIZADO
    IndexFun9();
 break;
 
 case 10:  // OBJETO INDEX 10 HA SIDO ACTUALIZADO
    IndexFun10();
 break;
 
 case 11:  // OBJETO INDEX 11 HA SIDO ACTUALIZADO
    IndexFun11();
 break;
 
 case 12:  // OBJETO INDEX 12 HA SIDO ACTUALIZADO
    IndexFun12();
 break;

 case 13: //  // OBJETO INDEX 13 HA SIDO ACTUALIZADO
    IndexFun13();
 break;
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
  case 14:  // OBJETO INDEX 14 HA SIDO ACTUALIZADO
    IndexFun14();
 break;
 
 case 15:  // OBJETO INDEX 15 HA SIDO ACTUALIZADO
    IndexFun15();
 break;
 
 case 16:  // OBJETO INDEX 16 HA SIDO ACTUALIZADO
    IndexFun16();
 break;
 
 case 17:  // OBJETO INDEX 17 HA SIDO ACTUALIZADO
    IndexFun17();
 break;
 
 case 18:  // OBJETO INDEX 18 HA SIDO ACTUALIZADO
    IndexFun18();
 break;
 
 case 19: // OBJETO INDEX 19 HA SIDO ACTUALIZADO
    IndexFun19();
 break;
 
 case 20: // OBJETO INDEX 20 HA SIDO ACTUALIZADO
    IndexFun20();
 break;
 
 case 21: // OBJETO INDEX 21 HA SIDO ACTUALIZADO
    IndexFun21();
 break;
 
 case 22: /// OBJETO INDEX 22 HA SIDO ACTUALIZADO
    IndexFun22();
 break;

 case 23: // OBJETO INDEX 23 HA SIDO ACTUALIZADO
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
  };

//ENVIA EL ULTIMO VALOR DE CORRIENTE LEIDO
void IndexFun1(){
    Knx.write(1,batCURRENT);
  };
  
//ENVIA EL ULTIMO VALOR DE TEMPERATURA LEIDO
void IndexFun2(){
    Knx.write(2,batTEMP);
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
  };

//ACTUALIZA EL VALOR UMBRAL DE CORRIENTE
void IndexFun7(){
    Knx.read(7,batCURR_TH);
  };

//ACTUALIZA EL VALOR UMBRAL DE TEMPERATURA
void IndexFun8(){
    Knx.read(8,batTEMP_TH);
  };

//ACTIVA ENVIO CICLICO PARA VOLTAJE DE LA BATERIA
void IndexFun9(){
    if (Knx.read(9)){batVolt_CycSend = true;}
    else{batVolt_CycSend = false;};
   };

//ACTIVA ENVIO CICLICO PARA CORRIENTE DE LA BATERIA
void IndexFun10(){
    if (Knx.read(10)){batCurr_CycSend = true;}
    else{batCurr_CycSend = false;};
   };

//ACTIVA ENVIO CICLICO PARA TEMPERATURA DE LA BATERIA
void IndexFun11(){
    if (Knx.read(11)){batTemp_CycSend = true;}
    else{batTemp_CycSend = false;};
   };

//ACTUALIZA LA BASE DE TIEMPO DE LA BATERIA
void IndexFun12(){
    if (Knx.read(12)){batTimeBase = 60;}
    else{batTimeBase = 1;};
   };

//ACTUALZA EL MULTIPLICADOR DE TIEMPO DE LA BATERIA
void IndexFun13(){
    Knx.read(13, batTimeBaseM);
   };
//////PANEL FUNTIONS/////////////////////////

//ENVIA EL ULTIMO VALOR DE VOLTAJE LEIDA
void IndexFun14(){
    Knx.write(14,panVOLTAGE);
  };

//ENVIA EL ULTIMO VALOR DE CORRIENTE LEIDA
void IndexFun15(){
    Knx.write(15,panCURRENT);
  };
  
//ENVIA EL ULTIMO VALOR DE TEMPERATURA LEIDA
void IndexFun16(){
    Knx.write(16,panTEMP);
  };

//ENVIA EL ULTIMO VALOR DE IRRADIANCIA LEIDA
void IndexFun17(){
    Knx.write(17,panIRRAD); //irradiancia
  };

//ACTIVA ENVIO CICLICO PARA VOLTAJE DEL PANEL
void IndexFun18(){
    if (Knx.read(18)){panVolt_CycSend = true;}
    else{panVolt_CycSend = false;};
   };

//ACTIVA ENVIO CICLICO PARA CORRIENTE DEL PANEL
void IndexFun19(){
    if (Knx.read(19)){panCurr_CycSend = true;}
    else{panCurr_CycSend = false;};
   };

//ACTIVA ENVIO CICLICO PARA TEMPERATURA DEL PANEL
void IndexFun20(){
    if (Knx.read(20)){panTemp_CycSend = true;}
    else{panTemp_CycSend = false;};
   };

//ACTIVA ENVIO CICLICO PARA IRRADIANCIA DEL PANEL
void IndexFun21(){
    if (Knx.read(21)){panIrrad_CycSend = true;}
    else{panIrrad_CycSend = false;};
   };

//ACTUALIZA BASE DE TIEMPO PARA LECTURA DEL PANEL
void IndexFun22(){
    if (Knx.read(22)){panTimeBase = 60;}
    else{panTimeBase = 1;};
   };

//ACTUALIZA EL MULTIPLICADOR DE BASE DE TIEMPO
void IndexFun23(){
    Knx.read(23, panTimeBaseM);
   };
/////////END INDEX FUNCTIONS//////////////////
////SETUP//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  analogReference(EXTERNAL); //3.3V
  Serial.begin(115200);//
  while (Knx.begin(Serial1, P_ADDR(1,1,50)) == KNX_DEVICE_ERROR) {} ////CONEXION CON EL BUS KNX
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  //t.every(60000, ReadBatSensors); //LECTURA DE LAS VARIABLES PARA ALARMAS T=60S
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
    if (batVolt_CycSend){batRead_Volt(); Knx.write(0,batVOLTAGE);} //Send Cyclical Voltage
    if (batCurr_CycSend){batRead_Curr(); Knx.write(1,batCURRENT);} //Send Cyclical Current
    if (batTemp_CycSend){batRead_Temp(); Knx.write(2,batTEMP);} //Send Cyclical Temperature
  }
void PanUpdateValue(){
    if (panVolt_CycSend) {panRead_Volt(); Knx.write(14,panVOLTAGE);} //Send Cyclical Voltage
    if (panCurr_CycSend) {panRead_Curr(); Knx.write(15,panCURRENT);} //Send Cyclical Current
    if (panTemp_CycSend) {panRead_Temp(); Knx.write(16,panTEMP);} //Send Cyclical Temperature
    if (panIrrad_CycSend){panRead_Irrad(); Knx.write(17,panIRRAD);} //Send Cyclical Temperature
  }

/////////READ SENSOR FOR ALARM////////////////////
void ReadBatSensors(){
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
  int sensorValue;
  for (int i=0; i<10;i++){
    sensorValue = analogRead(batVoltPIN);
    double voltage = sensorValue*0.0341+0.0716; //Curva de mejor ajuste Bateria 28_05_2018  
    voltageAv = voltageAv+voltage;
  }
  voltageAv=voltageAv/10;
  if (voltageAv<0){voltageAv=0;}
  batVOLTAGE = voltageAv;
  //Serial.println(batVOLTAGE);
  ////////////////////////////////////////////////////////////////////
//  if (!batVOLT_ALARM && (batVOLTAGE >= batVOLT_TH)){batVOLT_ALARM=true; } //send Over Voltage Alarm 
//  else{if (batVOLT_ALARM && (batVOLTAGE < batVOLT_TH) ){batVOLT_ALARM=false;} }
//  Knx.write(3,batVOLT_ALARM);
}
/////LECTURA DE CORRIENTE/////
void batRead_Curr()
{
    double CurrentAv;
    int sensorValue;
    for (int i=0; i<10;i++)
    { 
      sensorValue = analogRead(batCurrPIN);
      double Current = (sensorValue*0.2917-148.9374);//  100A 28/05/2018
      CurrentAv = Current+CurrentAv;
    }
    CurrentAv=CurrentAv/10;
    batCURRENT = CurrentAv;
    //Serial.println(batCURRENT);
//CHECK ALARMA////
  if (!batCURR_ALARM && (batCURRENT >= batCURR_TH)){batCURR_ALARM=true; } //send Over Voltage Alarm 
  else{if (batCURR_ALARM && (batCURRENT < batCURR_TH) ){batCURR_ALARM=false; } }
  Knx.write(4,batCURR_ALARM);
}
//////LECTURA DE TEMPERATURA////////
void batRead_Temp()
{
  double TempAv;
  int sensorValue;
  for (int i=0; i<10;i++)
  { 
  sensorValue = analogRead(batTempPIN);
  //Segunda opcion para convertir a resistencia sin pasar por el voltaje
  double Rt2 = -((sensorValue * batRf) / (sensorValue - 1023));
  //Calculo final de temperatura
  double Temp = 1 / (1 / batT0 + (log(Rt2 / batR0) / log(e)) / batBETA) - 273.15;
  TempAv=TempAv+Temp;
  }
  TempAv=TempAv/10;
  batTEMP = TempAv;
  Serial.println(sensorValue);
  ///////////////////////////////////////////////////////////////////////
  /////////////TEMPERATURE ALARM/////////////////////////////////////////
  if (!batTEMP_ALARM && (batTEMP >= batTEMP_TH)){batTEMP_ALARM=true; } 
  else{if (batTEMP_ALARM && (batTEMP < batTEMP_TH) ){batTEMP_ALARM=false; } }
  Knx.write(5,batTEMP_ALARM);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////LECTURA DE SENSORES PANEL SOLAR///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////LECTURA DE COLTAJE
void panRead_Volt()
{   
  double voltageAv;
  int sensorValue;
  for (int i=0; i<10;i++)
  { 
    sensorValue = analogRead(panVoltPIN);
    double voltage = sensorValue*0.0348+0.0959; //Curva de mejor ajuste Panel 28_05_2018
    voltageAv=voltageAv+voltage;
  }
    voltageAv=voltageAv/10;
    panVOLTAGE = voltageAv;
    //Serial.println(panVOLTAGE);
}
/////////////LECTURA DE CORRIENTE //////////////////////////////////////////
void panRead_Curr()
{   
  double CurrentAv;
  int sensorValue;
  for (int i=0; i<10;i++)
  { 
    sensorValue = analogRead(panCurrPIN);
    double Current = (sensorValue*0.0508-25.3813);// 20A 28/05/2018
    CurrentAv=CurrentAv+Current;
  }
    CurrentAv=CurrentAv/10;
    panCURRENT = CurrentAv;
    Serial.println(panCURRENT);
}
////////LECTURA DE TEMPERATURA////////
void panRead_Temp()
{
  int sensorValue;
  double TempAv;
  for (int i=0; i<10;i++)
  { 
    sensorValue = analogRead(panTempPIN);
    double temp = sensorValue*0.06071443-11.91; //pt1000
    TempAv = TempAv + temp;
  }
  TempAv = TempAv/10;
  panTEMP = TempAv;
  Serial.println(sensorValue);
}
////////LECTURA DE IRRADIANCIA////////
void panRead_Irrad()
{
    int sensorValue = analogRead(panIrradPIN);
    double voltage = sensorValue*0.0915+3.5145; //Curva de mejor ajuste IRRADIANCIA    
    double Irrad = voltage*12.69;

    panIRRAD = Irrad;
    //Serial.println(voltage);
}


