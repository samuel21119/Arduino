#include <PS2X_lib.h>
#include <Servo.h>
#define CLOCK 9
#define CMD 7
#define ATTEN 8
#define DATA 6
Servo Servo1, Servo2, Servo3, Servo4, Servo5, Servo6;
int ServoDegree[6], PS2_LeftX, PS2_RightY, PS2_RightX;
bool Grab;
PS2X PS2;

void setup() {
    PS2.config_gamepad(CLOCK, CMD, ATTEN, DATA, false, false);
    //clock, command, attention, data, pressure sensitivity enabled, rumble enabled

    Servo1.attach(0);
    Servo2.attach(1);
    Servo3.attach(2);
    Servo4.attach(3);
    Servo5.attach(4);
    Servo6.attach(5);
    Servo6.write(80);

    Grab = false;
}

void loop() {

    PS2.read_gamepad();
    if (PS2.ButtonPressed(PSB_BLUE)) {
        Grab = !Grab;
        if (Grab) {
            Servo6.write(95);
        }else {
            Servo6.write(80);
        }
    }
    
   

    
}
