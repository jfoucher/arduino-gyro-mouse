/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2021 Jonathan Foucher                                            */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/


#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Mouse.h>

/**********************************************************************************/
/************************* GYRO CALIBRATION CONSTANTS *****************************/
/*                                                                                */
/* Set these so that the data output to the serial monitor                        */
/* (activate SERIAL_DEBUG below) reads 0 for all axes                             */
/**********************************************************************************/
#define ROT_X_COR 0.171
#define ROT_Y_COR -0.05
#define ROT_Z_COR 0.03668

/**********************************************************************************/
/************************** ENABLE SERIAL DEBUGGING? ******************************/
/**********************************************************************************/

// #define SERIAL_DEBUG

/**********************************************************************************/
/****************** DO NOT CHANGE ANYTHING BELOW THIS POINT ***********************/
/**********************************************************************************/

#define ANGLE_RANGE 20.0

Adafruit_MPU6050 mpu;

int vx, vy;

sensors_event_t a, g, temp;

unsigned long el = 0;
unsigned long prev = 0;
unsigned long t = 0;


int posX = 0;
int posY = 0;

double fXg = 0;
double fYg = 0;
double fZg = 0;

double pitch, roll, Xg, Yg, Zg;

const float alpha = 0.5;

void setup() {
  #ifdef SERIAL_DEBUG
  Serial.begin(9600);
  #endif

  Wire.begin();

  Mouse.begin();

  if (!mpu.begin()) {
    #ifdef SERIAL_DEBUG
    Serial.println("No connection");
    #endif
  }


  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);

  delay(100);

  for (int i = 0; i< 10; i++) {
    mpu.getEvent(&a, &g, &temp);
    // Get initial orientation

    Xg = a.acceleration.x;
    Yg = a.acceleration.y;
    Zg = a.acceleration.z;
   //Low Pass Filter
    fXg = Xg * alpha + (fXg * (1.0 - alpha));
    fYg = Yg * alpha + (fYg * (1.0 - alpha));
    fZg = Zg * alpha + (fZg * (1.0 - alpha));
 
    // Roll & Pitch Equations
    // Roll and pitch in radians
    roll  = atan2(-fYg, fZg);
    pitch = atan2(fXg, sqrt(fYg*fYg + fZg*fZg));

    delay(10);
  }

   

  prev = micros();

}



float rotX = 0.0;
float rotY = 0.0;
float rotZ = 0.0;

void loop() {
  t = micros();
  el = t - prev;
  prev = t;
  mpu.getEvent(&a, &g, &temp);

  Xg = a.acceleration.x;
  Yg = a.acceleration.y;
  Zg = a.acceleration.z;
  //Low Pass Filter
  fXg = Xg * alpha + (fXg * (1.0 - alpha));
  fYg = Yg * alpha + (fYg * (1.0 - alpha));
  fZg = Zg * alpha + (fZg * (1.0 - alpha));
  
  // Roll & Pitch Equations
  // Roll and pitch in radians
  roll  = atan2(-fYg, fZg);
  pitch = atan2(fYg, sqrt(fXg*fXg + fZg*fZg));
  
  #ifdef SERIAL_DEBUG
  Serial.print("gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print(" gyro Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" gyro Z: ");
  Serial.println(g.gyro.Z);
  #endif

  
  
  rotX = (rotX * 0.99) + (float)el * (g.gyro.x + ROT_X_COR) * (float)180.0 / (float)3141592.6;
  rotY = (rotY * 0.99) + (float)el * (g.gyro.y + ROT_Y_COR) * (float)180.0 / (float)3141592.6;
  rotZ = (rotZ * 0.99) + (float)el * (g.gyro.z + ROT_Z_COR) * (float)180.0 / (float)3141592.6;
  
  vx = -rotY * sin(pitch) - rotZ * cos(pitch);  
  vy = -rotZ * sin(pitch) + rotY * cos(pitch); 

  posX += (int)vx;
  posY += (int)vy;

  // reset if position if out of bounds

  if (posX > 2000 || posY > 1100 || posX < -2000 || posY < -1100) {
    posX = 0;
    posY = 0;
    rotX = 0;
    rotY = 0;
  }

  Mouse.move(vx, vy);

  delayMicroseconds(8000-el);
}
