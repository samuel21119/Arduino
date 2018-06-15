#include <IRremote.h>
#include <Servo.h>

//Define motor pin
#define motorR1 8
#define motorR2 9
#define motorL1 10
#define motorL2 11
#define motorRRPM 5
#define motorLRPM 6
//Define IR receiver pin
#define IRpin 2
//Define Servo pin
#define Servo_Pin 3
//Define US pin
#define US_Trig  13
#define US_Echo  12
//Define IR controller's signal
#define UP 0x77E1D081
#define DOWN 0x77E1B081
#define RIGHT 0x77E1E081
#define LEFT 0x77E11081
#define CENTER 0x77E1BA81
#define MENU 0x77E14081
#define PAUSE 0x77E17A81
/*
    up: 77E1D081
    down: 77E1B081
    right: 77E1E081
    left: 77E11081
    center: 77E1BA81
    menu: 77E14081
    pause: 77E17A81
*/

//Define variable & function
Servo EyeServo;//Servo
IRrecv IRreceiver(IRpin);//IR receiver
decode_results IRresults;//IR receiver result
void forward();
void turnR();
void turnL();
void backward();
void stop();
void ChangeSpeed(int);
int GetDistance(int);
void US_Car();
char ChooseDirection();
int StopUS_Car();
int speed, running, temp;

void loop() {
    if (IRreceiver.decode(&IRresults)) {//Detect if receive IR signal
        resume:
        Serial.println(IRresults.value, HEX);//Print receive signal
        switch(IRresults.value) {   //According to the signal, do the right move
            case UP:                //Forward
                if (running == 1) { //If it is already running forward, then stop.
                    stop();
                    running = 0;
                    break;
                }
                forward();          //Go forward
                EyeServo.write(100);//Set servo direction
                running = 1;
                break;
            case RIGHT:             //Right
                if (running == 2) { //If it is already turning right, then stop.
                    EyeServo.write(100);
                    stop();
                    running = 0;
                    break;
                }
                turnR();            //Turn right
                EyeServo.write(60); //Set servo direction
                running = 2;
                break;
            case LEFT:              //Left
                if (running == 3) { //If it is already turning left, then stop.
                    stop();
                    running = 0;
                    EyeServo.write(100);
                    break;
                }
                turnL();            //Turn left
                EyeServo.write(140);//Set servo direction
                running = 3;
                break;
            case DOWN:              //Backward
                if (running == 4) { //If it is already running backward, then stop.
                    stop();
                    running = 0;
                    break;
                }
                backward();         //Go backward
                EyeServo.write(100);//Set servo direction
                running = 4;
                break;
            case MENU:              //-speed
                if (speed > 20) {
                    ChangeSpeed(speed - 20);
                    speed -= 20;
                }
                break;
            case PAUSE:             //+speed
                if (speed < 240) {
                    ChangeSpeed(speed + 20);
                    speed += 20;
                }
                break;
            case CENTER:            //US Car
                if (running == 5) { //If it is already running US_Car(), then stop.
                    EyeServo.write(100);
                    stop();
                    running = 0;
                    break;
                }
                IRreceiver.resume();//Receive the next IR signal
                running = 5;        //Initialize specific variable
                temp = 1;
                while (1) {
                    US_Car();       //Run US_Car() function
                    if (temp == 0) {
                        goto resume;//Depend if need to stop US_Car() function
                    }
                }
                EyeServo.write(100);//Set servo direction
                break;
        }
        IRreceiver.resume();        //Receive the next IR signal
    }
}

void setup() {
    //Let arduino can send standard output to computer
    Serial.begin(9600);
    //Set pin mode
    pinMode(motorR1, OUTPUT);
    pinMode(motorR2, OUTPUT);
    pinMode(motorL1, OUTPUT);
    pinMode(motorL2, OUTPUT);
    pinMode(motorRRPM, OUTPUT);
    pinMode(motorLRPM, OUTPUT);
    //Set motor speed
    analogWrite(motorRRPM, 180);
    analogWrite(motorLRPM, 180);
    speed = 180;
    //set run mode
    /*
    0: Stop
    1: Forward
    2: Turn right
    3: Backward
    4: Turn left
    5: US_Car
    */
    running = 0;
    //Set IR sensor to active
    IRreceiver.enableIRIn();
    //Set servo motor
    EyeServo.attach(Servo_Pin);
    EyeServo.write(100);
}

void forward() {
    //Control motor pin to high / low voltage
    digitalWrite(motorR1, HIGH);
    digitalWrite(motorR2, LOW);
    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, LOW);
}

void turnR() {
    //Control motor pin to high / low voltage
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, HIGH);
    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, LOW);
}

void turnL() {
    //Control motor pin to high / low voltage
    digitalWrite(motorR1, HIGH);
    digitalWrite(motorR2, LOW);
    digitalWrite(motorL1, LOW);
    digitalWrite(motorL2, HIGH);
}

void backward() {
    //Control motor pin to high / low voltage
    digitalWrite(motorR1, LOW);
    digitalWrite(motorR2, HIGH);
    digitalWrite(motorL1, LOW);
    digitalWrite(motorL2, HIGH);
}

void stop() {
    //Control motor pin to high / low voltage
    digitalWrite(motorR1, HIGH);
    digitalWrite(motorR2, HIGH);
    digitalWrite(motorL1, HIGH);
    digitalWrite(motorL2, HIGH);
}

void ChangeSpeed(int input) {
    //Write speed to right & left motor
    analogWrite(motorRRPM, input);
    analogWrite(motorLRPM, input);
}

int GetDistance(int input) {

    EyeServo.write(input);      //Set servo direction
    delay(250);

    digitalWrite(US_Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(US_Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_Trig, LOW);

    double distance = pulseIn(US_Echo, HIGH);   //Get delay time in microseconds
    distance = distance / 58;   //Calculate distance

    return distance;
}

char ChooseDirection() {
    int Car_Direction = 0;
    int F_Distance = 0;
    int F_DistanceLEFT = 0;
    int F_DistanceMID = 0;
    int F_DistanceRIGHT = 0;
    int R_Distance = 0;
    int L_Distance = 0;
    if (temp == 1) {
        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceLEFT = GetDistance(120);
        if (F_DistanceLEFT < 10) {
            goto depend;
        }

        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceMID = GetDistance(100);
        if (F_DistanceMID < 10) {
            goto depend;
        }

        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceRIGHT = GetDistance(80);
        if (F_DistanceRIGHT < 10) {
            goto depend;
        }
    } else {
        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceRIGHT = GetDistance(80);
        if (F_DistanceRIGHT < 10) {
            goto depend;
        }

        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceMID = GetDistance(100);
        if (F_DistanceMID < 10) {
            goto depend;
        }

        if (StopUS_Car()) {
            return 'F';
        }
        F_DistanceLEFT = GetDistance(120);
        if (F_DistanceLEFT < 10) {
            goto depend;
        }
    }

depend:
    if (F_DistanceLEFT < 10) {
        stop();
        delay(100);
        backward();
        delay(200);
        return 'R';
    }
    if (F_DistanceRIGHT < 10) {
        stop();
        delay(100);
        backward();
        delay(200);
        return 'L';
    }
    if (F_DistanceMID < 10) {
        stop();
        delay(100);
        backward();
        delay(200);
    }
    if (F_DistanceLEFT < 25 || F_DistanceMID < 25 || F_DistanceRIGHT < 25) {
        stop();
        delay(100);

        if (StopUS_Car()) {
            return 'F';
        }
        R_Distance = GetDistance(10);
        if (StopUS_Car()) {
            return 'F';
        }
        L_Distance = GetDistance(190);
        temp = 2;

        if (R_Distance < 10 && L_Distance < 10) {
            return 'B';
        }else if (R_Distance < L_Distance) {
            return 'L';
        }else {
            return 'R';
        }
    }
    return 'F';
}

void US_Car() {
    forward();
    switch (ChooseDirection()) {
        case 'B':
            backward();
            delay(100);
            turnL();
            delay(240);
            break;
        case 'L':
            turnL();
            delay(240 * 2);
            break;
        case 'R':
            turnR();
            delay(240 * 2);
            break;
        case 'F':
        default:
            break;
    }
    if (temp == 1) {
        temp++;
    }else if (temp == 2){
        temp = 1;
    }
}

int StopUS_Car() {
    if (IRreceiver.decode(&IRresults)) {
        Serial.println(IRresults.value, HEX);
        switch (IRresults.value) {
            case UP:
            case DOWN:
            case RIGHT:
            case LEFT:
                temp = 0;
                return 1;

            case MENU:
                if (speed > 20) {
                    ChangeSpeed(speed - 20);
                    speed -= 20;
                    Serial.println(speed);
                }
                IRreceiver.resume();
                return 0;

            case PAUSE:
                if (speed < 240) {
                    ChangeSpeed(speed + 20);
                    speed += 20;
                    Serial.println(speed);
                }
                IRreceiver.resume();
                return 0;

            case CENTER:
                temp = 0;
                return 1;

            default:
                IRreceiver.resume();
                return 0;
        }
    }
    return 0;
}
