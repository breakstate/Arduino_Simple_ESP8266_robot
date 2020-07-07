#ifndef myL298N_h
#define myL298N_h

#include "Arduino.h"

class myL298N
{
public:
    myL298N(uint8_t EN_A, uint8_t IN_1, uint8_t IN_2, uint8_t EN_B, uint8_t IN_3, uint8_t IN_4);
    myL298N(uint8_t IN_1, uint8_t IN_2, uint8_t IN_3, uint8_t IN_4);
    void set10BitPWM(boolean is10bitPWM);
    void set8BitPWM(boolean is8bitPWM);
    void setPWMpercentage(uint8_t percentage); // note this makes use of setBit
    void forwardA();
    void forwardB();
    void backwardA();
    void backwardB();
    void left();
    void right();
    void stopAll();
    void stopLeft();
    void stopRight();


private:
    void _activate(uint8_t IN_1, uint8_t IN_2);
    boolean _is10bitPWM;
    boolean _is8bitPWM;
    uint8_t _EN_A;
    uint8_t _IN_1;    
    uint8_t _IN_2;
    uint8_t _EN_B;
    uint8_t _IN_3;
    uint8_t _IN_4;
};

#endif