#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <EEPROM.h>
#include <Arduino.h>  

// ########### Declaração de variáveis globais ############

extern int numberOfAccounts; 
extern const int maxAccounts; 


// ########### Declaração de funções ############

void writeToEEPROM(int startAddr, const String &data);
String readFromEEPROM(int startAddr, int maxLength);
void clearEEPROM(int startAddr, int length);
void savePinToEEPROM(const String &pin);
String readPinFromEEPROM();
void loadAccountsFromEEPROM();
String getAccountName(int index);
String getAccountPassword(int index);
void deleteAccount(int accountIndex);
int getNextAccountIndex(String accountType);
bool accountNameExists(const String &name);

#endif




