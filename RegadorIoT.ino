//************************************************
// Programa default da placa EDUKit Versão 1.0   *
// Se comunica com Monitor Serial para Debug     *
// As práticas 1 a 4 utilizam esse programa      *
// Enviando as variáveis Contador e Temperatura  *
// Toda vez que o botão SW1 da placa EDUKit for  * 
// pressionada o Contador é incrementado de 1  e *
// a Temperatura é incrementada de 0,33          *
//************************************************

// Modificado por Mauricio Santos Pupo para o projeto "Regador de plantas IoT" - Jornada do Desenvolvimento


#define RESET 4     // Reset no HT32SX
#define TX 2        // Serial TX 
#define RX 3        // Serial RX
#define LED 13      // Led no Arduino Nano
#define SW1 5       // Botão na Placa EDUKIt
#define BOMBA 8    // saida para bomba de agua D8
#define LUZ  7     // saida para luz da planta D7
#define LDR 6      // entrada do sensor de luz D6
#define NIVEL 9    // entrada do sensor de nivel D9

#define sensor A0  // sensor de umidade capacitivo

#include <SoftwareSerial.h>       // Biblioteca de Serial por Software
SoftwareSerial serial_HT(RX, TX); // Cria serial para conectar no HT32SX

// parametros de leitura do sensor de umidade
int ValorAnalog = 0; // armazena valor do sensor
int MaxCalib = 255;  // limite maximo de leitura do sensor
int MaxUmidade = 313; // valor maximo de leitura do sensor (calibracao)
int MinUmidade = 660; // valor minimo de leitura do sensor (calibracao)
int16_t UmidadePercent = 0; // armazena valor do sensor em porcentagem do intervalo de calibracao
int16_t umidade = 0; // valor retornado pela funcao de leitura do sensor
int UmidadeCalib = 0; // valor de incremento de 10 valores de leitura para efetuar media
bool sensorLuz;
bool sensorNivel;

int second=0; // variavel que recebe os segundos decorridos
int minute=0; // variavel que recebe os minutos decorridos
int totalMin = 0; // tempo total decorrido em minutos
int totalMin1 = 0; // variavel auxiliar que mantem o tempo decorrido duante o ciclo de 720 minutos (12 horas)
int totalMinEscuro = 0; // variavel que armazena minutos de pouca luminosidade
int totalMinClaro = 0; // variavel que amazena minutos de claridade
int hora=0; // armazena as horas decorridas
bool vazio; // armazena o estado do nivel do reservatorio de agua (cheio / vazio)
unsigned long lastTick = 0; // variavel auxiliar de contagem de tempo
unsigned long extra_delay = 0; // variavel auxiliar para calculo de compensacao de tempo
unsigned long extra_delayFinal = 0;  // variavel auxiliar para calculo de compensacao de tempo
unsigned long extra_delayInicial = 0;  // variavel auxiliar para calculo de compensacao de tempo


int Contador;            // Variável Contador **modificado para conter o estado da bomba de agua**
                         // Toda vez que SW1 for acionado adiciona 1 ao 
                         // Contador 
int16_t Temperatura;     // Variável Temperatura unidade 0,01 Graus 
                         // Centigrados Representamos com numero 
                         // inteiro múltiplo com múltiplo de 100
                         // Toda Vez que SW1 for acionado adiciona 
                         // 33 (equivalente a 0,33 graus) a Temperatura

void setup() {             // Setup inicial
digitalWrite(RESET, HIGH); // ativa Reset  
pinMode(RESET, OUTPUT);    // configura o pino digital RESET como saida.
Serial.begin(9600);        // Abre Serial de Hardware em 9600 bps
                           // Conectada no Monitor Serial (PC)
serial_HT.begin(9600);     // Abre Serial de Software em 9600 bps
                           // Conectada no HT32SX
                           
//Escreve no Monitor Serial Menu de opções de comandos
Serial.println("**************************************************");
Serial.println("* Menu de opções de comandos, tecle de 1 a 6     *");
Serial.println("* 1 -  RESET HT32SX por comando AT               *");
Serial.println("* 2 -  RESET HT32SX por hardware                 *");
Serial.println("* 3 -  Configura para operar em RCZ2             *");
Serial.println("* 4 -  Envia dados (UPLink)                      *");
Serial.println("* 5 -  Envia dados  e aguarda  (Downlink)        *");
Serial.println("* 6 -  Lê sensor de umidade                      *");
Serial.println("*                                                *");
Serial.println("* OBS nas opções 4 e 5 aguardar 35 segundos ou   *");
Serial.println("* até o LED Apagar para envio de novos comandos  *");
Serial.println("**************************************************");
reset_HT();                // Gera o Reset de hardware no HT32SX
delay(8000);               //delay 8 segundos
Serial.println("Vai enviar Configuração AT+CFGRCZ=2"); 
serial_HT.print("AT+CFGRCZ=2;"); // Configura HT32SX para Região RCZ2

pinMode(SW1, INPUT_PULLUP);
pinMode(LDR, INPUT);
pinMode(NIVEL, INPUT_PULLUP);
pinMode(sensor, INPUT);
pinMode(LED, OUTPUT);
pinMode(BOMBA, OUTPUT);
pinMode(LUZ, OUTPUT);

// Leitura inicial do sensor de umidade
Serial.println("Lendo o sensor de umidade... ");
int16_t umidade = calibraSensor();

 sensorNivel = digitalRead(NIVEL);
 Serial.print ("Sensor de nível: ");
 Serial.println (sensorNivel);
 if (sensorNivel) {
    vazio = false;
    Serial.println("Cheio"); 
 } else {
    vazio = true;
    Serial.println("Vazio");
 }
 
 // Leitura inicial do sensor de luz 
  sensorLuz = digitalRead(LDR);
  Serial.print ("Sensor de luz: ");
  Serial.println (sensorLuz);
  if (sensorLuz) {
    Serial.println("Escuro"); 
 } else {
    Serial.println("Claro");
 }


} // Fim da Configuração.

// funcao para leitura do sensor de umidade
int16_t calibraSensor()
{
  int cont = 0;
  int UmidadeCalib = 0;
  
  do {
  ////Le o valor do pino A0 do sensor
  ValorAnalog = analogRead(sensor);
  Serial.print("Sensor de Umidade: "); 
  Serial.println(ValorAnalog);
  UmidadeCalib = UmidadeCalib + ValorAnalog;
  cont++;
  delay(1000);
  }while(cont < 10);

int ValorMedio = UmidadeCalib/10; 

  UmidadePercent = map(ValorMedio, MaxUmidade, MinUmidade, 100, 0);
  Serial.println("//////////////"); 
  Serial.print("Sensor de Umidade: "); 
  Serial.println(ValorMedio);
  Serial.print("Umidade(%): "); 
  Serial.println(UmidadePercent);
  Serial.println("//////////////");
  //delay(2000);
  
/////////////////////////////////
  return UmidadePercent;
}

void loop() {            // Loop principal

char buf[36];            // Buffer usado para conversão de String.
char c;                  // Variável auxiliar
// Inicializa variáveis

Contador = 0;
Temperatura = 1000;
hora = 0;
minute = 0;

lastTick = 0; 

while(hora < 2) {

// contabiliza segundos
   if (millis() - lastTick >= 1000) {
      Serial.print("Temporizacao: ");
      Serial.print(hora);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.println(second);
      lastTick = millis();
      second++;
       }
            
 
// contabiliza os minutos
    if (second > 59) {
            minute++;
            second = 0;
            totalMin = totalMin1 + minute;
            Serial.print("TotalMinutes: ");
            Serial.println(totalMin);
           
            sensorLuz = digitalRead(LDR);
            Serial.print ("Sensor de luz: ");
            Serial.println (sensorLuz);
           
// contabiliza os minutos em claro e escuro           
            if (sensorLuz) {
            Serial.println("Escuro");
            totalMinEscuro = totalMin - totalMinClaro; // total de minutos com falta de luz
        
            } else {
            Serial.println("Claro");
            totalMinClaro = totalMin - totalMinEscuro; // total de minutos com claridade
            }

            Serial.print("TotalMinutesEscuro: ");
            Serial.println (totalMinEscuro);
            Serial.print("TotalMinutesClaro: ");
            Serial.println (totalMinClaro);
            
 // condicoes para acionamento de luz (5 horas diarios)  
    if((sensorLuz) && (totalMinClaro < 300) && (totalMin != totalMinEscuro)) {
               digitalWrite(LUZ, HIGH);
                  } else {
               digitalWrite(LUZ, LOW);
                  }     
       }
      
 // contabiliza as horas      
    if (minute > 59){
            hora++;
            totalMin1 = totalMin;
            Serial.print("TotalMinutes: ");
            Serial.println(totalMin1);
            minute = 0;
       }  



// leitura de comandos do menu pela porta serial             
    if (serial_HT.available()) {
      c = serial_HT.read(); 
      Serial.print(c);
    } // serial HT
    if (Serial.available()) {
      c = Serial.read();

// executa comandos do menu
      switch (c) {
        case '1':
           Serial.println("Vai Executar AT+RESET;");
           serial_HT.print("AT+RESET;"); 
           break;

        case '2':
           Serial.println("Vai Executar RESET de Hardware");
           reset_HT();
           break;

        case '3':
           Serial.println("Vai enviar Configuração AT+CFGRCZ=2");
           serial_HT.print("AT+CFGRCZ=2;");
           break;

        case '4':
           extra_delayInicial = millis();
           Serial.println("Vai enviar Mensagem SIGFOX");
          
           // envia estado da bomba, nivel e umidade
           sprintf(buf, "AT+SEND=0:%02x%02x%04x;", Contador, vazio, UmidadePercent); 
           Serial.println (buf);
           serial_HT.print (buf);
           delay_msg();
           extra_delayFinal = millis() - extra_delayInicial;
           Serial.print("segundos extras... ");
           Serial.println(extra_delayFinal / 1000);
           second = second + extra_delayFinal / 1000;
           if(second > 59) {
           second = second - 60;
           minute++;
           }
           break;

        case '5':
           extra_delayInicial = millis();
           Serial.println("Vai enviar Mensagem SIGFOX e esperar Downlink");
           
           // envia estado da bomba, nivel e umidade
           sprintf(buf, "AT+SEND=1:%02x%02x%04x;", Contador, vazio, UmidadePercent);
           Serial.println (buf);
           serial_HT.print (buf);
           delay_msg();
           extra_delayFinal = millis() - extra_delayInicial;
           Serial.print("segundos extras... ");
           Serial.println(extra_delayFinal / 1000);
           second = second + extra_delayFinal / 1000;
           if(second > 59) {
           second = second - 60;
           minute++;
           }
           break;

        case '6':
            extra_delayInicial = millis();
            Serial.println("Lendo o sensor de umidade... ");
            umidade = calibraSensor();
            if ((umidade <= 65) && (!vazio)){
            digitalWrite(BOMBA, HIGH);
            Contador = 1;
            Serial.print ("Liga bomba! ");
            Serial.println (Contador);
            delay(3000);
            digitalWrite(BOMBA, LOW);
      }
           // contabiliza o tempo decorrido na operacao para compensar na contagem
           extra_delayFinal = millis() - extra_delayInicial;
           Serial.print("segundos extras... ");
           Serial.println(extra_delayFinal / 1000);
           second = second + extra_delayFinal / 1000;
           if(second > 59) {
            second = second - 60;
            minute++;
           }
           break;
        
      } // Fim do switch
      
   
     }// serial PC

extra_delayInicial = millis();

// verifica se a chave da placa foi pressionada
int sensorVal = digitalRead(5);

  if (sensorVal == HIGH) {  // Botão não pressionado
    digitalWrite(LED, LOW);
  } else {                  // Botão pressionado
    
    digitalWrite(LED, HIGH);
    Contador++;
    Temperatura += 33;   // Soma 0,33 na Temperatura Final
    Serial.println("Botão Pressionado, vai enviar Mensagem SIGFOX");
    sprintf(buf, "AT+SEND=0:%02x%02x%04x;", Contador, vazio, UmidadePercent);
    Serial.println (buf);
    serial_HT.print (buf);  
    delay_msg();
    while (!digitalRead(SW1))
     {
      delay(10);
     } // fim do while botão   
     }
     
    }// fim do while() 

    

  // verifica o nivel do reservatorio de agua
  sensorNivel = digitalRead(NIVEL);
  Serial.print ("Sensor de nível: ");
  Serial.println (sensorNivel);
  if (sensorNivel) {
    vazio = false;
    Serial.println("Cheio"); 
 } else {
    vazio = true;
    Serial.println("Vazio");
 }
 
 // faz a leitura do sensor de umidade e aciona a bomba de agua se menos que 65% 
 umidade = calibraSensor();
 if ((umidade <= 65) && (!vazio)){
      digitalWrite(BOMBA, HIGH);
      Serial.print ("Liga bomba! ");
      Contador = 1;
      Serial.println (Contador);
      delay(5000);
      digitalWrite(BOMBA, LOW);
      }
 
// faz a leitura do sensor de luz
  sensorLuz = digitalRead(LDR);
  Serial.print ("Sensor de luz: ");
  Serial.println (sensorLuz);

  // condicoes para acionamento de luz (5 horas diarios) 
  if((sensorLuz) && (totalMinClaro < 300) && (totalMin != totalMinEscuro)) {
    digitalWrite(LUZ, HIGH);
  } else {
    digitalWrite(LUZ, LOW);
  }

    
     Serial.println("Vai enviar Mensagem SIGFOX");
     // envia estado da bomba, nivel e umidade
     sprintf(buf, "AT+SEND=0:%02x%02x%04x;", Contador, vazio, UmidadePercent);
     Serial.println (buf);
     serial_HT.print (buf);
     delay_msg();

     extra_delayFinal = millis() - extra_delayInicial;
     Serial.print("segundos extras... ");
     Serial.println(extra_delayFinal / 1000);
     second = second + extra_delayFinal / 1000;

     if(second > 59) {
     second = second - 60;
        minute++;
       }

    // reinicia a contagem do tempo apos 12 horas
    if((totalMin >= 720) || (totalMinClaro == 0)) {
      Serial.println("Reset e envia configuração AT+CFGRCZ=2");
      reset_HT();                // Gera o Reset de hardware no HT32SX
     
      delay(8000);               //delay 8 segundos 
      serial_HT.print("AT+CFGRCZ=2;"); // Configura HT32SX para Região RCZ2
      
    Serial.println("12 horas ou totalMinClaro = 0...reiniciando a contagem !");
    totalMin = 0; 
    totalMin1 = 0;  
    totalMinClaro = 0;
    totalMinEscuro = 0;
            
            }
           
}// fim do Loop()

//Função de RESET do HT32SX por Hardware
void reset_HT() {          
  digitalWrite(RESET, HIGH); // ativa Reset
  delay(1000);               // espera por um segundo
  digitalWrite(RESET, LOW);  // libera Reset
  delay(100);               // espera 100 milisegundos       
}                           // fim da função reset_HT

// Função de Espera para envio de novo comando
// Monitora Serial TX do HT32SX e envia dados recebidos pela 
// Serial do Monitor Serial.


int Char2Int( char c ){
 int x;
 if (c >= '0' && c <='9' ) {
 x = c - '0';
 }// fim do if
 else if (c >= 'a' && c <='f' ){
   x = (c - 'a') + 10;
  } // fim do else if
  else return(-1);
  return(x);
      
} // Fim da Char2Int

void delay_msg() // Fica esperando Mensagens finaliza em 45 Segundos.
     { 
      int  Temp_H;
      int  Temp_L;
      int  Cont; 
      char c;
      int x;
      int xPos;
      char buf_aux[60];        // Buffer usado para conversão de String.
      
       //String resposta[60];  
       digitalWrite(LED, HIGH);
       Serial.println("Aguarde 45 segundos ou até o led apagar...");
       uint16_t i;
       
       for ( i =0; i<45000; i++)
        {
         if (serial_HT.available()) 
         {
          String resposta = serial_HT.readString(); 
          Serial.print(resposta);
          if(resposta.indexOf("Customer resp:") > 0) { // Downlink
            // exemplo de resposta:  
            // Customer resp: {0x64,0xf,0xa0,0x78,0x90,0xab,0xcd,0xef}
            xPos = resposta.indexOf('x');          // Parse Primeiro x
            c = resposta[xPos+1];
            Cont = Char2Int(c);
            c = resposta[xPos+2];
            if( c != ',' ){
              x = Char2Int(c);        
              Cont *=16;
              Cont +=x;
             }

            xPos = resposta.indexOf('x', xPos+1);  // Parse Segundo x
            c = resposta[xPos+1];
            Temp_H = Char2Int(c);
            c = resposta[xPos+2];
            if( c != ',' ){
              x = Char2Int(c);
              Temp_H *=16;
              Temp_H +=x;
             }

           xPos = resposta.indexOf('x', xPos+1); // Parse Terceiro x
            c = resposta[xPos+1];
            Temp_L = Char2Int(c);
            c = resposta[xPos+2];
            if( c != ',' ){
              x = Char2Int(c);
              Temp_L *=16;
              Temp_L +=x;
             }

             Contador = Cont;
             Temperatura = Temp_H;
             Temperatura *=256;
             Temperatura += Temp_L;

             sprintf(buf_aux, "Valor Recebido: Contador=0x%02x Temperatura=0x%04x;", Contador, Temperatura); 
             Serial.println (buf_aux);
             
            }// fim if resposta

        
          if(resposta.indexOf("AT_cmd_Waiting...") > 0) {   
            i = 50000; // Modulo finalizou operação pode receber novo comando.
          }
         } // serial HT
         delay(1);
       } // fim do for 45000
       digitalWrite(LED, LOW);     // Apaga o Led, 
       Serial.println("Ponto para receber novo comando");
              
     }// fim função delay_msg
 
  
