//
// Created by Marc O'Cleirigh on 8/12/20.
//
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "../.pio/libdeps/megaatmega2560/DFRobotDFPlayerMini/DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(11, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

bool winSoundPlaying = false;

int setupSpeaker() {
    delay(2000);
    mySoftwareSerial.begin(9600);
    Serial.println(F("Trying to begin speaker"));
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while(true);
    }
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.setTimeOut(500);
    myDFPlayer.volume(25);  //Set volume value. From 0 to 30
    myDFPlayer.loop(3);  //Play the first mp3*/
}

int playCellTriggeredSound() {
    Serial.println("Playing sound");
    myDFPlayer.advertise(1);
}

int playWinSound() {
    Serial.println("Playing win sound");
    myDFPlayer.playFolder(2, 5);
    winSoundPlaying = true;
}


int resetMusic() {
    myDFPlayer.loop(3);  //Play the first mp3*/
}

int playUndoSound() {
    myDFPlayer.advertise(3);  //Play the first mp3*/
}