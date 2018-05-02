#ifndef SOLARBATTERYMONITORKNX_H
#define SOLARBATTERYMONITORKNX_H

#include "Arduino.h"
#include <KnxDevice.h>
#include <SolarBatteryMonitorKNX.h>
#include <Event.h>
#include <Timer.h>

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
    Knx.write(3,VOLT_ALARM);
   };
void IndexFun4(){
    Knx.write(3,CURR_ALARM);
  };
void IndexFun5(){
    Knx.write(3,TEMP_ALARM);
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
    if (Knx.read(12)){
        TimeBase = 60;
      }else{
        TimeBase = 1;
        };
   };
void IndexFun13(){
    if (Knx.read(13)){
        Volt_CycSend = true;
      }else{
        Volt_CycSend = false;
        };
   };
void IndexFun14(){
    if (Knx.read(14)){
        Curr_CycSend = true;
      }else{
        Curr_CycSend = false;
        };
   };
void IndexFun15(){
    if (Knx.read(15)){
        Temp_CycSend = true;
      }else{
        Temp_CycSend = false;
        };
   };
/////////END INDEX FUNCTIONS//////////////////
#endif // SOLARBATTERYMONITORKNX_H
