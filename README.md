# Gerenciador de Senhas com Arduino

Este projeto apresenta um gerenciador de senhas desenvolvido para operar em um Arduino Uno com um shield LCD e uma bateria externa. O principal benefício oferecido pelo gerenciador de senhas é o fato de que as senhas geradas são armazenadas de maneira completamente offline, proporcionando uma opção mais segura em comparação ao armazenamento de senhas em aplicativos online.

## Funcionalidades

- **Mecanismo de autenticação por PIN:** Para acessar as senhas armazenadas, o usuário deve definir um PIN na primeira inicialização. Esse PIN será usado para autenticação nas próximas vezes que o dispositivo for acessado. Além disso, o usuário pode redefinir o PIN, caso esteja autenticado previamente.
- **Gerenciamento de senhas:** O usuário pode definir um nome para cada conta, gerar senhas de 32 caracteres, visualizar as senhas geradas e deletar aquelas que não são mais necessárias.
- **Limpeza total da memória:** Se desejado, o usuário pode resetar o dispositivo para seu estado inicial, apagando todas as senhas armazenadas e redefinindo o PIN de autenticação.

## Instruções para Upload do Código

### 1. Instalar a Arduino IDE
Baixe a Arduino IDE do [site oficial](https://www.arduino.cc/en/software) e siga as instruções de instalação para o seu sistema operacional.

### 2. Copiar ou Clonar o Repositório do GitHub
- **Cópia Manual:** Acesse o repositório (https://github…) e copie cada um dos arquivos do projeto.
- **Clonagem com Git:** Caso esteja familiarizado com o GitHub e/ou o utilitário git, abra o terminal, navegue até o diretório onde deseja salvar o código e execute o comando:
    ```sh
    git clone https://github.com/username/repository.git
    ```
    Esse comando irá baixar o código seguindo a estrutura do repositório.

### 3. Abrir o Código na Arduino IDE
Abra a Arduino IDE e selecione `File > Open` na barra superior, navegue até o diretório clonado e abra o arquivo principal `main.ino`.

### 4. Selecionar a Placa e a Porta
- No menu `Tools` (Ferramentas), selecione a placa **Arduino Uno** em `Board`.
- Ainda no menu `Tools`, clique em `Port` e selecione a entrada à qual o Arduino está conectado.

### 5. Fazer o Upload do Código
- Clique no botão de upload (ícone de seta para a direita) para compilar e carregar o código na placa.
- A Arduino IDE compilará o código e, se não houver erros, fará o upload automaticamente para a placa conectada.
- Após o upload, o código começará a ser executado imediatamente no Arduino.

## Contribuições
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e enviar pull requests para melhorias e correções.

## Licença
Este projeto está licenciado sob a MIT License - veja o arquivo LICENSE para mais detalhes.


