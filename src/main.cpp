#include "Arduino.h"
//#include <ArduinoSTL.h>
#include <FastLED.h>

#define COLOR_ORDER    BRG
#define LED_TYPE       WS2811
#define NUM_LEDS       37
#define NUM_SQUARES    9

#define DETECTOR_A1    22
#define DETECTOR_A2    23
#define DETECTOR_A3    24
#define DETECTOR_B1    25
#define DETECTOR_B2    26
#define DETECTOR_B3    27
#define DETECTOR_C1    28
#define DETECTOR_C2    29
#define DETECTOR_C3    30

#define LED_PIN_A1    36
#define LED_PIN_A2    38
#define LED_PIN_A3    40
#define LED_PIN_B1    42
#define LED_PIN_B2    44
#define LED_PIN_B3    46
#define LED_PIN_C1    48
#define LED_PIN_C2    50
#define LED_PIN_C3    52

int BRIGHTNESS = 255;

CRGB LED_STRIPS[9][NUM_LEDS];

CRGB COLOR_P1 = CRGB(0x00CC00);
CRGB COLOR_P2 = CRGB(0xFF0066);
CRGB COLOR_OFF = CRGB(0x000000);

unsigned long previousMillis = 0; // time at start of program
int cellsState[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 0 == unclaimed | 1 == player 1 | 2 == player 2
int turnCount = 0;
int turnList[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };

int winConditions[8][3] =
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
        {1, 4, 7},
        {2, 5, 8},
        {3, 6, 9},
        {1, 5, 9},
        {3, 5, 7},
    };
boolean gameWon = false;
int celebrateCount = 0; // num times to flash lights (when game is won)
int wonConditionIndex; // which winCondition won the game

void setup()
{
    Serial.begin(9600);

    pinMode(DETECTOR_A1, INPUT);
    pinMode(DETECTOR_A2, INPUT);
    pinMode(DETECTOR_A3, INPUT);
    pinMode(DETECTOR_B1, INPUT);
    pinMode(DETECTOR_B2, INPUT);
    pinMode(DETECTOR_B3, INPUT);
    pinMode(DETECTOR_C1, INPUT);
    pinMode(DETECTOR_C2, INPUT);
    pinMode(DETECTOR_C3, INPUT);

    CFastLED::addLeds<LED_TYPE, LED_PIN_A1, COLOR_ORDER>(LED_STRIPS[0], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_A2, COLOR_ORDER>(LED_STRIPS[1], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_A3, COLOR_ORDER>(LED_STRIPS[2], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_B1, COLOR_ORDER>(LED_STRIPS[3], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_B2, COLOR_ORDER>(LED_STRIPS[4], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_B3, COLOR_ORDER>(LED_STRIPS[5], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_C1, COLOR_ORDER>(LED_STRIPS[6], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_C2, COLOR_ORDER>(LED_STRIPS[7], NUM_LEDS).setCorrection( TypicalLEDStrip );
    CFastLED::addLeds<LED_TYPE, LED_PIN_C3, COLOR_ORDER>(LED_STRIPS[8], NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

}

CRGB getCurrentPlayerColor() {
    if (turnCount % 2 == 0 ) {
        return COLOR_P1;
    } else {
        return COLOR_P2;
    }
}

void setCellColor(int cellIndex, const CRGB& color) {
    for( int i = 0; i < NUM_LEDS; i++) {
        LED_STRIPS[cellIndex][i] = color;
    }
}

void celebrate() {
    CRGB color = getCurrentPlayerColor();

    for (int i = 0; i < 3; i++) {
        int cellIndex = winConditions[wonConditionIndex][i] - 1;

        if (celebrateCount % 2 == 0) {
            setCellColor(cellIndex, COLOR_OFF);
        } else {
            setCellColor(cellIndex, color);
        }
    }
}

void recordTurn(int cellIndex) {
    if (turnCount % 2 == 0 ) {
        Serial.println("Player 1");
        cellsState[cellIndex] = 1;
    } else {
        Serial.println("Player 2");
        cellsState[cellIndex] = 2;
    }
    turnList[turnCount] = cellIndex;
}

void printTurnMatrix() {
    for(int i = 0; i < NUM_SQUARES; i++) {
        Serial.print((String)"| " + cellsState[i] + " ");

        if ((i + 1) % 3 == 0) {
            Serial.print("|\n");
        }
    }
}

void checkWinConditions() {
    for (int i = 0; i < 8; i++) {
        int player1CellCount = 0;
        int player2CellCount = 0;
        int winningPlayer = 0;

        for (int j = 0; j < 3; j++) {
            int cellIndex = winConditions[i][j] - 1;

            if (cellsState[cellIndex] == 1) {
                player1CellCount++;
            } else if (cellsState[cellIndex] == 2) {
                player2CellCount++;
            }

            if (player1CellCount == 3) {
                winningPlayer = 1;
            } else if (player2CellCount == 3) {
                winningPlayer = 2;
            }

            if (winningPlayer != 0) {
                Serial.println((String) "\nPlayer " + winningPlayer + " wins! Cells: " + winConditions[i][0] + ", " +
                               winConditions[i][1] + ", " + winConditions[i][2]);
                gameWon = true;
                celebrateCount = 10;
                wonConditionIndex = i;
                return;
            }
        }

        Serial.println((String)"Win condition: " + (i + 1) + " | P1 cell count: " + player1CellCount + " | P2 cell count: " + player2CellCount);
    }
    Serial.println();
}

void handleCellTriggered(int cellIndex, bool isUndo) {
    if (isUndo) {
        setCellColor(cellIndex, COLOR_OFF);
    } else {
        if (cellsState[cellIndex] != 0) { // cell is already claimed
            return;
        }
        CRGB color = getCurrentPlayerColor();
        setCellColor(cellIndex, color);
        recordTurn(cellIndex);
        printTurnMatrix();
        checkWinConditions();

        if (!gameWon) {
            turnCount++;
        }
    }
}

void loop()
{
    unsigned long currentMillis = millis();

    if (gameWon && celebrateCount == 0) { // game won, celebration complete

    } else if (gameWon && celebrateCount > 0) { // game won, celebratory light flashes happening
        if (currentMillis - previousMillis >= 300) {
            previousMillis = currentMillis;
            celebrate();
            celebrateCount--;
        }
    } else { // game ongoing
        boolean A1Triggered = digitalRead(DETECTOR_A1);
        boolean A2Triggered = digitalRead(DETECTOR_A2);
        boolean A3Triggered = digitalRead(DETECTOR_A3);
        boolean B1Triggered = digitalRead(DETECTOR_B1);
        boolean B2Triggered = digitalRead(DETECTOR_B2);
        boolean B3Triggered = digitalRead(DETECTOR_B3);
        boolean C1Triggered = digitalRead(DETECTOR_C1);
        boolean C2Triggered = digitalRead(DETECTOR_C2);
        boolean C3Triggered = digitalRead(DETECTOR_C3);

        if (currentMillis < 2000) { // allow time for lasers and sensors to turn on at start of game
            return;
        } else {
            if (A1Triggered != 0) {
                handleCellTriggered(0, false);
            }
            if (A2Triggered != 0) {
                handleCellTriggered(1, false);
            }
            if (A3Triggered != 0) {
                handleCellTriggered(2, false);
            }

            if (B1Triggered != 0) {
                handleCellTriggered(3, false);
            }
            if (B2Triggered != 0) {
                handleCellTriggered(4, false);
            }
            if (B3Triggered != 0) {
                handleCellTriggered(5, false);
            }

            if (C1Triggered != 0) {
                handleCellTriggered(6, false);
            }
            if (C2Triggered != 0) {
                handleCellTriggered(7, false);
            }
            if (C3Triggered != 0) {
                handleCellTriggered(8, false);
            }
        }
    }

    FastLED.show();
}

void undoLastTurn() {
    int cellIndex = turnList[turnCount-1];
    Serial.println((String)" Undoing last turn at cellIndex: " + cellIndex + " ");
    handleCellTriggered(cellIndex, true);
    turnList[turnCount-1] = -1;
    cellsState[cellIndex] = 0;
    turnCount--;
}

int changeBrightness(int brightnessLevel) {
    if (brightnessLevel == 1) {
        BRIGHTNESS = 45;
    } else if (brightnessLevel == 2) {
        BRIGHTNESS = 145;
    } else if (brightnessLevel == 3) {
        BRIGHTNESS = 255;
    }
    FastLED.setBrightness(BRIGHTNESS);
}

int resetGame() {
    gameWon = false;
    turnCount = 0;
    for (int i = 0; i < 9; i++) {
        setCellColor(i, COLOR_OFF);
        cellsState[i] = 0;
    }
}
