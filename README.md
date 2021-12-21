PlantaIoT
Projeto utilizando a placa Edukit Redfox

O cultivo de plantas saudáveis exige cuidados constantes que nem sempre podem ser mantidos. Esse projeto de monitoramento de plantas IoT almeja prover, de forma automática e monitoramento remoto, alguns cuidados básicos como a irrigação e o fornecimento de luz em volumes suficientes para o cultivo apropriado de plantas em pequenos espaços como vasos e canteiros, encontrando aplicações em domótica, hortas, fazendas urbanas e cidades inteligentes.

Componentes:

HARDWARE

Placa Edukit Redfox (Duodigit) + Arduino Nano x 1:
-	função: ler o sensor de umidade, controlar o acionamento da bomba d 'água / luz e comandar o SiP Sigfox da placa serialmente via comandos AT, transmitindo os dados para o backend Sigfox.

Sensor de Luz LDR (Light Dependent Resistor) × 1:
-	função: detectar a luminosidade do ambiente.

Módulo MOSFET IRF520N × 2:
-	função: comandar cargas que necessitam de corrente e tensão maiores que as fornecidas pelo Arduino Nano como a bomba d’água e a lâmpada LED.

Mini Conversor de Tensão DC-DC Step Down × 1:
-	função: converter a tensão para alimentar a lâmpada LED.

Sensor Capacitivo de Umidade do Solo × 1:
-	função: medir a umidade do solo no vaso.

Bomba d'água submersível 3-6V 100L/H × 1:
-	função:  bombear a água do reservatório.
 
Sensor de Nível de Água horizontal × 1:
-	função: determinar a presença ou a falta d’água no reservatório.

Reservatório de água para 5 litros:
-	função: armazenar a água para regar o vaso. 

Aspersor:
-	função: regar o vaso com água.

Lâmpada LED:
-	função: fornecer luz artificial quando a luz natural do ambiente não for suficiente (pelo menos 5 horas diárias).

Fonte de alimentação de 9V x 1A:
-	função: fornecer alimentação para o Arduino Nano, conversor de tensão e módulos MOSFET. A alimentação da placa Edukit Redfox / SiP é interna através do pino de 5V do Arduino Nano.
 
SOFTWARE

Arduino IDE + biblioteca da placa Edukit Redfox modificada para o projeto.
-	função: executar as funções de leitura dos sensores armazenando os dados em variáveis, transmitir dos dados de payload para o backend Sigfox, acionar a bomba e a luz quando necessário.

Sigfox backend 
-	função: receber os dados dos dispositivos pela rede LPWAN (Low-Power Wide-Area Network) Sigfox.

TagoIO
- função: plataforma IoT que recebe os dados em formato JSON do backend Sigfox, exibindo-os graficamente.


Mais detalhes sobre esta implementação:

https://drive.google.com/file/d/1cJEePLoJAt0aTmvFVHt-Wwz2sbts0gME/view?usp=sharing

autor: Mauricio Santos Pupo

