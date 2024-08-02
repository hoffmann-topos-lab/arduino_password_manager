#include <Arduino.h>
#include "lcd_interface.h"
#include "eeprom_manager.h"
#include "menu.h"
#include "pin.h"
#include "password_generator.h"


// Definição dos botões
const int btnRight = 0;
const int btnUp = 1;
const int btnDown = 2;
const int btnLeft = 3;
const int btnSelect = 4;

String tempPin = ""; // Variável temporária para armazenar o PIN
const unsigned long debounceDelay = 50; // Delay para evitar debounce de botões

// Função de configuração inicial do Arduino
void setup() {
    Serial.begin(9600); // Inicializa a comunicação serial
    initializeLCD(); // Inicializa o display LCD
    loadAccountsFromEEPROM(); // Carrega as contas armazenadas na EEPROM
    String storedPin = readPinFromEEPROM(); // Lê o PIN armazenado na EEPROM
    if (storedPin.length() != 4) {
        menuState = SETUP_PIN; // Se o PIN não está configurado, vai para a configuração do PIN
    } else {
        menuState = VERIFY_PIN; // Se o PIN está configurado, vai para a verificação do PIN
    }
    showSelectedOption(); // Mostra a opção selecionada no menu
}

// Função principal de loop do Arduino
void loop() {
    // Verifica se o dispositivo está bloqueado
    if (millis() < lockoutUntil) {
        lcd.clear();
        unsigned long remainingTime = (lockoutUntil - millis()) / 1000;
        lcd.print("Bloqueado: ");
        lcd.print(remainingTime);
        lcd.print("s");
        delay(1000);
        return;
    }

    // Gerencia os diferentes estados do menu
    if (menuState == SETUP_PIN) {
        setupPin(); // Configura o PIN
    } else if (menuState == VERIFY_PIN) {
        if (!verifyPin()) {
            menuState = VERIFY_PIN; // Tenta verificar o PIN novamente
        } else {
            menuState = MAIN_MENU; // PIN verificado, vai para o menu principal
            showSelectedOption();
        }
    } else if (menuState == VERIFY_CURRENT_PIN) {
        if (verifyPin()) {
            menuState = MODIFY_PIN; // PIN atual verificado, vai para a modificação do PIN
            setupPin(false, true);
        } else {
            lcd.clear();
            lcd.print("PIN Incorreto!");
            delay(2000);
            menuState = MAIN_MENU; // PIN incorreto, volta para o menu principal
            showSelectedOption();
        }
    } else if (menuState == CONFIRM_NEW_PIN) {
        if (verifyPin(true)) {
            savePinToEEPROM(tempPin); // PIN novo confirmado e salvo na EEPROM
            lcd.clear();
            lcd.print("PIN alterado com sucesso.");
            delay(2000);
            menuState = MAIN_MENU;
            showSelectedOption();
        } else {
            lcd.clear();
            lcd.print("PINs não correspondem, tente novamente.");
            delay(2000);
            menuState = MODIFY_PIN; // PINs não correspondem, tenta novamente
            showSelectedOption();
        }
    } else if (menuState == CONFIRM_RESET) {
        int button = readButtons();
        if (button == btnSelect) {
            executeSelectedOption(); // Executa a opção selecionada
        } else if (button == btnUp || button == btnDown) {
            navigate(button == btnUp ? -1 : 1); // Navega pelas opções do menu
        }
    } else {
        int button = readButtons();

        if (menuState == ACCOUNT_SELECTED && button == btnSelect) {
            menuState = MAIN_MENU;
            lcd.clear();
            showSelectedOption(); // Mostra a opção selecionada no menu principal
        } else {
            switch (button) {
                case btnUp:
                    navigate(-1); // Navega para cima no menu
                    break;
                case btnDown:
                    navigate(1); // Navega para baixo no menu
                    break;
                case btnSelect:
                    executeSelectedOption(); // Executa a opção selecionada
                    break;
            }
        }
        delay(100); // Delay para evitar múltiplas leituras de botões
    }
}
