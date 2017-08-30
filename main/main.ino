/*
  Arduino Yún Bridge example

  This example for the YunShield/Yún shows how 
  to use the Bridge library to access the digital and
  analog pins on the board through REST calls.
  It demonstrates how you can create your own API when
  using REST style calls through the browser.

  Possible commands created in this shetch:

  "/arduino/digital/13"     -> digitalRead(13)
  "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
  "/arduino/analog/2/123"   -> analogWrite(2, 123)
  "/arduino/analog/2"       -> analogRead(2)
  "/arduino/mode/13/input"  -> pinMode(13, INPUT)
  "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

  This example code is part of the public domain

  http://www.arduino.cc/en/Tutorial/Bridge

*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define DIGITAL_PINS_NUMBER_YUN 14
#define ANALOG_PINS_NUMBER_YUN 6
#define MAX_PARAMETERS_URL 3
#define MAX_LEN_COMMAND_BUF 20


MPU6050 accelgyro;
int16_t ax, ay, az;
double fi, teta;
int16_t intFi, intTeta;
// Listen to the default port 5555, the Yún webserver will forward there all the HTTP requests you send
BridgeServer server;

void setup() {
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  accelgyro.initialize();
}

  //возвращает угол интовый в градусах
void getAngles(double* fi, double* teta, int16_t ax, int16_t ay, int16_t az){
    double ax_d =(double) ax, ay_d = (double) ay, az_d = (double) az;
    *fi = atan(ay_d/ax_d);
//    if(az_d < 0){
//      az_d = fabs(az_d);
//    }
    *teta = atan( (double) sqrt((ax_d*ax_d + ay_d*ay_d))/az_d);
}
 
void ToInt(double fi, double teta, int* fi_i, int* teta_i){
  *fi_i = (int16_t) round(fi*180/3.14);
  *teta_i = (int16_t) round(teta*180/3.14);
}

void loop() {
  

  accelgyro.getAcceleration(&ax,&ay,&az);
  getAngles(&fi, &teta, ax, ay, az);
  ToInt(fi,teta,&intFi,&intTeta);
  
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
  // read the command
  uint16_t parameter[MAX_PARAMETERS_URL];
  String command;//[MAX_LEN_COMMAND_BUF];
  String str = client.readStringUntil('\r');
  uint16_t n_parameter = myparseURL(str, &command, parameter, MAX_PARAMETERS_URL, '/');
  if ( command == "status" ){
    gliderstatus(client);
  }else if ( command == "dive" ){
    dive(client);
  }else if ( command == "emersion" ){
    client.println(F("Выполняю всплытие"));
    digitalWrite(LED_BUILTIN, LOW);
  }else if ( command == "bubble" ){
    if (n_parameter == 1){
      client.print(F("Выполняю регулировку заполнености шприцов, целевое положение: "));
      client.print(parameter[0]);
      client.println("%");
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      client.print(F("error: command \""));
      client.print(command);
      client.print(F("\" must have one parameter; n_parameter = "));
      client.println(n_parameter);
    }
  }else if ( command == "batpos" ){ //batpos
    if (n_parameter == 1){
      client.print(F("Выполняю регулировку положения батарей, целевое положение: "));
      client.print(parameter[0]);
      client.println("%");
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      client.print(F("error: command \""));
      client.print(command);
      client.print(F("\" must have one parameter; n_parameter = "));
      client.println(n_parameter);
    }
  } else if ( command == "finpos" ){ //finpos
    if (n_parameter == 1){
      client.print(F("Выполняю поворот плавника, целевое положение: "));
      client.print(parameter[0]);
      client.println("%");
    } else {
      client.print(F("error: command \""));
      client.print(command);
      client.print(F("\" must have one parameter; n_parameter = "));
      client.println(n_parameter);
    }
  } else if ( command == "roll" ){ //roll
    if (n_parameter == 1){
      client.print(F("Выполняю установку крена, целевое угол: "));
      client.println(parameter[0]);
    } else {
      client.print(F("error: command \""));
      client.print(command);
      client.print(F("\" must have one parameter; n_parameter = "));
      client.println(n_parameter);
    }
  } else if ( command == "course" ){ //course
    if (n_parameter == 1){
      client.print(F("Выполняю смену курса, целевое угол: "));
      client.println(parameter[0]);
    } else {
      client.print(F("error: command \""));
      client.print(command);
      client.print(F("\" must have one parameter; n_parameter = "));
      client.println(n_parameter);
    }
  } else {
    client.print(F("error: command \""));
    client.print(command);
    client.println(F("\" does not exist"));
  }
}

//возвращает количество считанных параметров
uint16_t myparseURL(String str, String *command, uint16_t *parameter, uint16_t max_parameter, char delimiter){
  uint16_t ilast=0;
  uint16_t n_parameter=0;
  String buf;
  uint16_t i=0;
  while(str[i] != '\0') {
    if( str[i] == delimiter ) {
      if (!ilast){
        *command = str.substring(ilast,i);
        ilast=i+1;
      } else if(n_parameter<max_parameter) {
        parameter[n_parameter] = str.substring(ilast,i).toInt();
        ilast=i+1;
        //parameter[n_parameter] = buf.toInt();
        n_parameter++;
      } else {
        return -1;
      }
    }
    i++;
  }
  i++;
  if (!ilast){
      *command = str.substring(ilast,i);
      ilast=i+1;
    } else if(n_parameter<max_parameter) {
      buf = str.substring(ilast,i);
      ilast=i+1;
      parameter[n_parameter] = buf.toInt();
      n_parameter++;
    } else {
      return -1;
    }
  return n_parameter;
}

void gliderstatus(BridgeClient client) {
  int D[DIGITAL_PINS_NUMBER_YUN];
  int A[ANALOG_PINS_NUMBER_YUN];
  for (int i=0; i < DIGITAL_PINS_NUMBER_YUN; i++) {
    D[i] = digitalRead(i);
    client.print(D[i]);
    client.print(F(" "));
    
  }
  for (int i=0; i < ANALOG_PINS_NUMBER_YUN; i++) {
    A[i] = analogRead(i);
    client.print(A[i]);
    client.print(F(" "));
  }
  client.println();



  client.println(intFi);
  client.println(intTeta);
}

void dive(BridgeClient client) {
  client.println(F("Выполняю погружение"));
}

