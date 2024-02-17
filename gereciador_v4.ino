// ######################################### Bibliotecas ############################################
#include <LiquidCrystal.h>
#include <EEPROM.h>


// ################################ Variáveis Globais e Definições ##################################
// Definições dos botões
const int btnUp = 0;
const int btnDown = 1;
const int btnSelect = 4;

// Configurações do LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Variáveis globais
int selectedOption = 0;
int selectedAccount = 0;
unsigned long displayTimeout = 0;
const unsigned long timeoutDuration = 5000; // 5 segundos

const int maxAccounts = 15; // Limite máximo de contas
int numberOfAccounts = 0; // Inicializa como 0

//Opções de contas
String newAccountOptions[] = {"Email", "Instagram", "Computador", "Facebook", "Twitter", "Outro"};
const int newAccountOptionsSize = 6;

// Estados do menu
enum MenuState {
  MAIN_MENU,
  PASSWORDS_MENU,
  DELETE_PASSWORD_MENU, 
  ACCOUNTS_MENU,
  ACCOUNT_SELECTED,
  CREATE_ACCOUNT_MENU
};

MenuState menuState = MAIN_MENU;
MenuState previousMenuState = MAIN_MENU;


// Variáveis para debounce
const int debounceDelay = 50; // Tempo de debounce em milissegundos
int lastButtonState = -1; // Armazena o último estado do botão lido
unsigned long lastDebounceTime = 0; // Última vez que a saída do botão foi alterada

//####################################################################################


// Protótipos de funções
void printWrapped(const String &message);
String getAccountName(int index);
String getAccountPassword(int index);
void showSelectedOption();
void executeSelectedOption();
int readButtons();
void navigate(int direction);
String generateRandomPassword();
int getNextAccountIndex(String accountType);
void writeToEEPROM(int startAddr, const String &data);
String readFromEEPROM(int startAddr, int maxLength);
void loadAccountsFromEEPROM();
//####################################################################################


//############################# Funções Auxiliares ###################################


//Função para ajustar a mensagem no display LCD
void printWrapped(const String &message) {
  lcd.clear();
  // Primeira metade da mensagem
  lcd.setCursor(0, 0);
  lcd.print(message.substring(0, 16));

  // Segunda metade da mensagem, se necessário
  if (message.length() > 16) {
    lcd.setCursor(0, 1);
    lcd.print(message.substring(16, min(32, message.length())));
  }
}

//Função para fazer o debounce dos botões
int readButtons() {
  static unsigned long lastDebounceTime = 0;  // Última vez que o estado do botão mudou
  static int lastButtonState = -1;           // Último estado lido do botão
  static int buttonState = -1;               // Estado atual do botão

  int reading = -1;
  int adc_key_in = analogRead(0);

  if (adc_key_in < 50) reading = btnSelect;
  else if (adc_key_in < 250) reading = btnUp;
  else if (adc_key_in < 450) reading = btnDown;

  // Se o estado atual é diferente do último estado lido
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // Reinicia o temporizador
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se passaram mais de debounceDelay milissegundos desde a última mudança
    // e o estado do botão mudou, atualiza o estado do botão
    if (reading != buttonState) {
      buttonState = reading;

      // Somente retorna um estado válido (diferente de -1) se o botão foi pressionado
      if (buttonState != -1) {
        lastButtonState = reading;  // Salva o último estado do botão
        return buttonState;
      }
    }
  }

  // Salva o último estado do botão
  lastButtonState = reading;
  return -1; // Retorna -1 se nenhum botão válido foi pressionado
}


//############################### Funções para lidar com o Menu ###################################

//Mostra as opções dos menus
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
           }
           break;


        case PASSWORDS_MENU:
          if (numberOfAccounts == 0) {
            printWrapped("Nenhuma senha armazenada");
            displayTimeout = millis() + 3000; // 3 segundos a partir de agora
          } else if (selectedOption == 0) {
            printWrapped("Retornar ao menu inicial");
          } else {
            printWrapped(getAccountName(selectedOption - 1));
          }
          break;

        case DELETE_PASSWORD_MENU:
            if (selectedOption == 0) {
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
                printWrapped(getAccountPassword(selectedAccount));
                displayTimeout = millis() + timeoutDuration;
            }
            break;

        case CREATE_ACCOUNT_MENU:
          if (selectedOption == 0) {
            printWrapped("Retornar ao menu inicial");
          } else if (selectedOption <= newAccountOptionsSize) {
            printWrapped(newAccountOptions[selectedOption - 1]);
          }
          break;

    }
}

//Executa uma opção selecionada no menu
void executeSelectedOption() {
    switch (menuState) {
        case MAIN_MENU:
            if (selectedOption == 0) {
                menuState = CREATE_ACCOUNT_MENU;
                selectedOption = 0;
            } else if (selectedOption == 1) {
                menuState = PASSWORDS_MENU;
                selectedOption = 0;
            } else if (selectedOption == 2) {
                menuState = DELETE_PASSWORD_MENU;
                selectedOption = 0;
            }
            break;

        case CREATE_ACCOUNT_MENU:
          if (selectedOption > 0 && selectedOption <= newAccountOptionsSize) { // Verifica se a opção selecionada é válida para uma conta
            if (numberOfAccounts < maxAccounts) {
              String accountType = newAccountOptions[selectedOption - 1]; // Ajusta o índice subtraindo 1
              int index = getNextAccountIndex(accountType);
              String accountName = accountType + String(index);
              String accountData = accountName + ":" + generateRandomPassword();

              int eepromStartAddr = numberOfAccounts * 64; // Alocando 64 bytes por conta
              writeToEEPROM(eepromStartAddr, accountData);

              numberOfAccounts++;
              menuState = MAIN_MENU;
            } else {
                printWrapped("Limite de contas atingido");
                delay(2000); // Exibe a mensagem por 2 segundos
                menuState = MAIN_MENU;
            }
          } else {
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

    }
    showSelectedOption(); // Atualiza a exibição após a execução de uma opção
}

//função para controlar a navegação no menu
void navigate(int direction) {
  int maxOptions = 0;

  switch (menuState) {
    case MAIN_MENU:
      maxOptions = 3; // Alterado para incluir 3 opções (0, 1, 2)
      break;
    case PASSWORDS_MENU:
    case ACCOUNTS_MENU:
      maxOptions = numberOfAccounts + 1; // Número de contas + opção de retorno
      break;
    case CREATE_ACCOUNT_MENU:
      maxOptions = newAccountOptionsSize + 1; // +1 para incluir a opção de retorno
      break;
    case ACCOUNT_SELECTED:
      // Não há navegação no estado ACCOUNT_SELECTED
      break;
    case DELETE_PASSWORD_MENU:
      maxOptions = numberOfAccounts + 1; // Inclui todas as contas mais a opção de retorno
      break;

  }

  selectedOption = (selectedOption + direction + maxOptions) % maxOptions;
  showSelectedOption();
}

//###############################################################################################################

// ########################################### Gerar Senhas  ####################################################
String generateRandomPassword() {
  String password = "";
  String chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()"; // Inclui letras, números e caracteres especiais

  // Inicializa a semente do gerador de números aleatórios com o tempo de inicialização do dispositivo
  // Usa micros() para uma variação mais fina que millis()
  randomSeed(micros());

  for (int i = 0; i < 32; i++) { // Gera uma senha de 32 caracteres
    int randomIndex = random(0, chars.length()); // Escolhe um índice aleatório dentro da string de caracteres
    char randomChar = chars[randomIndex]; // Seleciona o caractere no índice aleatório
    password += randomChar; // Adiciona o caractere à string da senha
  }

  return password; // Retorna a senha gerada
}


//################### Funções para lidar com as senhas armazenadas na memória EEPROM ###########################

//Função para manejar os indeces das contas
int getNextAccountIndex(String accountType) {
    int highestIndex = 0;
    for (int i = 0; i < numberOfAccounts; i++) {
        String accountData = readFromEEPROM(i * 64, 64);
        int delimiterPos = accountData.indexOf(':');
        if (delimiterPos != -1) {
            String currentType = accountData.substring(0, delimiterPos);
            if (currentType.startsWith(accountType)) {
                int currentTypeIndex = currentType.substring(accountType.length()).toInt();
                if (currentTypeIndex > highestIndex) {
                    highestIndex = currentTypeIndex;
                }
            }
        }
    }
    return highestIndex + 1; // Retorna o próximo índice disponível para o tipo de conta fornecido
}

//Escreve os dados na memória EEPROM do Arduino
void writeToEEPROM(int startAddr, const String &data) {
  int len = data.length();
  EEPROM.put(startAddr, len); // Armazena o tamanho da string
  for (int i = 0; i < len; i++) {
    EEPROM.put(startAddr + 1 + i, data[i]); // Armazena cada caractere
  }
}

//Lê os dados na memória EEPROM do Arduino
String readFromEEPROM(int startAddr, int maxLength) {
  int len;
  EEPROM.get(startAddr, len); // Recupera o tamanho da string
  len = min(len, maxLength); // Limita o tamanho
  String data = "";
  for (int i = 0; i < len; i++) {
    char c;
    EEPROM.get(startAddr + 1 + i, c);
    data += c; // Reconstrói a string
  }
  return data;
}

//Carrega os dados na memória EEPROM do Arduino
void loadAccountsFromEEPROM() {
  numberOfAccounts = 0;
  while (numberOfAccounts < maxAccounts) {
    String accountData = readFromEEPROM(numberOfAccounts * 64, 64);
    if (accountData.length() == 0) break; // Se a string está vazia, não há mais contas
    numberOfAccounts++;
  }
}

//Busca o nome da conta na EEPROM
String getAccountName(int index) {
  String accountData = readFromEEPROM(index * 64, 64); // Lê os dados da conta da EEPROM
  int delimiterPos = accountData.indexOf(':');
  if (delimiterPos != -1) {
    return accountData.substring(0, delimiterPos);
  }
  return ""; // Retorna uma string vazia se o delimitador não for encontrado
}

//Busca a senha da conta na EEPROM
String getAccountPassword(int index) {
  String accountData = readFromEEPROM(index * 64, 64); // Lê os dados da conta da EEPROM
  int delimiterPos = accountData.indexOf(':');
  if (delimiterPos != -1) {
    return accountData.substring(delimiterPos + 1);
  }
  return ""; // Retorna uma string vazia se o delimitador não for encontrado
}

//Deleta contas armazenadas na EEPROM
void deleteAccount(int accountIndex) {
    if (accountIndex < 0 || accountIndex >= numberOfAccounts) {
        // Índice inválido, não faz nada
        return;
    }

    // Desloca todos os dados uma posição para cima
    for (int i = accountIndex; i < numberOfAccounts - 1; i++) {
        String accountData = readFromEEPROM((i + 1) * 64, 64);
        writeToEEPROM(i * 64, accountData);
    }

    // Limpa a última posição
    clearEEPROM((numberOfAccounts - 1) * 64, 64);

    // Atualiza o contador de contas
    numberOfAccounts--;
}


void clearEEPROM(int startAddr, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(startAddr + i, 0);
    }
}

//############################# Setup e Loop ############################################
void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Bem-vindo!");
  delay(2000);
  loadAccountsFromEEPROM(); // Carrega as contas da EEPROM
  showSelectedOption();
}

void loop() {
    int button = readButtons();

    // Verifica se o botão Select é pressionado enquanto uma senha está sendo exibida
    if (menuState == ACCOUNT_SELECTED && button == btnSelect) {
        menuState = MAIN_MENU; // Define explicitamente para retornar ao menu principal
        lcd.clear(); // Limpa a tela do LCD para preparar para a nova exibição
        showSelectedOption(); // Atualiza o display para o menu principal
    } else {
        // Lida com a navegação e seleção normal no menu
        switch (button) {
            case btnUp:
                navigate(-1);
                break;
            case btnDown:
                navigate(1);
                break;
            case btnSelect:
                executeSelectedOption();
                break;
        }
    }

    delay(100); // Pequeno delay para evitar sobrecarga do loop
}
