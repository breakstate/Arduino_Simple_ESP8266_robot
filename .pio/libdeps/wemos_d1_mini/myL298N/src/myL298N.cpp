/*
  myL298N.cpp - Library for operation of L298N motor driver in wheeled robot (WIP name)
  Author:   Brenton Moodley
  Link: [add repo]
*/

#include "myL298N.h"

myL298N::myL298N(uint8_t EN_A, uint8_t IN_1, uint8_t IN_2, uint8_t EN_B, uint8_t IN_3, uint8_t IN_4) {
    _EN_A = EN_A;
    _EN_B = EN_B;
    _IN_1 = IN_1;
    _IN_2 = IN_2;
    _IN_3 = IN_3;
    _IN_4 = IN_4;
}

