/*
  Alterations by Brenton Moodley:
  1) Enable pins changed from using uint8_t (8bit) to uint16_t in order to support 10bit PWM as found in ESP8266
  2) Enum explicitly made to be of type uint8_t to avoid compile time errors
*/

#ifndef L298N_h
#define L298N_h

#include "Arduino.h"

typedef void (*CallBackFunction)();

class L298N
{
public:
   typedef enum : uint8_t // see comment at top #2
   {
      FORWARD = 0,
      BACKWARD = 1,
      STOP = 2
   } Direction;

   L298N(uint16_t pinEnable, uint8_t pinIN1, uint8_t pinIN2); // see comment at top #1
   L298N(uint8_t pinIN1, uint8_t pinIN2);
   void setSpeed(unsigned short pwmVal);
   unsigned short getSpeed();
   void forward();
   void forwardFor(unsigned long delay, CallBackFunction callback);
   void forwardFor(unsigned long delay);
   void backward();
   void backwardFor(unsigned long delay, CallBackFunction callback);
   void backwardFor(unsigned long delay);
   void run(uint8_t direction);
   void runFor(unsigned long delay, L298N::Direction direction);
   void runFor(unsigned long delay, L298N::Direction direction, CallBackFunction callback);
   void stop();
   void reset();
   boolean isMoving();
   Direction getDirection();

private:
   uint16_t _pinEnable; // see comment at top #1
   byte _pinIN1;
   byte _pinIN2;
   uint16_t _pwmVal; // see comment at top #1
   unsigned long _lastMs;
   boolean _canMove;
   boolean _isMoving;
   L298N::Direction _direction;
   static void fakeCallback();
};

#endif