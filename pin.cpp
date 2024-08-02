#include "pin.h"
#include <Arduino.h>  
#include "lcd_interface.h"
#include "eeprom_manager.h"

extern String tempPin; // Variável temporária para armazenar o PIN

// Função para configurar o PIN
void setupPin(bool isConfirmation, bool isModification) {
    String pin = ""; // Armazena o PIN digitado
    int pinIndex = 0;
    int pinValue = 0;
    lcd.clear();
    if (isConfirmation) {
        lcd.print("Confirme o PIN:");
    } else if (isModification) {
        lcd.print("Defina o novo PIN:");
    } else {
        lcd.print("Defina o PIN:");
    }

    while (pin.length() < 4) { // Repete até que o PIN tenha 4 dígitos
        int button = readButtons();
        if (button == btnUp) {
            pinValue = (pinValue + 1) % 10; // Incrementa o valor do dígito
        } else if (button == btnDown) {
            pinValue = (pinValue + 9) % 10; // Decrementa o valor do dígito
        } else if (button == btnSelect) {
            pin += String(pinValue); // Adiciona o dígito ao PIN
            pinIndex++;
        }

        lcd.setCursor(pinIndex, 1);
        lcd.print(pinValue); // Exibe o dígito no LCD
        delay(100);
    }

    if (isConfirmation) {
        if (pin == tempPin) {
            savePinToEEPROM(pin); // Salva o PIN na EEPROM se confirmado
            printWrapped("PIN alterado com sucesso.");
            delay(2000);
            menuState = MAIN_MENU;
        } else {
            printWrapped("PINs não correspondem, tente novamente.");
            delay(2000);
            menuState = MODIFY_PIN;
        }
    } else {
        tempPin = pin;
        if (isModification) {
            menuState = CONFIRM_NEW_PIN;
        } else {
            savePinToEEPROM(pin); // Salva o PIN na EEPROM
            menuState = MAIN_MENU;
        }
    }
    showSelectedOption(); // Atualiza a exibição após a configuração do PIN
}

// Função para verificar o PIN
bool verifyPin(bool isConfirming) {
    String storedPin = isConfirming ? tempPin : readPinFromEEPROM(); // PIN armazenado
    String enteredPin = "";
    int pinIndex = 0;
    int pinValue = 0;
    lcd.clear();
    lcd.print(isConfirming ? "Confirme o PIN:" : "Digite o PIN:");

    while (enteredPin.length() < 4) { // Repete até que o PIN tenha 4 dígitos
        int button = readButtons();
        if (button == btnUp) {
            pinValue = (pinValue + 1) % 10; // Incrementa o valor do dígito
        } else if (button == btnDown) {
            pinValue = (pinValue + 9) % 10; // Decrementa o valor do dígito
        } else if (button == btnSelect) {
            enteredPin += String(pinValue); // Adiciona o dígito ao PIN digitado
            pinIndex++;
        }

        lcd.setCursor(pinIndex, 1);
        lcd.print(pinValue); // Exibe o dígito no LCD
        delay(100);
    }

    if (enteredPin == storedPin) { // Verifica se o PIN digitado está correto
        pinAttempts = 0;
        return true;
    } else {
        pinAttempts++;
        if (pinAttempts >= 3) { // Bloqueia o dispositivo após 3 tentativas incorretas
            lockoutUntil = millis() + lockoutDuration;
            lockoutDuration += 30000;
            lcd.clear();
            lcd.print("Bloqueado por ");
            lcd.print(lockoutDuration / 1000);
            lcd.print("s");
            delay(2000); // Mostrar mensagem por 2 segundos
        } else {
            lcd.clear();
            lcd.print("PIN Incorreto!");
            delay(2000);
        }
    }

    return false;
}
