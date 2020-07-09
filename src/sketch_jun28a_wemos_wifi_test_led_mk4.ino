//CODE START 
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <L298NX2.h>
#include <secrets.h> // separate header containing my own WiFi credentials.

// DEBUG
//#define DEBUG_ON // comment to disable serial output and loop delay
//#define STEP_ON 
const uint16_t STEP_DELAY = 500;

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
const unsigned short PWMSpeed = 600; // of possible max 1023 due to 10bit PWM on wemos d1 mini

// Motor states
const uint8_t MOTORS_F = 0; // driving forward 
const uint8_t MOTORS_B = 1; // driving backward
const uint8_t MOTORS_TL = 2; // turning left
const uint8_t MOTORS_TR = 3; // turning right
const uint8_t MOTORS_S = 4; // stopped
uint8_t STATE = MOTORS_S; // current state (initalized to MOTOS_S for STOP)

// Intervals
const uint16_t driveIntervalTime = 3000; // default drive time
const uint16_t turnIntervalTime = 225;   // default turn time

// Setting up the http servers
ESP8266WebServer server(81);
IPAddress ip(192, 168, 0, 99); // desired IP address on network
IPAddress gateway(192, 168, 1, 1); // set gateway to match network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match network

// Instantiate motor library object with motor constants
L298NX2 motors(ENA, IN1, IN2, ENB, IN3, IN4);

File fsUploadFile;
bool ledState = LOW;
bool isStopped = true;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// void setup is where we initialize variables, pin modes, start using libraries, etc. 
//The setup function will only run once, after each powerup or reset of the wemos board.
void setup() {
  initDebug();
  initLED();
  initMotors();
  initSPIFFS();
  initHTTP();
  initWifi();
  initOTA();
}

//void loop is where you put all your code. it is a funtion that returns nothing and will repeat over and over again
void loop() {
  StepDebug();
  currentMillis = millis();

  ArduinoOTA.handle();
  server.handleClient();
  motorManager();

  // if (WiFi.status() != WL_CONNECTED) {
  //   mStop();
  // }
}

void motorManager() {
  if (STATE != MOTORS_S) {
    isStopped = false;
  } 
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
      if (!isStopped) {
        mStop();
      }
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

}






void mForward() {
  Serial.println("mF");
  notReset();
  motors.forwardFor(3000, mStop);// forwardFor(3000);
}

void mBackward() {
  Serial.println("mB");
  notReset();
  motors.backwardFor(1000, mStop);
}

void mStop() {
  Serial.println("mS");
  motors.stop();
  // motors.forwardFor(0);
  motors.reset();
  motors.setSpeed(PWMSpeed);
  isStopped = true;
  STATE = MOTORS_S;
}

void mLeft() {
  Serial.println("mL");
  notReset();
  motors.forwardForB(turnIntervalTime);
  motors.backwardForA(turnIntervalTime, mStop);
}

void mRight() {
  Serial.println("mR");
  notReset();
  motors.backwardForB(turnIntervalTime);
  motors.forwardForA(turnIntervalTime, mStop);
}

void notReset() {
  isStopped = false;
}

void setMotorStateF() {STATE = MOTORS_F; };
void setMotorStateB() {STATE = MOTORS_B; };
void setMotorStateL() {STATE = MOTORS_TL; };
void setMotorStateR() {STATE = MOTORS_TR; };
void setMotorStateS() {STATE = MOTORS_S; };

// void handleRoot() {
//   home();
// }

void handleF() {
  setMotorStateF();
  handleFileRead("/");
}
void handleB() {
  setMotorStateB();
  handleFileRead("/");
}
void handleL() {
  setMotorStateL();
  handleFileRead("/");
}
void handleR() {
  setMotorStateR();
  handleFileRead("/");
}
void handleS() {
  setMotorStateS();
  handleFileRead("/");
}

void home() {
  Serial.println("hm");
  server.send(200, "text/html", "<br><br><br><a href=\"/F\">Click here for FORWARD.</a> <br><br><br><a href=\"/S\">Click here for STOP.</a> <br><br><br><a href=\"/B\">Click here for BACKWARD.</a><br><br><br><a href=\"/L\">Click here for LEFT.</a><br><br><br><a href=\"/R\">Click here for RIGHT.</a> <br><br><br>");
  //delay?
}

void StepDebug() {
  #ifdef STEP_ON
  delay(STEP_DELAY);
  #endif
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".txt")) return "text/txt";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz)) {                          // If there's a compressed version available
      path += ".gz";
    }                                                      // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

/*
  _____   _   _   _____   _______ 
 |_   _| | \ | | |_   _| |__   __|
   | |   |  \| |   | |      | |   
   | |   | . ` |   | |      | |   
  _| |_  | |\  |  _| |_     | |   
 |_____| |_| \_| |_____|    |_|   

*/

void initDebug() {
  #ifdef DEBUG_ON
  Serial.begin(115200);
  #endif
}

void initLED() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void initMotors() {
  motors.stop();
  motors.setSpeed(PWMSpeed);
  delay(50);
}

void initSPIFFS() {
  SPIFFS.begin();
  Serial.println("SPIFFS strt");
}

void initHTTP() {
  // server.on("/", handleRoot);
  server.on("/F", handleF);
  server.on("/B", handleB);
  server.on("/L", handleL);
  server.on("/R", handleR);
  server.on("/S", handleS);

  server.on("/upload", HTTP_GET, []() {
    if (!handleFileRead("/upload.html"))                // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  server.on("/upload", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );

  server.onNotFound([]() { // send requested uri else 404
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "404: Not found");
    }
  });
  server.begin(); // Start the server
  Serial.println("Server started");
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP()); // Print the IP address
  Serial.println("/"); 
}

void initWifi() {
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip); // Assigned IP address
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password); // Connect to WiFi network

  bool ledState = LOW;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // displays dots while trying to connect
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 500) { // Blink LED while connecting
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(LED_BUILTIN, ledState);
    }

  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
}

void initOTA() {
  ArduinoOTA.setHostname("ESP8266");
  ArduinoOTA.setPassword("esp8266");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA strt");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA end");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
    ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Fail");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Fail");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Fail");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Fail");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
}

/* 
** Init end ===================================================================
*/

/*
  ______   _   _   _____  
 |  ____| | \ | | |  __ \ 
 | |__    |  \| | | |  | |
 |  __|   | . ` | | |  | |
 | |____  | |\  | | |__| |
 |______| |_| \_| |_____/ 

*/                       
