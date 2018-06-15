#include "DHT.h"
#include <Wire.h>
#include <DS1302.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <pitches.h>
#define BUZZER 7
#define LED    13
#define IRPIN  12
#define DHTPIN 2
#define DHTTYPE DHT11
#define UP 0x77E1D081
#define DOWN 0x77E1B081
#define RIGHT 0x77E1E081
#define LEFT 0x77E11081
#define CENTER 0x77E1BA81
#define MENU 0x77E14081
#define PAUSE 0x77E17A81
#define FLASH 44
DS1302 TIME(10, 9, 8);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
DHT dht(DHTPIN, DHTTYPE);


int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0,NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0,NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4,NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18,18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};
int song, stop, flash;


IRrecv IR(IRPIN);
decode_results signal;

float Temp, Humidity;
int ButtonState, mode, mode2, change, time_hour, time_minute;
int alarm_hour, alarm_minute;
bool alarm_on, temp;

void DisplayMode1();
void DisplayMode2();
int ChangeMode();
void Mode3();
void Mode4();
void BUZZ();
void setup() {
    Serial.begin(9600);
    pinMode(BUZZER, OUTPUT);
    pinMode(LED, OUTPUT);
    TIME.halt(false);
    TIME.writeProtect(true);
    IR.enableIRIn();
    dht.begin();
    lcd.begin(16, 2);
    mode = mode2 = 1;
    alarm_on = alarm_hour = alarm_minute = 0;
    temp = true;
    song = stop = 0;
    flash = 0;
}
void loop() {
    time_hour = (TIME.getTimeStr()[0] - '0') * 10 + TIME.getTimeStr()[1] - '0';
    time_minute = (TIME.getTimeStr()[3] - '0') * 10 + TIME.getTimeStr()[4] - '0';
    if (mode == 1) {
        if (mode2 != mode) {
            lcd.clear();
            mode2 = mode;
        }
        lcd.backlight();
        DisplayMode1();
        for (int i = 0; i < 40; i++, delay(50)) {
            if (ChangeMode()) {
                return;
            }
        }
    }else if(mode == 2) {
        lcd.backlight();
        if (mode2 != mode) {
            lcd.clear();
            mode2 = mode;
        }
        DisplayMode2();
    }else if(mode == 3) {
        lcd.backlight();
        if (mode2 != mode) {
            lcd.clear();
            mode2 = mode;
            flash = 0;
        }
        Mode3();
        flash++;
        if (flash == FLASH) {
            flash = 0;
        }
    }else if(mode == 4) {
        lcd.backlight();
        if (mode2 != mode) {
            lcd.clear();
            mode2 = mode;
            flash = 0;
        }
        Mode4();
        flash++;
        if (flash == FLASH) {
            flash = 0;
        }
    }else if (mode == 0){
        mode2 = mode;
        lcd.noBacklight();
    }else if (mode == 5){
        lcd.clear();
        lcd.backlight();
        sing(1);
        sing(1);
        sing(2);
        stop = 0;
    }
    if(ChangeMode()) {
        return;
    }
    if (time_hour == alarm_hour && time_minute == alarm_minute && temp && alarm_on && mode < 3) {
        mode2 = mode;
        mode = 5;
        temp = false;
    }
    if (!(time_hour == alarm_hour && time_minute == alarm_minute)){
        temp = true;
    }
}











void DisplayMode1() {
    if (isnan(Humidity) || isnan(Temp)) {
        return;
    }
    Humidity = dht.readHumidity();
    Temp = dht.readTemperature();
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.setCursor(10, 0);
    lcd.print(Humidity);
    lcd.setCursor(14, 0);
    lcd.print(" %");
    lcd.setCursor(0, 1);
    lcd.print("Temp    :");
    lcd.setCursor(10, 1);
    lcd.print(Temp);
    lcd.setCursor(14, 1);
    lcd.print("*C");
}
void DisplayMode2() {
    lcd.setCursor(3, 0);
    for (int i = 0; i < 4; i++) {
        lcd.print(TIME.getDateStr()[6 + i]);
    }
    lcd.print("/");
    lcd.print(TIME.getDateStr()[3]);
    lcd.print(TIME.getDateStr()[4]);
    lcd.print("/");
    lcd.print(TIME.getDateStr()[0]);
    lcd.print(TIME.getDateStr()[1]);
    lcd.setCursor(2, 1);
    lcd.print(TIME.getTimeStr());
    lcd.setCursor(11, 1);
    lcd.print(TIME.getDOWStr()[0]);
    lcd.print(TIME.getDOWStr()[1]);
    lcd.print(TIME.getDOWStr()[2]);
}
int ChangeMode() {
    if (IR.decode(&signal)) {
        switch(signal.value) {
            case RIGHT:
                if (mode == 4) {
                    mode = 0;
                }else if (mode == 5) {
                    mode = mode2;
                    IR.resume();
                    return 1;
                }else {
                    mode += 1;
                }
                change = 1;
                IR.resume();
                return 1;
            case LEFT:
                if (mode == 0) {
                    mode = 4;
                }else if (mode == 5) {
                    mode = mode2;
                    IR.resume();
                    return 1;
                }else {
                    mode -= 1;
                }
                change = 1;
                IR.resume();
                return 1;
            case UP:
            case DOWN:
            case CENTER:
                if (mode == 5) {
                    mode = mode2;
                    IR.resume();
                    return 1;
                }
        }
        IR.resume();
    }
    return 0;
}
void Mode3() {
    lcd.setCursor(4, 0);
    lcd.print("Set Hour");
    lcd.setCursor(4, 1);
    if (flash < FLASH / 2) {
        if (alarm_hour < 10) {
            lcd.print(0);
        }
        lcd.print(alarm_hour);
    }else {
        lcd.print("  ");
    }
    lcd.setCursor(7, 1);
    lcd.print(": ");
    if (alarm_minute < 10) {
        lcd.print(0);
    }
    lcd.print(alarm_minute);
    lcd.setCursor(13, 1);
    if (alarm_on) {
        lcd.print("On ");
    }else {
        lcd.print("Off");
    }
    if (IR.decode(&signal)) {
        switch (signal.value) {
            case UP:
                if (alarm_hour == 23) {
                    alarm_hour = 0;
                }else {
                    alarm_hour++;
                }
                flash = 0;
                break;
            case DOWN:
                if (alarm_hour == 0) {
                    alarm_hour = 23;
                }else {
                    alarm_hour--;
                }
                flash = 0;
                break;
            case PAUSE:
                alarm_on = true;
                break;
            case MENU:
                alarm_on = false;
        }
        ChangeMode();
    }
}
void Mode4() {
    lcd.setCursor(4, 0);
    lcd.print("Set Minute");
    lcd.setCursor(4, 1);
    if (alarm_hour < 10) {
        lcd.print(0);
    }
    lcd.print(alarm_hour);
    lcd.setCursor(7, 1);
    lcd.print(":");
    lcd.setCursor(9, 1);
    if (flash < FLASH / 2) {
        if (alarm_minute < 10) {
            lcd.print(0);
        }
        lcd.print(alarm_minute);
    }else {
        lcd.print("  ");
    }
    lcd.setCursor(13, 1);
    if (alarm_on) {
        lcd.print("On ");
    }else {
        lcd.print("Off");
    }
    if (IR.decode(&signal)) {
        switch (signal.value) {
            case UP:
                if (alarm_minute == 59) {
                    alarm_minute = 0;
                }else {
                    alarm_minute++;
                }
                flash = 0;
                break;
            case DOWN:
                if (alarm_minute == 0) {
                    alarm_minute = 59;
                }else {
                    alarm_minute--;
                }
                flash = 0;
                break;
            case PAUSE:
                alarm_on = true;
                break;
            case MENU:
                alarm_on = false;
        }
        ChangeMode();
    }
}















void sing(int s){
    // iterate over the notes of the melody:
    if (stop){
        return;
    }
    song = s;
    int IfBreak = 0;
    if(song==2){
        Serial.println(" 'Underworld Theme'");
        int size = sizeof(underworld_melody) / sizeof(int);
        for (int thisNote = 0; thisNote < size; thisNote++) {
            // to calculate the note duration, take one second
            // divided by the note type.
            //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000/underworld_tempo[thisNote];

            buzz(BUZZER, underworld_melody[thisNote],noteDuration);

            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            buzz(BUZZER, 0,noteDuration);
            if (ChangeMode()) {
                stop = 1;
            }
            if (stop){
                return;
            }
    }

    }else{

        Serial.println(" 'Mario Theme'");
        int size = sizeof(melody) / sizeof(int);
        for (int thisNote = 0; thisNote < size; thisNote++) {

            // to calculate the note duration, take one second
            // divided by the note type.
            //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000/tempo[thisNote];

            buzz(BUZZER, melody[thisNote],noteDuration);

            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);

            // stop the tone playing:
            buzz(BUZZER, 0,noteDuration);
            if (ChangeMode()) {
                stop = 1;
            }
            if (stop){
                return;
            }
        }
    }
}

void buzz(int targetPin, long frequency, long length) {
    digitalWrite(13,HIGH);
    long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
    // 1 second's worth of microseconds, divided by the frequency, then split in half since
    // there are two phases to each cycle
    long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
    // multiply frequency, which is really cycles per second, by the number of seconds to
    // get the total number of cycles to produce
    for (long i=0; i < numCycles; i++){ // for the calculated length of time...
        digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
        delayMicroseconds(delayValue); // wait for the calculated delay value
        digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
        delayMicroseconds(delayValue); // wait again or the calculated delay value
    }
    digitalWrite(13,LOW);
}
