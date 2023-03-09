//https://wiki.seeedstudio.com/XIAO_BLE/
//https://files.seeedstudio.com/wiki/XIAO-BLE/Seeed-Studio-XIAO-nRF52840-Sense-v1.1.pdf

#include "TORICA_SD.h"
const int cs_SD = 7;
TORICA_SD main_SD(cs_SD);

char SD_Buf[256];

#include <Scheduler.h>
//https://www.arduino.cc/reference/en/libraries/scheduler/

#include "LSM6DS3.h"
#include "Wire.h"

#include <MadgwickAHRS.h>
Madgwick MadgwickFilter;
//https://github.com/arduino-libraries/MadgwickAHRS/blob/master/examples/Visualize101/Visualize101.ino

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

uint32_t time_ms = 0;

void setup() {
  Serial.begin(115200);
  main_SD.begin();

  pinMode (P0_13, OUTPUT);
  digitalWrite(P0_13, LOW);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }
  MadgwickFilter.begin(100); //100Hz
  
  digitalWrite(LEDG, LOW);

  Scheduler.startLoop(loop2);
}

void loop() {
  MadgwickFilter.updateIMU(myIMU.readFloatGyroX(), myIMU.readFloatGyroY(), myIMU.readFloatGyroZ(), myIMU.readFloatAccelX(), myIMU.readFloatAccelY(), myIMU.readFloatAccelZ());

  float roll = MadgwickFilter.getRoll();
  float pitch = MadgwickFilter.getPitch();
  float heading = MadgwickFilter.getYaw();

  sprintf(SD_Buf, "%.2f,%.2f,%.2f\n", roll, pitch, heading );
  main_SD.add_str(SD_Buf);
  //Serial.print(SD_Buf);

  digitalWrite(LEDR, LOW);
  while (millis() - time_ms < 10) {}
  time_ms = millis();
  digitalWrite(LEDR, HIGH);
  //Serial.print("time_ms = ");
  //Serial.print(time_ms);
}

void loop2() {
  main_SD.flash();
}
