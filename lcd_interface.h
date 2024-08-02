#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

#include <LiquidCrystal.h>


// ########### Declaração de variáveis globais ############

extern LiquidCrystal lcd;


// ########### Declaração de funções ############

void initializeLCD();
void printWrapped(const String &message);
int readButtons();
void showConfirmationOption();

#endif

