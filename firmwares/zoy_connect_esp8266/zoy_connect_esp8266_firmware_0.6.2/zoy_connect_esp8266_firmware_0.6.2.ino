/* 
*
* Firmaware para teste via rede wi-fi na IDE ZoyBlocks
* Comandos simples: MOTOR_FRENTE, MOTOR_TRAS, PARAR
* 
* Futuramente esse firmarware será desenvolvido ...
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Sua Rede";
const char* senha = "Sua Senha";

ESP8266WebServer servidor(80);

int ME_A = D2;
int ME_B = D5;
int MD_A = D8;
int MD_B = D6;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  pinMode(ME_A, OUTPUT);
  pinMode(ME_B, OUTPUT);
  pinMode(MD_A, OUTPUT);
  pinMode(MD_B, OUTPUT);

  servidor.on("/comando", []() {
    String acao = servidor.arg("acao");
    Serial.println("Comando recebido: " + acao);
    Serial.println("Conectado ao Wi-Fi");
    Serial.println("IP do robô: " + WiFi.localIP().toString());

    if (acao == "MOTOR_FRENTE") {
    moverFrente();
   }
   else if (acao == "MOTOR_TRAS") {
    moverTras();
    }
   else if (acao == "PARAR") {
    pararMotores();
   }


    servidor.send(200, "text/plain", "OK: " + acao);
  });

  servidor.begin();
  Serial.println("Servidor iniciado no IP: " + WiFi.localIP().toString());
}

void loop() {
  servidor.handleClient();
}


void moverFrente() {
  digitalWrite(ME_A, HIGH);
  digitalWrite(ME_B, LOW);
  digitalWrite(MD_A, HIGH);
  digitalWrite(MD_B, LOW);
}

void moverTras() {
  digitalWrite(ME_A, LOW);
  digitalWrite(ME_B, HIGH);
  digitalWrite(MD_A, LOW);
  digitalWrite(MD_B, HIGH);
}

void girarEsquerda() {
  digitalWrite(ME_A, LOW);
  digitalWrite(ME_B, HIGH);
  digitalWrite(MD_A, HIGH);
  digitalWrite(MD_B, LOW);
}

void girarDireita() {
  digitalWrite(ME_A, HIGH);
  digitalWrite(ME_B, LOW);
  digitalWrite(MD_A, LOW);
  digitalWrite(MD_B, HIGH);
}

void pararMotores() {
  digitalWrite(ME_A, LOW);
  digitalWrite(ME_B, LOW);
  digitalWrite(MD_A, LOW);
  digitalWrite(MD_B, LOW);
}