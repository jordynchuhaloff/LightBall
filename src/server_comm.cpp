#include <ArduinoJson.h>
#include "Arduino.h"
#include <main.h>

String message = "";
bool messageReady = false;

int loopServer()
{
    while (Serial1.available()) {
        message = Serial1.readString();
        messageReady = true;
        // Serial.println("read message from Serial");
    }
    if (messageReady) {
        //
        StaticJsonDocument<400> doc;
        DeserializationError error = deserializeJson(doc, message);

        // Test if parsing succeeds.
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            Serial.print("message was: ");
            Serial.println(message);
            messageReady = false;
            return;
        }
        const char* command = doc["command"];
        const char* value = doc["value"];
        messageReady = false;
        Serial.println((String)"command: " + command + ", value: " + value);
        if ((strcmp( command, "gameOn") == 0)  && (strcmp( value, "true") == 0)) {
            Serial.println("GAME ON!");
        }
        if ((strcmp( command, "gameOn") == 0)  && (strcmp( value, "false") == 0)) {
            Serial.println("GAME OFF!");
        }
        if ((strcmp( command, "resetUndo") == 0)  && (strcmp( value, "undo") == 0)) {
            Serial.println("UNDO!");
            undoLastTurn();
        }
        if ((strcmp( command, "resetUndo") == 0)  && (strcmp( value, "reset") == 0)) {
            Serial.println("RESET!");
            resetGame();
        }
        if (strcmp( command, "brightness") == 0) {
            Serial.print("Set Brightness: ");
            if (strcmp(value, "1") == 0 ) {
                changeBrightness(1);
            } else if (strcmp(value, "2") == 0 ) {
                changeBrightness(2);
            } else if (strcmp(value, "3") == 0 ) {
                changeBrightness(3);
            }
            Serial.println(value);
        }
        if (strcmp( command, "gamemode") == 0) {
            Serial.print("Set Gamemode: ");
            Serial.println(value);
        }
    }
}