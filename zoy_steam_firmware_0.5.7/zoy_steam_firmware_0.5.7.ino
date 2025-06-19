
const int LED_LEFT = 15;
const int LED_RIGHT = 16;

const int MOTOR_E1 = 3;
const int MOTOR_E2 = 5;
const int MOTOR_D1 = 6;
const int MOTOR_D2 = 11;

const int BUZZER = 12;  // Opcional, se quiser BEEP
const int LED_13 = 13;

// Controle assíncrono do LED_LEFT
bool piscaLeftAtivo = false;
int piscaLeftRestantes = 0;
unsigned long tempoAnteriorLeft = 0;
bool estadoLedLeft = LOW;
const unsigned long intervaloLedLeft = 300;

// Controle assíncrono do LED_RIGHT
bool piscaRightAtivo = false;
int piscaRightRestantes = 0;
unsigned long tempoAnteriorRight = 0;
bool estadoLedRight = LOW;
const unsigned long intervaloLedRight = 300;


// Controle assíncrono do LED_13
bool pisca13Ativo = false;
int pisca13Restantes = 0;
unsigned long tempoAnterior13 = 0;
bool estadoLed13 = LOW;
const unsigned long intervaloLed13 = 300;


void setup() {
  Serial.begin(9600);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  
  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);
  pinMode(LED_13 , OUTPUT);
  
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_13, LOW);
  
}

String buffer = "";

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '<') {
      buffer = "";
      buffer += c;
    } else if (buffer.length() > 0) {
      buffer += c;
      if (c == '>') {
        processarComando(buffer);
        buffer = "";
      }
    }
  }
// Controle assíncrono do LED_13
if (pisca13Ativo && pisca13Restantes > 0) {
  unsigned long agora = millis();
  if (agora - tempoAnterior13 >= intervaloLed13) {
    tempoAnterior13 = agora;

    estadoLed13 = !estadoLed13;
    digitalWrite(LED_13, estadoLed13 ? HIGH : LOW);

    if (!estadoLed13) {
      pisca13Restantes--;
      if (pisca13Restantes == 0) {
        pisca13Ativo = false;
      }
    }
  }
}



// Pisca LED_LEFT com millis()
 if (piscaLeftAtivo && millis() - tempoAnteriorLeft >= intervaloLedLeft) {
  tempoAnteriorLeft = millis();
  estadoLedLeft = !estadoLedLeft;
  digitalWrite(LED_LEFT, estadoLedLeft);

  if (!estadoLedLeft) { // só conta quando apaga
    piscaLeftRestantes--;
    if (piscaLeftRestantes <= 0) {
      piscaLeftAtivo = false;
      digitalWrite(LED_LEFT, LOW);
    }
  }
}

// Pisca LED_RIGHT com millis()
if (piscaRightAtivo && millis() - tempoAnteriorRight >= intervaloLedRight) {
  tempoAnteriorRight = millis();
  estadoLedRight = !estadoLedRight;
  digitalWrite(LED_RIGHT, estadoLedRight);

  if (!estadoLedRight) {
    piscaRightRestantes--;
    if (piscaRightRestantes <= 0) {
      piscaRightAtivo = false;
      digitalWrite(LED_RIGHT, LOW);
    }
  }
}
 
}

void processarComando(String cmd) {
  cmd.remove(0, 1);  // remove '<'
  cmd.remove(cmd.length() - 1);  // remove '>'

  int sepCmdArg = cmd.indexOf(':');
  if (sepCmdArg == -1) {
    Serial.println("ERRO:SEM_SEPARADOR");
    return;
  }

  String comando = cmd.substring(0, sepCmdArg);
  String argumentos = cmd.substring(sepCmdArg + 1);
  argumentos.trim();

 
 if (comando == "LED_TREZE") {

  if (argumentos == "HIGH") {
    digitalWrite(LED_13, HIGH);
    Serial.println("OK");
  } else if (argumentos == "LOW") {
    digitalWrite(LED_13, LOW);
    Serial.println("OK");
  } else {
    int vezes = argumentos.toInt();
    if (vezes > 0) {
      pisca13Restantes = vezes;
      pisca13Ativo = true;
      tempoAnterior13 = millis();
      estadoLed13 = LOW;
      digitalWrite(LED_13, estadoLed13);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO");
    }
  }
}



  // === LED LEFT ===
 if (comando == "LED_LEFT") {
    if (argumentos == "HIGH") {
      digitalWrite(LED_LEFT, HIGH);
      Serial.println("OK");
    } else if (argumentos == "LOW") {
      digitalWrite(LED_LEFT, LOW);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO");
    }
 }
  // === LED RIGHT ===
   if (comando == "LED_RIGHT") {
    if (argumentos == "HIGH") {
      digitalWrite(LED_RIGHT, HIGH);
      Serial.println("OK");
    } else if (argumentos == "LOW") {
      digitalWrite(LED_RIGHT, LOW);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO");
    }
   }
  // === MOTOR FRENTE ===
   if (comando == "MOTOR_FRENTE") {
    int sep = argumentos.indexOf(',');
    if (sep == -1) {
      Serial.println("ERRO:ARGUMENTOS");
      return;
    }
    int velA = argumentos.substring(0, sep).toInt();
    int velB = argumentos.substring(sep + 1).toInt();
    analogWrite(MOTOR_E1, velA);
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, velB);
    analogWrite(MOTOR_D2, 0);
    Serial.println("OK");
   }

   // === MOTOR TRAS ===
   if (comando == "MOTOR_TRAS") {
    int sep = argumentos.indexOf(',');
    if (sep == -1) {
      Serial.println("ERRO:ARGUMENTOS");
      return;
    }
    int velA = argumentos.substring(0, sep).toInt();
    int velB = argumentos.substring(sep + 1).toInt();
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, velA);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, velB);
    Serial.println("OK");
   }

  // === PARAR ===
  if (comando == "PARAR") {
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, 0);
    Serial.println("OK");
 
  }
  // === AGUARDA:N segundos ===
   if (comando == "AGUARDA") {
    int segundos = argumentos.toInt();
    delay(segundos * 1000);
    Serial.println("OK");

   } // === BEEP:N ms ===
    if (comando == "BEEP") {
    int duracao = argumentos.toInt();
    digitalWrite(BUZZER, HIGH);
    delay(duracao);
    digitalWrite(BUZZER, LOW);
    Serial.println("OK");
    }

if (comando == "LED_PISCA_LEFT") {
    int vezes = argumentos.toInt();
    if (vezes > 0) {
      piscaLeftAtivo = true;
      piscaLeftRestantes = vezes;
      tempoAnteriorLeft = millis();
      estadoLedLeft = LOW;
      digitalWrite(LED_LEFT, estadoLedLeft);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO");
    }

  } 
  if (comando == "LED_PISCA_RIGHT") {
    int vezes = argumentos.toInt();
    if (vezes > 0) {
      piscaRightAtivo = true;
      piscaRightRestantes = vezes;
      tempoAnteriorRight = millis();
      estadoLedRight = LOW;
      digitalWrite(LED_RIGHT, estadoLedRight);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO");
    }
 
  } 
   if(comando == "ZOY" && argumentos == "ZOY"){  
    Serial.println("FIRMWARE:ZOY_STEAM:v0.5.7");
   }
}