#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <main.h>
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_KBV my_lcd(320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

using namespace std;

//param calibration from kbv
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92
#define TS_MAXY 952

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#define MINPRESSURE 10
#define MAXPRESSURE 1000


uint16_t color_mask[] = {0xF800,0xFFE0,0x07E0,0x07FF,0x001F,0xF81F}; //color select

#define COLORBOXSIZE my_lcd.Get_Display_Width()/6

int16_t flag_colour;
int16_t flag_pen;
int16_t flag_gameon;
boolean show_flag = true;

const uint8_t BUTTON_GAP = 5;
const uint8_t BTN_ROUNDNESS = 5;
const uint8_t BUTTON_HEIGHT = 80;
const uint8_t BTN_X1 = BUTTON_GAP;
const uint16_t BTN_X2 = my_lcd.Get_Display_Width() - BUTTON_GAP;
const uint16_t BTN_START_Y1 = BUTTON_GAP;
const uint16_t BTN_START_Y2 = BTN_START_Y1 + BUTTON_HEIGHT;
const uint16_t BTN_GAMEMODE_Y1 = BTN_START_Y2 + BUTTON_GAP;
const uint16_t BTN_GAMEMODE_Y2 = BTN_GAMEMODE_Y1 + BUTTON_HEIGHT;
const uint16_t BTN_BRIGHTNESS_Y1 = BTN_GAMEMODE_Y2 + BUTTON_GAP;
const uint16_t BTN_BRIGHTNESS_Y2 = BTN_BRIGHTNESS_Y1 + BUTTON_HEIGHT;

const uint8_t BTN_BACK_HEIGHT = 40;
const uint8_t BTN_BACK_X1 = 20;
const uint16_t BTN_BACK_X2 = my_lcd.Get_Display_Width() - 20;
const uint16_t BTN_BACK_Y1 = my_lcd.Get_Display_Height() - BTN_BACK_HEIGHT- BUTTON_GAP;
const uint16_t BTN_BACK_Y2 = my_lcd.Get_Display_Height() - BUTTON_GAP;
const uint16_t TXT_BACK_BTN_X = (my_lcd.Get_Display_Width()/2) - 20;
const uint16_t TXT_BACK_BTN_Y = BTN_BACK_Y1 + ((BTN_BACK_Y2 - BTN_BACK_Y1)/2) - 5;

const uint16_t BTN_UNDO_Y1 = BTN_BACK_Y1 - BTN_BACK_HEIGHT - BUTTON_GAP;
const uint16_t BTN_UNDO_Y2 = BTN_BACK_Y1 - BUTTON_GAP;
const uint16_t TXT_UNO_BTN_X = (my_lcd.Get_Display_Width()/2) - 20;
const uint16_t TXT_UNDO_BTN_Y = BTN_UNDO_Y1 + ((BTN_UNDO_Y2 - BTN_UNDO_Y1)/2) - 5;

const uint8_t TXT_HEIGHT_OFFSET = 5;
const uint16_t TXT_GAMEMODE_Y = BTN_GAMEMODE_Y1 + ((BTN_GAMEMODE_Y2 - BTN_GAMEMODE_Y1)/2) - TXT_HEIGHT_OFFSET;
const uint16_t TXT_START_Y = BTN_START_Y1 + ((BTN_START_Y2 - BTN_START_Y1)/2) - TXT_HEIGHT_OFFSET;
const uint16_t TXT_BRIGHTNESS_Y = BTN_BRIGHTNESS_Y1 + ((BTN_BRIGHTNESS_Y2 - BTN_BRIGHTNESS_Y1)/2) - TXT_HEIGHT_OFFSET;
const uint16_t TXT_X = my_lcd.Get_Display_Width()/2;

uint8_t currentGameMode = 1;
uint8_t currentBrightness = 3;
const uint8_t CHECK_WIDTH = 30;
const uint8_t CHECK_HEIGHT = 30;
const uint8_t CHECK_GAP = 80;
const uint8_t CHECK_X1 = 10;
const uint8_t CHECK_X2 = CHECK_X1 + CHECK_WIDTH;
const uint8_t CHECK1_Y1 = 10;
const uint16_t CHECK1_Y2 = CHECK1_Y1 + CHECK_HEIGHT;
const uint16_t CHECK2_Y1 = CHECK1_Y2 + CHECK_GAP;
const uint16_t CHECK2_Y2 = CHECK2_Y1 + CHECK_HEIGHT;
const uint16_t CHECK3_Y1 = CHECK2_Y2 + CHECK_GAP;
const uint16_t CHECK3_Y2 = CHECK3_Y1 + CHECK_HEIGHT;

const uint16_t CHECK_TEXT_X = CHECK_WIDTH/2 + 5;
const uint16_t CHECK1_TEXT_Y = CHECK1_Y1 + 8;
const uint16_t CHECK2_TEXT_Y = CHECK2_Y1 + 8;
const uint16_t CHECK3_TEXT_Y = CHECK3_Y1 + 8;
const uint8_t CHECK_TEXT_LINE_OFFSET = 20;

/**GAME STARTED MENU -- BELOW DRAWS THE GAME BOARD*/
//show game on menu
const uint8_t CELL_SIZE = 80;
const uint8_t EDGE_OFFSET = 40;
const uint8_t CELL_GAP = 1;

const uint16_t CELLA1_X2 = my_lcd.Get_Display_Width() - EDGE_OFFSET;
const uint16_t CELLA1_X1 = CELLA1_X2 - CELL_SIZE;
const uint16_t CELLA1_Y1 = EDGE_OFFSET;
const uint16_t CELLA1_Y2 = CELLA1_Y1 + CELL_SIZE;

const uint16_t CELLA2_X2 = CELLA1_X1 - CELL_GAP;
const uint16_t CELLA2_X1 = CELLA1_X1 - CELL_GAP - CELL_SIZE;
const uint16_t CELLA2_Y1 = EDGE_OFFSET;
const uint16_t CELLA2_Y2 = CELLA2_Y1 + CELL_SIZE;

const uint16_t CELLA3_X2 = CELLA2_X1 - CELL_GAP;
const uint16_t CELLA3_X1 = CELLA2_X1 - CELL_GAP - CELL_SIZE;
const uint16_t CELLA3_Y1 = EDGE_OFFSET;
const uint16_t CELLA3_Y2 = CELLA3_Y1 + CELL_SIZE;

const uint16_t CELLB1_X1 = CELLA1_X1;
const uint16_t CELLB1_X2 = CELLA1_X2;
const uint16_t CELLB1_Y1 = CELLA1_Y2 + CELL_GAP;
const uint16_t CELLB1_Y2 = CELLB1_Y1 + CELL_SIZE;

const uint16_t CELLB2_X1 = CELLA2_X1;
const uint16_t CELLB2_X2 = CELLA2_X2;
const uint16_t CELLB2_Y1 = CELLA2_Y2 + CELL_GAP;
const uint16_t CELLB2_Y2 = CELLB2_Y1 + CELL_SIZE;

const uint16_t CELLB3_X1 = CELLA3_X1;
const uint16_t CELLB3_X2 = CELLA3_X2;
const uint16_t CELLB3_Y1 = CELLA3_Y2 + CELL_GAP;
const uint16_t CELLB3_Y2 = CELLB3_Y1 + CELL_SIZE;

const uint16_t CELLC1_X1 = CELLB1_X1;
const uint16_t CELLC1_X2 = CELLB1_X2;
const uint16_t CELLC1_Y1 = CELLB1_Y2 + CELL_GAP;
const uint16_t CELLC1_Y2 = CELLC1_Y1 + CELL_SIZE;

const uint16_t CELLC2_X1 = CELLB2_X1;
const uint16_t CELLC2_X2 = CELLB2_X2;
const uint16_t CELLC2_Y1 = CELLB2_Y2 + CELL_GAP;
const uint16_t CELLC2_Y2 = CELLC2_Y1 + CELL_SIZE;

const uint16_t CELLC3_X1 = CELLB3_X1;
const uint16_t CELLC3_X2 = CELLB3_X2;
const uint16_t CELLC3_Y1 = CELLB3_Y2 + CELL_GAP;
const uint16_t CELLC3_Y2 = CELLC3_Y1 + CELL_SIZE;

void show_string(char *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String((uint8_t *)str,x,y);
}

//show gamemode select menu
void show_gamemode_select_menu()
{
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK1_Y1, CHECK_X2, CHECK1_Y2);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK2_Y1, CHECK_X2, CHECK2_Y2);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK3_Y1, CHECK_X2, CHECK3_Y2);
    my_lcd.Set_Draw_color(GREEN);
    my_lcd.Fill_Round_Rectangle(BTN_BACK_X1, BTN_BACK_Y1, BTN_BACK_X2, BTN_BACK_Y2, BTN_ROUNDNESS);
    show_string((char *)"Back", TXT_BACK_BTN_X, TXT_BACK_BTN_Y, 2, BLACK, BLACK, true);
    if (currentGameMode == 1) {
        show_string( (char *)"X", CHECK_TEXT_X, CHECK1_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK2_Y1+1, CHECK_X2-1, CHECK2_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK3_Y1+1, CHECK_X2-1, CHECK3_Y2-1);
    }
    else if (currentGameMode == 2) {
        show_string((char *)"X", CHECK_TEXT_X, CHECK2_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK1_Y1+1, CHECK_X2-1, CHECK1_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK3_Y1+1, CHECK_X2-1, CHECK3_Y2-1);
    }
    else if (currentGameMode == 3) {
        show_string((char *)"X", CHECK_TEXT_X, CHECK3_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK1_Y1+1, CHECK_X2-1, CHECK1_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK2_Y1+1, CHECK_X2-1, CHECK2_Y2-1);
    }

    show_string((char *)" <- Game Mode 1:",CHECK_TEXT_X + 40, CHECK1_TEXT_Y,2,WHITE, BLACK,true);
    show_string((char *)"Normal Tic Tac Toe",CHECK_TEXT_X + 80, CHECK1_TEXT_Y + CHECK_TEXT_LINE_OFFSET,2,WHITE, BLACK,true);

    show_string((char *)" <- Game Mode 2:",CHECK_TEXT_X + 40, CHECK2_TEXT_Y,2,WHITE, BLACK,true);
    show_string((char *)"Capture the Square",CHECK_TEXT_X + 80, CHECK2_TEXT_Y + CHECK_TEXT_LINE_OFFSET,2,WHITE, BLACK,true);

    show_string((char *)" <- Game Mode 3:",CHECK_TEXT_X + 40, CHECK3_TEXT_Y,2,WHITE, BLACK,true);
    show_string((char *)"Solo game",CHECK_TEXT_X + 80, CHECK3_TEXT_Y + CHECK_TEXT_LINE_OFFSET,2,WHITE, BLACK,true);
}

//show pen size select menu
void show_brightness_select_menu()
{
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK1_Y1, CHECK_X2, CHECK1_Y2);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK2_Y1, CHECK_X2, CHECK2_Y2);
    my_lcd.Draw_Rectangle(CHECK_X1, CHECK3_Y1, CHECK_X2, CHECK3_Y2);
    my_lcd.Set_Draw_color(GREEN);
    my_lcd.Fill_Round_Rectangle(BTN_BACK_X1, BTN_BACK_Y1, BTN_BACK_X2, BTN_BACK_Y2, BTN_ROUNDNESS);
    show_string((char *)"Back", TXT_BACK_BTN_X, TXT_BACK_BTN_Y, 2, BLACK, BLACK, true);
    if (currentBrightness == 1) {
        show_string((char *)"X", CHECK_TEXT_X, CHECK1_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK2_Y1+1, CHECK_X2-1, CHECK2_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK3_Y1+1, CHECK_X2-1, CHECK3_Y2-1);
    }
    else if (currentBrightness == 2) {
        show_string((char *)"X", CHECK_TEXT_X, CHECK2_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK1_Y1+1, CHECK_X2-1, CHECK1_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK3_Y1+1, CHECK_X2-1, CHECK3_Y2-1);
    }
    else if (currentBrightness == 3) {
        show_string((char *)"X", CHECK_TEXT_X, CHECK3_TEXT_Y, 2, WHITE, BLACK, true);
        my_lcd.Set_Draw_color(BLACK);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK1_Y1+1, CHECK_X2-1, CHECK1_Y2-1);
        my_lcd.Fill_Rectangle(CHECK_X1+1, CHECK2_Y1+1, CHECK_X2-1, CHECK2_Y2-1);
    }

    show_string((char *)" <- Low",CHECK_TEXT_X + 40, CHECK1_TEXT_Y,2,WHITE, BLACK,true);
    show_string((char *)" <- Medium",CHECK_TEXT_X + 40, CHECK2_TEXT_Y,2,WHITE, BLACK,true);
    show_string((char *)" <- High",CHECK_TEXT_X + 40, CHECK3_TEXT_Y,2,WHITE, BLACK,true);
}

void show_game_on_menu()
{
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Rectangle(CELLA1_X1, CELLA1_Y1, CELLA1_X2, CELLA1_Y2);
    my_lcd.Draw_Rectangle(CELLA2_X1, CELLA2_Y1, CELLA2_X2, CELLA2_Y2);
    my_lcd.Draw_Rectangle(CELLA3_X1, CELLA3_Y1, CELLA3_X2, CELLA3_Y2);

    my_lcd.Draw_Rectangle(CELLB1_X1, CELLB1_Y1, CELLB1_X2, CELLB1_Y2);
    my_lcd.Draw_Rectangle(CELLB2_X1, CELLB2_Y1, CELLB2_X2, CELLB2_Y2);
    my_lcd.Draw_Rectangle(CELLB3_X1, CELLB3_Y1, CELLB3_X2, CELLB3_Y2);

    my_lcd.Draw_Rectangle(CELLC1_X1, CELLC1_Y1, CELLC1_X2, CELLC1_Y2);
    my_lcd.Draw_Rectangle(CELLC2_X1, CELLC2_Y1, CELLC2_X2, CELLC2_Y2);
    my_lcd.Draw_Rectangle(CELLC3_X1, CELLC3_Y1, CELLC3_X2, CELLC3_Y2);

    my_lcd.Set_Draw_color(GREEN);
    my_lcd.Fill_Round_Rectangle(BTN_BACK_X1, BTN_BACK_Y1, BTN_BACK_X2, BTN_BACK_Y2, BTN_ROUNDNESS);
    show_string((char *)"END GAME", TXT_BACK_BTN_X - 30, TXT_BACK_BTN_Y, 2, BLACK, BLACK, true);
    if (currentBrightness == 1) {
    }
}

int drawMove(int cellsState[9]) {
    my_lcd.Set_Draw_color(WHITE);
    int cellWidth = CELLA1_X2 - CELLA1_X1;
    int halfCellWidth = cellWidth/2;
    if (cellsState[0] != 0) {
        int textX = CELLA1_X1  + halfCellWidth- 10;
        int textY = CELLA1_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
    if (cellsState[1] != 0) {
        int textX = CELLA2_X1  + halfCellWidth- 10;
        int textY = CELLA2_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, CELLA1_Y1, 4, WHITE, WHITE, true);
    }
    if (cellsState[2] != 0) {
        int textX = CELLA2_X1  + halfCellWidth- 10;
        int textY = CELLA3_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, CELLA1_Y1, 4, WHITE, WHITE, true);
    }

    if (cellsState[3] != 0) {
        // Serial.println("Drawing X in square 1");
        int textX = CELLB1_X1  + halfCellWidth- 10;
        int textY = CELLB1_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
    if (cellsState[4] != 0) {
        int textX = CELLB2_X1  + halfCellWidth- 10;
        int textY = CELLB2_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
    if (cellsState[5] != 0) {
        int textX = CELLB3_X1  + halfCellWidth- 10;
        int textY = CELLB3_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }

    if (cellsState[6] != 0) {
        // Serial.println("Drawing X in square 1");
        int textX = CELLC1_X1  + halfCellWidth- 10;
        int textY = CELLC1_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
    if (cellsState[7] != 0) {
        int textX = CELLC2_X1  + halfCellWidth- 10;
        int textY = CELLC2_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
    if (cellsState[8] != 0) {
        int textX = CELLC3_X1  + halfCellWidth- 10;
        int textY = CELLC3_Y1  + halfCellWidth- 10;
        show_string((char *)"X", textX, textY, 4, WHITE, WHITE, true);
    }
}
//show main menu
void show_main_menu_moc()
{
    my_lcd.Fill_Screen(BLACK);
    my_lcd.Set_Draw_color(YELLOW);
    my_lcd.Fill_Round_Rectangle(BTN_X1, BTN_GAMEMODE_Y1, BTN_X2, BTN_GAMEMODE_Y2, BTN_ROUNDNESS);
    my_lcd.Fill_Round_Rectangle(BTN_X1, BTN_GAMEMODE_Y1, BTN_X2, BTN_GAMEMODE_Y2, BTN_ROUNDNESS);
    my_lcd.Set_Draw_color(GREEN);
    my_lcd.Fill_Round_Rectangle(BTN_X1, BTN_START_Y1, BTN_X2, BTN_START_Y2, BTN_ROUNDNESS);
    my_lcd.Set_Draw_color(MAGENTA);
    my_lcd.Fill_Round_Rectangle(BTN_X1, BTN_BRIGHTNESS_Y1, BTN_X2, BTN_BRIGHTNESS_Y2, BTN_ROUNDNESS);
    show_string((char *)"GAMEMODE",TXT_X - 45, TXT_GAMEMODE_Y,2,BLUE, BLACK,true);
    show_string((char *)"START A GAME!",TXT_X - 58, TXT_START_Y,2,BLACK, BLACK,true);
    show_string((char *)"BRIGHTNESS",TXT_X - 55, TXT_BRIGHTNESS_Y, 2,WHITE, BLACK,true);
}

int setupScreen()
{
    my_lcd.Init_LCD();
    // Serial.println(my_lcd.Read_ID(), HEX);
    show_main_menu_moc();
    pinMode(13, OUTPUT);
    // printf("Hello World\n");
}

int loopScreen()
{
    comme:
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
    {
        p.x = map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
        p.y = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(),0);
        if( (p.x > BTN_X1 && p.x < BTN_X2) && (p.y >= BTN_GAMEMODE_Y1) && (p.y <= BTN_GAMEMODE_Y2) && !flag_pen) //select gamemode
        {
            flag_colour = 1;
            if(show_flag)
            {
                my_lcd.Fill_Screen(BLACK);
                show_gamemode_select_menu();
            }
            show_flag = false;
        }
        if(flag_colour)
        {
            if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK1_Y1 && p.y <= CHECK1_Y2) {
                currentGameMode = 1;
                show_gamemode_select_menu();
            }
            else if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK2_Y1 && p.y <= CHECK2_Y2) {
                currentGameMode = 2;
                show_gamemode_select_menu();
            }
            else if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK3_Y1 && p.y <= CHECK3_Y2) {
                currentGameMode = 3;
                show_gamemode_select_menu();
            }
            else if(p.x >= BTN_BACK_X1 && p.x <= BTN_BACK_X2 && p.y >= BTN_BACK_Y1 && p.y <= BTN_BACK_Y2)
            {
                my_lcd.Set_Draw_color(BLACK);
                my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                show_main_menu_moc();
                flag_colour = 0;
                show_flag = true;
                goto comme;

            }
        }
        if( (p.x > BTN_X1 && p.x < BTN_X2) && (p.y >= BTN_BRIGHTNESS_Y1) && (p.y <= BTN_BRIGHTNESS_Y2)  &&!flag_colour) //select pen size
        {
            flag_pen = 1;
            if(show_flag)
            {
                my_lcd.Fill_Screen(BLACK);
                show_brightness_select_menu();
            }
            show_flag = false;
        }
        if(flag_pen)
        {
            if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK1_Y1 && p.y <= CHECK1_Y2) {
                currentBrightness = 1;
                changeBrightness(1);
                show_brightness_select_menu();
            }
            else if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK2_Y1 && p.y <= CHECK2_Y2) {
                currentBrightness = 2;
                changeBrightness(2);
                show_brightness_select_menu();
            }
            else if (p.x >= CHECK_X1 && p.x <= CHECK_X2 && p.y >= CHECK3_Y1 && p.y <= CHECK3_Y2) {
                currentBrightness = 3;
                changeBrightness(3);
                show_brightness_select_menu();
            }
            else if(p.x >= BTN_BACK_X1 && p.x <= BTN_BACK_X2 && p.y >= BTN_BACK_Y1 && p.y <= BTN_BACK_Y2)
            {
                show_main_menu_moc();
                flag_pen = 0;
                show_flag = true;
                goto comme;

            }

        }
        if((p.x > BTN_X1 && p.x < BTN_X2) && (p.y >= BTN_START_Y1) && (p.y <= BTN_START_Y2) &&!flag_colour&&!flag_pen)
        {
            flag_gameon = 1;
            if (show_flag) {
                my_lcd.Fill_Screen(BLACK);
                show_game_on_menu();
            }
            show_flag = false;
        }
        if(flag_gameon)
        {
            if (p.x >= BTN_BACK_X1 && p.y >= BTN_BACK_Y1 && p.x <= BTN_BACK_X2 && p.y <= BTN_BACK_Y2)
            {
                show_main_menu_moc();
                resetGame();
                flag_gameon = 0;
                show_flag = true;
                goto comme;
            }
        }

    }
}