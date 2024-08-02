#include "eeprom_manager.h"
#include <Arduino.h>  
#include "password_generator.h"



// ############# Variáveis  ##################

int numberOfAccounts = 0; // Número atual de contas armazenadas
const int maxAccounts = 15; // Número máximo de contas que podem ser armazenadas


// ########### Funções ###################

//Escreve os dados na memória EEPROM do Arduino
void writeToEEPROM(int startAddr, const String &data) {
    int len = data.length();
    EEPROM.write(startAddr, len); // Armazena o tamanho da string
    for (int i = 0; i < len; i++) {
        EEPROM.write(startAddr + 1 + i, data[i]); // Armazena cada caractere
    }

}

//Lê os dados na memória EEPROM do Arduino
String readFromEEPROM(int startAddr, int maxLength) {
    int len = EEPROM.read(startAddr); // Recupera o tamanho da string
    len = min(len, maxLength); // Limita o tamanho
    String data = "";
    for (int i = 0; i < len; i++) {
        char c = EEPROM.read(startAddr + 1 + i);
        data += c; // Reconstrói a string a partir dos caracteres armazenados
    }
    return data;
}


//Carrega os dados das contas na memória EEPROM 
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
        return accountData.substring(0, delimiterPos); // Retorna o nome da conta
    }
    return ""; // Retorna uma string vazia se o delimitador não for encontrado
}


//Busca a senha da conta na EEPROM
String getAccountPassword(int index) {
    String accountData = readFromEEPROM(index * 64, 64); // Lê os dados da conta da EEPROM
    int delimiterPos = accountData.indexOf(':');
    if (delimiterPos != -1) {
        String password = accountData.substring(delimiterPos + 1);  // Extrai a senha da conta
        return password;
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

// Função para limpar a EEPROM
void clearEEPROM(int startAddr, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(startAddr + i, 0); // Escreve zero em todos os bytes da área especificada
    }
}

// Função para salvar o PIN na EEPROM
void savePinToEEPROM(const String &pin) {
    int startAddr = 1000; // Endereço específico na EEPROM para o PIN
    for (int i = 0; i < pin.length(); i++) {
        EEPROM.write(startAddr + i, pin[i]);
    }
}

// Função para ler o PIN da EEPROM
String readPinFromEEPROM() {
    int startAddr = 1000; // Endereço específico na EEPROM para o PIN
    String pin = "";
    for (int i = 0; i < 4; i++) {
        char c = EEPROM.read(startAddr + i);
        if (c >= '0' && c <= '9') {
            pin += c; // Adiciona o caractere ao PIN se for um dígito válido
        } else {
            return ""; // Se não for um dígito válido, considera que não há PIN definido
        }
    }
    return pin;
}

// Função para escrever o nome da conta na EEPROM
void writeAccountNameToEEPROM(int index, const String &name) {
    int startAddr = 2000 + (index * 10); // Endereço específico para o nome da conta
    for (int i = 0; i < name.length(); i++) {
        EEPROM.write(startAddr + i, name[i]);
    }
    EEPROM.write(startAddr + name.length(), '\0'); // Null terminator para indicar o fim da string
}

// Função para obter o próximo índice disponível para um tipo de conta específico
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

// Função para verificar se o nome da conta já existe
bool accountNameExists(const String &name) {
    for (int i = 0; i < numberOfAccounts; i++) {
        String existingName = getAccountName(i);
        if (existingName == name) {
            return true; // Retorna true se o nome da conta já existe
        }
    }
    return false; // Retorna false se o nome da conta não foi encontrado
}