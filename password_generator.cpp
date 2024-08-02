#include "password_generator.h"

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
