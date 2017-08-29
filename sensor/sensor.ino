#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 accelgyro;

int16_t ax, ay, az;

bool blinkState = false;

void setup() {
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

}
int i = 0;
  //возвращает угол интовый в градусах
void getAngles(double* fi, double* teta, int16_t ax, int16_t ay, int16_t az){
    double ax_d =(double) ax, ay_d = (double) ay, az_d = (double) az;
    *fi = atan(ay_d/ax_d);
//    if(az_d < 0){
//      az_d = fabs(az_d);
//    }
    *teta = atan( (double) sqrt((ax_d*ax_d + ay_d*ay_d))/az_d);
}

void loop() {
    // read raw accel/gyro measurements from device
  accelgyro.getAcceleration(&ax,&ay,&az);
  double fi = 0;
  double teta = 0;
  getAngles(&fi, &teta, ax, ay, az);
  int16_t fi_i = 0;
  int16_t teta_i = 0;
  fi_i = (int16_t) round(fi*180/3.14);
  teta_i = (int16_t) round(teta*180/3.14);
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(fi_i); Serial.print("\t");
        Serial.print(teta_i); Serial.print("\t\n");
  i++;
}
