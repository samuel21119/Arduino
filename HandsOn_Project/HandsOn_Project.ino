//Hands-on Project
//define motor & US sensor pin
#define Motor 10
#define US_Trig 11
#define US_Echo 12
#define US_Power 13
//declare functions & variable
void setup();
void loop();
long MeasureDistance();
void run();
void stop();
long distance;

void setup() {
    Serial.begin(9600);// initialize the serial port
    pinMode(Motor, OUTPUT);//set pin Motor(10) mode to OUTPUT;
    pinMode(US_Power, OUTPUT);//set pin US_Power(13) mode to OUTPUT
    digitalWrite(US_Power, HIGH);//write HIGH state to pin US_Power(13)
    pinMode(US_Trig, OUTPUT);//set pin US_Trig(11) mode
    stop();
}

void loop() {
    distance = MeasureDistance();//measure distance
    Serial.println(distance);//send distance to computer
    if (distance < 20 && distance > 0) {//if the distance is smaller than 20 cm -> run
        run();//spin the motor
    }else if (distance >= 20) {//if the distance is greater than 20 cm or equal to 20 cm -> stop
        stop();//stop the motor
    }
    delay(1000);//wait for a second
}

long MeasureDistance() {
    //declare variables
    long duration, distance;
    //The sensor is triggered by a HIGH pulse of 10 or more microseconds

    //Give a short LOW pulse beforehand to ensure a clean HIGH pulse
    digitalWrite(US_Trig, LOW);
    delayMicroseconds(2);
    //Give a 10-microsecond HIGH pulse
    digitalWrite(US_Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_Trig, LOW);
    pinMode(US_Echo, INPUT);
    //write sound wave travel time (in microseconds) to variable duration
    duration = pulseIn(US_Echo, HIGH);
    //calculate the distance from sound wave travel time (in microseconds)
    distance = duration / 29 / 2;
    return distance;
}

void run() {
    digitalWrite(Motor, HIGH);//write HIGH state to pin Motor(10)
}

void stop() {
    digitalWrite(Motor, LOW);//write LOW state to pin Motor(10)
}
