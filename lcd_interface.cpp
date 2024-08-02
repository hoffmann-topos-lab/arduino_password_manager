#include "lcd_interface.h"
#include <Arduino.h>  
#include "lcd_interface.h"
#include "menu.h"     



// Inicializa o objeto LiquidCrystal com os pinos conectados ao display LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Função para inicializar o LCD
void initializeLCD() {
    lcd.begin(16, 2);  // Configura o LCD para 16 colunas e 2 linhas
    lcd.clear();       // Limpa o display LCD
    lcd.print("Bem-vindo!"); // Exibe a mensagem de boas-vindas
    delay(2000);  // aguarda dois segundos
}

// Função para imprimir uma mensagem no LCD com quebra de linha automática
void printWrapped(const String &message) {
    lcd.clear(); // Limpa o display LCD
    lcd.setCursor(0, 0); // Posiciona o cursor no início da primeira linha
    lcd.print(message.substring(0, 16)); // Imprime até 16 caracteres na primeira linha
    if (message.length() > 16) { // Se a mensagem tiver mais de 16 caracteres
        lcd.setCursor(0, 1);  // Posiciona o cursor no início da segunda linha
        lcd.print(message.substring(16, min(32, message.length()))); // Imprime até mais 16 caracteres
    }
}

// Função para ler o estado dos botões
int readButtons() {
  static unsigned long lastDebounceTime = 0;  // Última vez que o estado do botão mudou
  static int lastButtonState = -1;           // Último estado lido do botão
  static int buttonState = -1;               // Estado atual do botão
  int reading = -1; // Variável para armazenar a leitura do botão
  int adc_key_in = analogRead(0); // Lê o valor analógico do pino 0
  // Ajuste dos valores para os botões mapeados
  if (adc_key_in < 50) {
    reading = btnRight;
  } else if (adc_key_in < 150) {
    reading = btnUp;
  } else if (adc_key_in < 300) {
    reading = btnDown;
  } else if (adc_key_in < 450) {
    reading = btnLeft;
  } else if (adc_key_in < 650) {
    reading = btnSelect;
  }
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

// Função para mostrar a opção de confirmação no LCD
void showConfirmationOption() {
    lcd.clear(); // Limpa o display LCD
    lcd.print("Deseja resetar");
    lcd.setCursor(0, 1);
    lcd.print("o dispositivo?");
    lcd.setCursor(15, 1);
    lcd.print((selectedOption == 0) ? "S" : "N");
}