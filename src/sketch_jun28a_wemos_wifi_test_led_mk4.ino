//This example will use a static IP to control the switching of a relay. Over LAN using a web browser. 
//A lot of this code have been resued from the example on the ESP8266 Learning Webpage below. 
//http://www.esp8266learning.com/wemos-webserver-example.php

//CODE START 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <L298NX2.h>
#include <myWiFiCredentials.h> // separate header containing my own WiFi credentials. Add this file yourself or comment this line out.

// DEBUG
// #define DEBUG_ON

// WiFi network information
const char* ssid = mySSID;         // WiFi name (wemos will only connect to 2.4GHz network)
const char* password = myPASSWORD; // WiFi password

// Motor constants
const int ENB = D8; // brown
const int IN3 = D6; // orange
const int IN4 = D7; // red
const int ENA = D3; // yellow
const int IN1 = D2; // green
const int IN2 = D1; // blue
const unsigned short PWMSpeed = 600;

// Instantiate motor library object with motor constants
L298NX2 motors(ENA, IN1, IN2, ENB, IN3, IN4);

// Motor states
const uint8_t MOTORS_F = 0; // driving forward 
const uint8_t MOTORS_B = 1; // driving backward
const uint8_t MOTORS_TL = 2; // turning left
const uint8_t MOTORS_TR = 3; // turning right
const uint8_t MOTORS_S = 4; // stopped
uint8_t STATE = 4; // current state

// Intervals
// unsigned long turnStartTime = 0;
// unsigned long driveStartTime = 0;
unsigned long driveIntervalTime = 3000; // default drive time
unsigned long turnIntervalTime = 225;   // default turn time
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// Setting up the http server
// WiFiServer server(81);
ESP8266WebServer server(81);
IPAddress ip(192, 168, 0, 99); // desired IP address on network
IPAddress gateway(192, 168, 1, 1); // set gateway to match network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match network

  bool ledState = LOW;


// void setup is where we initialize variables, pin modes, start using libraries, etc. 
//The setup function will only run once, after each powerup or reset of the wemos board.
void setup() {
  #ifdef DEBUG_ON
  Serial.begin(115200);
  #endif

  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);

  initMotors();

  initWifi();

  initHTTP();
}

//void loop is where you put all your code. it is a funtion that returns nothing and will repeat over and over again
void loop() {
  #ifdef DEBUG_ON
  delay(1000);
  #endif

  currentMillis = millis();
  server.handleClient();
  // if (WiFi.status() != WL_CONNECTED) {
  //   mStop();
  // }
  motorManager();


}

void motorManager() {
  switch(STATE){
    case MOTORS_F :
      Serial.println("mmF");
      mForward();
      break;
    case MOTORS_B :
      Serial.println("mmB");
      mBackward();
      break;
    case MOTORS_S :
      Serial.println("mmS");
      mStop();
      break;
    case MOTORS_TL :
      Serial.println("mmTL");
      mLeft();
      break;
    case MOTORS_TR :
      Serial.println("mmTR");
      mRight();
      break;
  };
  if (!motors.isMovingA()) {
    Serial.println("!mov");
    motors.reset();
    motors.setSpeed(PWMSpeed);
    STATE = MOTORS_S;
  }
}

void initMotors() {
  motors.stop();
  motors.setSpeed(PWMSpeed);
  delay(50);
}

void mForward() {
  motors.forwardFor(3000, reset);// forwardFor(3000);
  Serial.println("mF");
}

void mBackward() {
  motors.backwardFor(1000);
  Serial.println("mB");
}

void mStop() {
  // motors.forwardFor(0);
  // motors.backwardFor(0);
  motors.stop();
  Serial.println("mS");
}

void mLeft() {
  motors.forwardForA(turnIntervalTime);
  motors.backwardForB(turnIntervalTime);
  Serial.println("mL");
}

void mRight() {
  motors.backwardForA(turnIntervalTime);
  motors.forwardForB(turnIntervalTime);
  Serial.println("mR");
}

void reset() {
  motors.reset();
  motors.setSpeed(PWMSpeed);
  Serial.println("rst");
}

void setMotorStateF() {STATE = MOTORS_F; };
void setMotorStateB() {STATE = MOTORS_B; };
void setMotorStateL() {STATE = MOTORS_TL; };
void setMotorStateR() {STATE = MOTORS_TR; };
void setMotorStateS() {STATE = MOTORS_S; };

void handleRoot() {
  home();
}

void handleF() {
  setMotorStateF();
  home();
}
void handleB() {
  setMotorStateB();
  home();
}
void handleL() {
  setMotorStateL();
  home();
}
void handleR() {
  setMotorStateR();
  home();
}
void handleS() {
  setMotorStateS();
  home();
}

void home() {
  Serial.println("hm");
  server.send(200, "text/html", "<br><br><br><a href=\"/F\">Click here for FORWARD.</a> <br><br><br><a href=\"/S\">Click here for STOP.</a> <br><br><br><a href=\"/B\">Click here for BACKWARD.</a><br><br><br><a href=\"/L\">Click here for LEFT.</a><br><br><br><a href=\"/R\">Click here for RIGHT.</a> <br><br><br>");
  //delay?
}

void initWifi() {
    // assigned IP address
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
  //Trying to connect it will display dots
  bool ledState = LOW;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 500) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      // set the LED with the ledState of the variable:
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
}

void initHTTP() {
  server.on("/", handleRoot);
  server.on("/F", handleF);
  server.on("/B", handleB);
  server.on("/L", handleL);
  server.on("/R", handleR);
  server.on("/S", handleS);
    // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 
}