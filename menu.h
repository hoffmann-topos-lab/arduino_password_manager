#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "lcd_interface.h"
#include "eeprom_manager.h"


// ########### Declaração de variáveis globais ############

// Definições dos botões
extern const int btnRight;
extern const int btnUp;
extern const int btnDown;
extern const int btnLeft;
extern const int btnSelect;


// Variáveis globais
extern int selectedOption;
extern int selectedAccount;
extern unsigned long displayTimeout;
extern const unsigned long timeoutDuration;

extern const int maxAccounts;
extern int numberOfAccounts;

// Variáveis globais para o controle de tentativas e bloqueio
extern int pinAttempts;
extern unsigned long lockoutUntil;
extern unsigned long lockoutDuration;

// Variáveis para debounce
extern const unsigned long debounceDelay;
extern int lastButtonState;
extern unsigned long lastDebounceTime;

extern bool isModification;
extern String tempPin;



// Estados do menu
enum MenuState {
    MAIN_MENU,
    PASSWORDS_MENU,
    DELETE_PASSWORD_MENU, 
    ACCOUNTS_MENU,
    ACCOUNT_SELECTED,
    RESET_DEVICE,
    CREATE_ACCOUNT_MENU,
    SETUP_PIN,
    VERIFY_PIN,
    SELECT_ACCOUNT_TYPE, 
    ENTER_ACCOUNT_NAME,
    CONFIRM_RESET,
    VERIFY_CURRENT_PIN,
    MODIFY_PIN,
    CONFIRM_NEW_PIN
};

extern MenuState menuState;
extern MenuState previousMenuState;


// ########### Declaração de funções ############

void showSelectedOption();
void executeSelectedOption();
void navigate(int direction);
void resetLockout();
String getNewAccountName();
bool verifyPin(bool isConfirming = false);
void setupPin(bool isConfirmation = false, bool isModification = false);
void resetLockout();
String getNewAccountName();
void writeAccountNameToEEPROM(int index, const String &name);


#endif
