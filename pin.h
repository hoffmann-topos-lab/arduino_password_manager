#ifndef PIN_H
#define PIN_H

#include "lcd_interface.h"
#include "eeprom_manager.h"
#include "menu.h"


// ########### Declaração de variáveis globais ############

extern String tempPin;


// ########### Declaração de funções ############

void setupPin(bool isConfirmation = false, bool isModification = false);
bool verifyPin(bool isConfirming = false);

#endif
