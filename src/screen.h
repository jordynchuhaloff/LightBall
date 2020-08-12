#ifndef SCREEN_H
#define SCREEN_H

#include <LCDWIKI_GUI.h>
#include <LCDWIKI_KBV.h>

int show_main_menu_moc();
int setupScreen();
int loopScreen();
int drawMove(int cellsState[9]);
void setUndoing(bool undoing);

#endif //LIGHTBALL_SCREEN_H