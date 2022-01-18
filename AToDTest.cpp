#include <wiringPi.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>
#include "../RemoteControlTest/PHSensor.h"
#include "../RemoteControlTest/TurbiditySensor.h"
#include "../RemoteControlTest/AToD.h"

using namespace std;

int main (void)
{
  wiringPiSetupGpio(); 
  ShipLog logfile;
  char *i2c_filename = (char*)"/dev/i2c-1";
  const unsigned char A_TO_D_ADDRESS = 0x48;
  pthread_mutex_t i2cMutex = PTHREAD_MUTEX_INITIALIZER;;//mutex for controlling access to i2c bus
    
  AToD atod(i2c_filename, A_TO_D_ADDRESS, &logfile, &i2cMutex);//constructor
  PHSensor phSensor(3,&atod,nullptr);//PHSensor object on A to D channel 3, use default pH calibration
  TurbiditySensor turbiditySensor(2,&atod);//TurbiditySensor object on A to D channel 2
  for (int i=0;i<100;i++) 
  {
	  //using voltage divider of 50k resistor with 10k resistor, so voltage gain is:
	  double dVoltageGain = (50.0 + 10.0) / 10.0;
	  double dVoltage=0.0;
	  bool bGotResult = atod.GetMeasurement(1,1,dVoltageGain,dVoltage);
	  if (bGotResult) {
		  printf("Voltage: %.3f V\n",dVoltage);
	  }
	  else {
		  printf("Error getting voltage for sample #%d.\n",i+1);
	  }
	  //get pH using default parameters
	  double dPH=0.0;
	  bGotResult = phSensor.GetPHSensorPH(dPH);
	  if (!bGotResult) {
		  printf("Error getting pH.\n");
	  }
	  else {
		  printf("pH = %.3f\n",dPH);
	  }
	  //get turbidity voltage
	  double dTurbidityVoltage = 0.0;
	  if (turbiditySensor.GetTurbidity(dTurbidityVoltage)) {
		  printf("Turbidity = %.3f V\n",dTurbidityVoltage);
	  }
	  else {
		  printf("Error getting turbidity.\n");
	  }
  }
  return 0 ;
}
