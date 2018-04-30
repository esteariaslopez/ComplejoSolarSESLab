//This is a test of the comunication between Arduino UART to KNX based on the tutorial https://thingtype.com/blog/hacking-a-knx-bus-with-an-arduino-like-board/ 
//Programer Esteban Arias
//inicial programation: 30/04/2018

#include <KnxDevice.h>

/////////////Comunication Object
// INDICATOR field : B7 B6 B5 B4 B3 B2 B1 B0
//                   xx xx C  R  W  T  U  I 
//#define KNX_COM_OBJ_C_INDICATOR 0x20 // Communication (C)
//#define KNX_COM_OBJ_R_INDICATOR 0x10 // Read (R)
//#define KNX_COM_OBJ_W_INDICATOR 0x08 // Write (W)
//#define KNX_COM_OBJ_T_INDICATOR 0x04 // Transmit (T)
//#define KNX_COM_OBJ_U_INDICATOR 0x02 // Update (U)
//#define KNX_COM_OBJ_I_INDICATOR 0x01 // Init Read (I)

KnxComObject KnxDevice::_comObjectsList[] = 
{
 /* Index 0 : */KnxComObject(G_ADDR(0, 0, 1), KNX_DPT_1_001 /* 1.001 B1 DPT_Switch */, 0x2E /* CWTU */), // button
 /* Index 1 : */KnxComObject(G_ADDR(0, 0, 1), KNX_DPT_1_001 /* 1.001 B1 DPT_Switch */, 0x2C /* CWT */), // led
};

const byte KnxDevice::_comObjectsNb = sizeof(_comObjectsList) / sizeof(KnxComObject); // do no change this code

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


// Callback function to handle com objects updates
void knxEvents( byte index )
{
 switch ( index )
 {
 case 1: // Object index 1 has been updated
 if ( Knx.read(1) )
 {
 digitalWrite(LED_BUILTIN, HIGH);
 } else
 {
 digitalWrite(LED_BUILTIN, LOW);
 }
 break;

 default:
 break;
 }
}

void setup()
{
 pinMode(LED_BUILTIN, OUTPUT);
 Knx.begin(Serial, P_ADDR(1, 1, 3)); // start a KnxDevice session with physical address 1.1.3 on Serial1 UART
}

void loop()
{
  Knx.task();
}
