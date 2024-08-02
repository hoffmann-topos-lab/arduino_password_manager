#include "menu.h"
#include <Arduino.h>  
#include "password_generator.h"
#include "eeprom_manager.h"


extern const int btnRight;
extern const int btnUp;
extern const int btnDown;
extern const int btnLeft;
extern const int btnSelect;

extern const unsigned long debounceDelay;

extern const int maxAccounts;
extern int numberOfAccounts;

extern String tempPin; 



// Variáveis globais
int selectedOption = 0;
int selectedAccount = 0;
unsigned long displayTimeout = 0;
const unsigned long timeoutDuration = 5000; // 5 segundos


// Variáveis globais para o controle de tentativas e bloqueio
int pinAttempts = 0; // Contador de tentativas de PIN incorretas
unsigned long lockoutUntil = 0; // Tempo até o qual o dispositivo está bloqueado
unsigned long lockoutDuration = 30000; // Duração inicial do bloqueio em milissegundos

// Variáveis para debounce
int lastButtonState = -1; // Armazena o último estado do botão lido
unsigned long lastDebounceTime = 0; // Última vez que a saída do botão foi alterada



bool isModification = false;

MenuState menuState = MAIN_MENU;
MenuState previousMenuState = MAIN_MENU;

void showSelectedOption() {
    lcd.clear();
    lcd.setCursor(0, 0);

    switch (menuState) {
       case MAIN_MENU:
          if (selectedOption == 0) {
            printWrapped("Adicionar Nova Senha");
          } else if (selectedOption == 1) {
              printWrapped("Senhas Registradas");
          } else if (selectedOption == 2) {
              printWrapped("Deletar Senha");
          } else if (selectedOption == 3) {
              printWrapped("Modificar PIN"); 
          } else if (selectedOption == 4) {
              printWrapped("Resetar Dispositivo");
          }
          break;

        case PASSWORDS_MENU:
            if (numberOfAccounts == 0) {
                printWrapped("Nenhuma senha armazenada");
                delay(5000); // 5 segundos de atraso
                menuState = MAIN_MENU; // Retorna ao menu inicial
                showSelectedOption(); // Atualiza o display para o menu inicial
            } else if (selectedOption == 0) {
                printWrapped("Retornar ao menu inicial");
            } else {
                printWrapped(getAccountName(selectedOption - 1));
            }
            break;

        case DELETE_PASSWORD_MENU:
            if (numberOfAccounts == 0) {
                printWrapped("Nenhuma senha armazenada");
                delay(5000); // 5 segundos de atraso
                menuState = MAIN_MENU; // Retorna ao menu inicial
                showSelectedOption(); // Atualiza o display para o menu inicial
            } else if (selectedOption == 0) {
                printWrapped("Retornar ao menu inicial");
            } else {
                printWrapped(getAccountName(selectedOption - 1));
            }
            break;

        case ACCOUNTS_MENU:
            if (selectedOption == 0) {
                printWrapped("Retornar ao menu inicial");
            } else {
                printWrapped(getAccountName(selectedOption - 1));
            }
            break;

        case ACCOUNT_SELECTED:
            if (selectedOption == 0) {
                printWrapped("Retornar ao menu anterior");
            } else {
                String password = getAccountPassword(selectedAccount);
                lcd.clear();
                lcd.setCursor(0, 0);
                if (password.length() > 16) {
                    lcd.print(password.substring(0, 16));
                    lcd.setCursor(0, 1);
                    lcd.print(password.substring(16));
                } else {
                    lcd.print(password);
                }
                displayTimeout = millis() + timeoutDuration;
            }
            break;

        case CREATE_ACCOUNT_MENU:
             printWrapped("Escolha o nome da conta");
            delay(1000);
            menuState = ENTER_ACCOUNT_NAME;
            showSelectedOption();
            break;

        case ENTER_ACCOUNT_NAME:
            getNewAccountName(); // Chama a função para definir o nome da conta
            menuState = MAIN_MENU; // Retorna ao menu principal após definir o nome
            showSelectedOption();
            break;

        case VERIFY_CURRENT_PIN:
            printWrapped("Digite o PIN atual:");
            break;

        case MODIFY_PIN:
            printWrapped("Novo PIN:");
            break;

        case CONFIRM_NEW_PIN:
            printWrapped("Confirme o novo PIN:");
            break;

        case CONFIRM_RESET:
            showConfirmationOption();
            break;

        default:
            break;

        }
}

void executeSelectedOption() {
    switch (menuState) {
        case MAIN_MENU:
            if (selectedOption == 0) {
                printWrapped("Escolha o nome da conta");
                delay(2000); // Exibe a mensagem por 2 segundos
                menuState = CREATE_ACCOUNT_MENU;
                selectedOption = 0;
            } else if (selectedOption == 1) {
                menuState = PASSWORDS_MENU;
                selectedOption = 0;
            } else if (selectedOption == 2) {
                menuState = DELETE_PASSWORD_MENU;
                selectedOption = 0;
            }else if (selectedOption == 3) {
                menuState = VERIFY_CURRENT_PIN; 
                selectedOption = 0;
            }else if (selectedOption == 4) {
                menuState = CONFIRM_RESET;
                selectedOption = 0;
            }
            break;

        case CREATE_ACCOUNT_MENU:
            if (numberOfAccounts < maxAccounts) {
                String accountName = getNewAccountName();
                // O nome da conta já foi verificado e salvo dentro de getNewAccountName
                menuState = MAIN_MENU;
            } else {
                printWrapped("Limite de contas atingido");
                delay(2000); // Exibe a mensagem por 2 segundos
                menuState = MAIN_MENU;
            }
            break;

        case PASSWORDS_MENU:
            if (selectedOption == 0) {
                menuState = MAIN_MENU;
            } else {
                menuState = ACCOUNT_SELECTED;
                selectedAccount = selectedOption - 1;
            }
            break;

        case DELETE_PASSWORD_MENU:
            if (selectedOption > 0) {
                deleteAccount(selectedOption - 1);
                if (selectedOption >= numberOfAccounts && numberOfAccounts > 0) {
                    selectedOption = numberOfAccounts; // Ajusta para a última conta válida
                }
            }
            menuState = MAIN_MENU; // Independentemente da opção selecionada, retorna ao MAIN_MENU
            break;

        case ACCOUNTS_MENU:
            if (selectedOption == 0) {
                menuState = MAIN_MENU;
            } else {
                selectedAccount = selectedOption - 1;
                menuState = ACCOUNT_SELECTED;
            }
            break;

        case ACCOUNT_SELECTED:
            if (selectedOption == 0) {
                menuState = (previousMenuState == PASSWORDS_MENU) ? PASSWORDS_MENU : ACCOUNTS_MENU;
            }
            // Aqui você pode adicionar lógica adicional para quando uma conta é selecionada
            break;

        case VERIFY_CURRENT_PIN:
            if (verifyPin()) {
                menuState = MODIFY_PIN;
                setupPin(false, true); // Passa 'true' para isModification para mostrar "Defina o novo PIN"
            } else {
                printWrapped("PIN incorreto, tente novamente.");
                delay(2000);
                menuState = MAIN_MENU;
            }
            break;

        case MODIFY_PIN:
            setupPin(true, true); // Configura o novo PIN e exibe a mensagem correta
            break;

        case CONFIRM_NEW_PIN:
            if (verifyPin(true)) { // Verifica o novo PIN para confirmação
                savePinToEEPROM(tempPin);
                printWrapped("PIN alterado.");
                delay(2000);
                menuState = MAIN_MENU;
            } else {
                printWrapped("PINs não correspondem, tente novamente.");
                delay(2000);
                menuState = MODIFY_PIN;
            }
            break;

        case CONFIRM_RESET:
            if (selectedOption == 0) {
                clearEEPROM(0, EEPROM.length());
                lcd.clear();
                lcd.print("EEPROM limpa!");
                delay(2000);
                numberOfAccounts = 0;
                lockoutDuration = 30000;
                menuState = SETUP_PIN;
                setupPin();
            } else {
                menuState = MAIN_MENU;
            }
            break;

        default:
            break;
    }
    showSelectedOption(); // Atualiza a exibição após a execução de uma opção
}

void navigate(int direction) {
    int maxOptions = 0;

    switch (menuState) {
        case MAIN_MENU:
            maxOptions = 5; // Número de opções no menu principal
            break;
        case PASSWORDS_MENU:
        case ACCOUNTS_MENU:
            maxOptions = numberOfAccounts + 1; // Número de contas + opção de retorno
            break;
        case CREATE_ACCOUNT_MENU:
            maxOptions = 1; // Apenas a opção de retornar ao menu inicial
            break;
        case ACCOUNT_SELECTED:
            // Não há navegação no estado ACCOUNT_SELECTED
            return;
        case DELETE_PASSWORD_MENU:
            maxOptions = numberOfAccounts + 1; // Inclui todas as contas mais a opção de retorno
            break;
        case CONFIRM_RESET:
            maxOptions = 2; // S e N
            break;
    }

    selectedOption = (selectedOption + direction + maxOptions) % maxOptions;

    if (menuState == CONFIRM_RESET) {
        showConfirmationOption();
    } else {
        showSelectedOption();
    }
}


// Adicione uma função para resetar o tempo de bloqueio se necessário
void resetLockout() {
    int lockoutEndTime = 0;
    int lockoutDuration = 30000;
}

String getNewAccountName() {
    String accountName = "";
    int charIndex = 0;
    char currentChar = 'A';
    lcd.clear();
    lcd.print("Nome da Conta:");
    lcd.setCursor(0, 1);
    lcd.print(currentChar);

    while (true) {
        int button = readButtons();

        if (button == btnUp) {
            if (currentChar == '9') {
                currentChar = 'A';
            } else if (currentChar == 'Z') {
                currentChar = '0';
            } else {
                currentChar++;
            }
        } else if (button == btnDown) {
            if (currentChar == '0') {
                currentChar = 'Z';
            } else if (currentChar == 'A') {
                currentChar = '9';
            } else {
                currentChar--;
            }
        } else if (button == btnRight) {
            accountName += currentChar;
            charIndex++;
            if (charIndex < 7) {
                currentChar = 'A'; // Reseta para 'A' após selecionar um caractere
                lcd.setCursor(charIndex, 1);
                lcd.print(currentChar);
            } else {
                break; // Confirma automaticamente após o 7º caractere
            }
        } else if (button == btnLeft) {
            if (charIndex > 0) {
                charIndex--;
                accountName.remove(charIndex);
                currentChar = 'A'; // Reseta para 'A' após remover um caractere
                lcd.setCursor(charIndex, 1);
                lcd.print(' '); // Limpa o caractere atual no display
                lcd.setCursor(charIndex, 1);
                lcd.print(currentChar);
            }
        } else if (button == btnSelect) {
            if (accountName.length() > 0) {
                if (!accountNameExists(accountName)) {
                    // Nome não existe, gerar senha e salvar
                    String accountData = accountName + ":" + generateRandomPassword();
                    int eepromStartAddr = numberOfAccounts * 64; // Alocando 64 bytes por conta
                    writeToEEPROM(eepromStartAddr, accountData);
                    writeAccountNameToEEPROM(numberOfAccounts, accountName); 
                    numberOfAccounts++;
                    return accountName;
                } else {
                    // Nome já existe, mostrar mensagem de erro
                    lcd.clear();
                    lcd.print("Nome ja existe");
                    delay(2000);
                    // Limpar e resetar para tentar novamente
                    accountName = "";
                    charIndex = 0;
                    currentChar = 'A';
                    lcd.clear();
                    lcd.print("Nome da Conta:");
                    lcd.setCursor(0, 1);
                    lcd.print(currentChar);
                }
            }
        }

        // Atualiza o display com o caractere atual
        lcd.setCursor(charIndex, 1);
        lcd.print(currentChar);

        delay(100); // Pequeno atraso para debouncing
    }

    return accountName;
}