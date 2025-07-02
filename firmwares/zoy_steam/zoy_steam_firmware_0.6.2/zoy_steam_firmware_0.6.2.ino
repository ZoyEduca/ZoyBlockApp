const int LED_LEFT = 15;
const int LED_RIGHT = 16;

const int MOTOR_E1 = 3;
const int MOTOR_E2 = 5;
const int MOTOR_D1 = 6;
const int MOTOR_D2 = 11;

const int BUZZER = 12; // Opcional, se quiser BEEP
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

// Variáveis globais para o buffer serial.
String buffer = ""; 

// === Função de leitura do Ultrassom (retorna float para maior precisão) ===
float ler_ultrassom(int trigPin, int echoPin)
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duracao = pulseIn(echoPin, HIGH); 
  float distancia_cm = duracao / 29.6 / 2.0;

  return distancia_cm;
}

void setup()
{
  Serial.begin(9600);
  // Inicialização de pinos
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);
  pinMode(LED_13, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Garante que os LEDs estejam desligados no início
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_13, LOW);
}

void loop()
{
  // Lógica de leitura serial para comandos
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '<') // Início de um novo comando
    {
      buffer = "";
      buffer += c;
    }
    else if (buffer.length() > 0) // Continua lendo o comando
    {
      buffer += c;
      if (c == '>') // Fim do comando
      {
        processarComando(buffer); // Processa o comando completo
        buffer = ""; // Limpa o buffer para o próximo comando
      }
    }
  }
  
  // --- Controle Assíncrono dos LEDs (permanecem no loop, pois são baseados em millis()) ---
  // Controle assíncrono do LED_13
  if (pisca13Ativo && pisca13Restantes > 0)
  {
    unsigned long agora = millis();
    if (agora - tempoAnterior13 >= intervaloLed13)
    {
      tempoAnterior13 = agora;
      estadoLed13 = !estadoLed13;
      digitalWrite(LED_13, estadoLed13 ? HIGH : LOW);
      if (!estadoLed13) // Decrementa a contagem quando o LED apaga
      {
        pisca13Restantes--;
        if (pisca13Restantes == 0)
        {
          pisca13Ativo = false;
        }
      }
    }
  }

  // Pisca LED_LEFT com millis()
  if (piscaLeftAtivo && millis() - tempoAnteriorLeft >= intervaloLedLeft)
  {
    tempoAnteriorLeft = millis();
    estadoLedLeft = !estadoLedLeft;
    digitalWrite(LED_LEFT, estadoLedLeft);
    if (!estadoLedLeft) // só conta quando apaga
    {
      piscaLeftRestantes--;
      if (piscaLeftRestantes <= 0)
      {
        piscaLeftAtivo = false;
        digitalWrite(LED_LEFT, LOW);
      }
    }
  }

  // Pisca LED_RIGHT com millis()
  if (piscaRightAtivo && millis() - tempoAnteriorRight >= intervaloLedRight)
  {
    tempoAnteriorRight = millis();
    estadoLedRight = !estadoLedRight;
    digitalWrite(LED_RIGHT, estadoLedRight);
    if (!estadoLedRight)
    {
      piscaRightRestantes--;
      if (piscaRightRestantes <= 0)
      {
        piscaRightAtivo = false;
        digitalWrite(LED_RIGHT, LOW);
      }
    }
  }
}

// === Função para mapear pino analógico ===
int lerAnalogico(String pino)
{
  pino.trim();
  if (pino == "A0")
    return analogRead(A0);
  if (pino == "A3")
    return analogRead(A3);
  if (pino == "A4")
    return analogRead(A4);
  if (pino == "A5")
    return analogRead(A5);
#if defined(__AVR_ATmega328P__) // Ex: Arduino Nano
  if (pino == "A6")
    return analogRead(A6);
  if (pino == "A7")
    return analogRead(A7);
#endif
  return -1; // pino inválido
}

// === Função para processar comandos recebidos via Serial ===
void processarComando(String cmd)
{
  cmd.remove(0, 1);             // remove '<' do início
  cmd.remove(cmd.length() - 1); // remove '>' do final

  int sepCmdArg = cmd.indexOf(':');
  String comando_temp;
  String argumentos_temp;

  if (sepCmdArg == -1) // Se não houver ':'
  {
    comando_temp = cmd;
    argumentos_temp = "";
  }
  else
  {
    comando_temp = cmd.substring(0, sepCmdArg);
    argumentos_temp = cmd.substring(sepCmdArg + 1);
  }
  comando_temp.trim();
  argumentos_temp.trim();
  
  // === LER SENSOR ULTRASSOM ===
  if (comando_temp == "ULTRASSOM")
  {
    int primeiro_virgula = argumentos_temp.indexOf(',');
    if (primeiro_virgula != -1) {
      int trigPin = argumentos_temp.substring(0, primeiro_virgula).toInt();
      int echoPin = argumentos_temp.substring(primeiro_virgula + 1).toInt();
      
      // CORREÇÃO: 'arguments_temp' para 'argumentos_temp'
      if (trigPin == 0 && argumentos_temp.substring(0, primeiro_virgula) != "0" || 
          echoPin == 0 && argumentos_temp.substring(primeiro_virgula + 1) != "0") {
          Serial.println("ERRO:PINOS_ULTRASSOM_INVALIDOS");
          return;
      }

      float distancia = ler_ultrassom(trigPin, echoPin);
      Serial.print("DISTANCIA:");
      Serial.println(distancia, 2); // Imprime a distância com 2 casas decimais
      // REMOVIDO: Serial.println("OK"); para evitar linhas extras
    } else {
      Serial.println("ERRO:PARAMETROS_ULTRASSOM_AUSENTES"); // Se faltar a vírgula
    }
    return; // Importante: Sai da função processarComando após lidar com o comando ULTRASSOM
  }
  
  // === LER SENSOR ANALÓGICO (IR, LDR, Potenciômetro, etc.) ===
  // Comando esperado do Python: <ANALOG_READ:A0> ou <ANALOG_READ:A1> etc.
  if (comando_temp == "ANALOG_READ")
  {
    int valor = lerAnalogico(argumentos_temp);
    if (valor != -1) { // -1 significa pino inválido
      Serial.print("ANALOG_VALOR:"); // Resposta formatada para o Python
      Serial.println(valor);
      // REMOVIDO: Serial.println("OK"); para evitar linhas extras
    } else {
      Serial.println("ERRO:PINO_ANALOGICO_INVALIDO"); // Mensagem de erro mais específica
    }
    return; // Sai da função após processar o comando
  }

  // === LED_TREZE ===
  if (comando_temp == "LED_TREZE")
  {
    if (argumentos_temp == "HIGH")
    {
      digitalWrite(LED_13, HIGH);
      Serial.println("OK");
    }
    else if (argumentos_temp == "LOW")
    {
      digitalWrite(LED_13, LOW);
      Serial.println("OK");
    }
    else
    {
      int vezes = argumentos_temp.toInt();
      if (vezes > 0)
      {
        pisca13Restantes = vezes;
        pisca13Ativo = true;
        tempoAnterior13 = millis();
        estadoLed13 = LOW; // Começa apagado para o primeiro pisca
        digitalWrite(LED_13, estadoLed13);
        Serial.println("OK");
      }
      else
      {
        Serial.println("ERRO:ARG_INVALIDO");
      }
    }
    return;
  }

  // === LED LEFT ===
  if (comando_temp == "LED_LEFT")
  {
    if (argumentos_temp == "HIGH")
    {
      digitalWrite(LED_LEFT, HIGH);
      Serial.println("OK");
    }
    else if (argumentos_temp == "LOW")
    {
      digitalWrite(LED_LEFT, LOW);
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERRO:ARG_INVALIDO");
    }
    return;
  }
  // === LED RIGHT ===
  if (comando_temp == "LED_RIGHT")
  {
    if (argumentos_temp == "HIGH")
    {
      digitalWrite(LED_RIGHT, HIGH);
      Serial.println("OK");
    }
    else if (argumentos_temp == "LOW")
    {
      digitalWrite(LED_RIGHT, LOW);
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERRO:ARG_INVALIDO");
    }
    return;
  }

  // === ACIONA OS PINOS DO MOTOR (D3, D5, D6, D11) ===
  if (comando_temp == "D3" || comando_temp == "D5" || comando_temp == "D6" || comando_temp == "D11")
  {
    int pinoAlvo = comando_temp.substring(1).toInt(); // Extrai o número do pino (ex: de "D3" pega 3)
    if (argumentos_temp == "HIGH")
    {
      digitalWrite(pinoAlvo, HIGH);
      Serial.println("OK");
    }
    else if (argumentos_temp == "LOW")
    {
      digitalWrite(pinoAlvo, LOW);
      Serial.println("OK");
    } else {
      Serial.println("ERRO:ARG_INVALIDO_PINO");
    }
    return;
  }

  // === MOTOR ESQUERDO_FRENTE ===
  if (comando_temp == "MOTOR_ESQUERDO_FRENTE")
  {
    int velA = argumentos_temp.toInt();
    analogWrite(MOTOR_E1, velA);
    analogWrite(MOTOR_E2, 0);
    Serial.println("OK");
    return;
  }
  // === MOTOR ESQUERDO_TRAS ===
  if (comando_temp == "MOTOR_ESQUERDO_TRAS")
  {
    int velA = argumentos_temp.toInt();
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, velA);
    Serial.println("OK");
    return;
  }
  // === MOTOR DIREITO_FRENTE ===
  if (comando_temp == "MOTOR_DIREITO_FRENTE")
  {
    int velA = argumentos_temp.toInt();
    analogWrite(MOTOR_D1, velA);
    analogWrite(MOTOR_D2, 0);
    Serial.println("OK");
    return;
  }
  // === MOTOR DIREITO_TRAS ===
  if (comando_temp == "MOTOR_DIREITO_TRAS")
  {
    int velA = argumentos_temp.toInt();
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, velA);
    Serial.println("OK");
    return;
  }

  // === MOTOR FRENTE ===
  if (comando_temp == "MOTOR_FRENTE")
  {
    int sep = argumentos_temp.indexOf(',');
    if (sep == -1)
    {
      Serial.println("ERRO:ARGUMENTOS");
      return;
    }
    int velA = argumentos_temp.substring(0, sep).toInt();
    int velB = argumentos_temp.substring(sep + 1).toInt();
    analogWrite(MOTOR_E1, velA);
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, velB);
    analogWrite(MOTOR_D2, 0);
    Serial.println("OK");
    return;
  }

  // === MOTOR TRAS ===
  if (comando_temp == "MOTOR_TRAS")
  {
    int sep = argumentos_temp.indexOf(',');
    if (sep == -1)
    {
      Serial.println("ERRO:ARGUMENTOS");
      return;
    }
    int velA = argumentos_temp.substring(0, sep).toInt();
    int velB = argumentos_temp.substring(sep + 1).toInt();
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, velA);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, velB);
    Serial.println("OK");
    return;
  }

  // === PARAR TODOS OS MOTORES ===
  if (comando_temp == "PARAR")
  {
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, 0);
    Serial.println("OK");
    return;
  }
  
  // === AGUARDA:N segundos ===
  if (comando_temp == "AGUARDA")
  {
    int segundos = argumentos_temp.toInt();
    delay(segundos * 1000);
    Serial.println("OK");
    return;
  } 

  // === BEEP:N ms ===
  if (comando_temp == "BEEP")
  {
    int duracao = argumentos_temp.toInt();
    digitalWrite(BUZZER, HIGH);
    delay(duracao);
    digitalWrite(BUZZER, LOW);
    Serial.println("OK");
    return;
  }

  // === LED_PISCA_LEFT ===
  if (comando_temp == "LED_PISCA_LEFT")
  {
    int vezes = argumentos_temp.toInt();
    if (vezes > 0)
    {
      piscaLeftAtivo = true;
      piscaLeftRestantes = vezes;
      tempoAnteriorLeft = millis();
      estadoLedLeft = LOW; // Inicia apagado para piscar
      digitalWrite(LED_LEFT, estadoLedLeft);
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERRO:ARG_INVALIDO");
    }
    return;
  }

  // === LED_PISCA_RIGHT ===
  if (comando_temp == "LED_PISCA_RIGHT")
  {
    int vezes = argumentos_temp.toInt();
    if (vezes > 0)
    {
      piscaRightAtivo = true;
      piscaRightRestantes = vezes;
      tempoAnteriorRight = millis();
      estadoLedRight = LOW; // Inicia apagado para piscar
      digitalWrite(LED_RIGHT, estadoLedRight);
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERRO:ARG_INVALIDO");
    }
    return;
  }

  // === Comando ZOY de firmware ===
  if (comando_temp == "ZOY" && argumentos_temp == "ZOY")
  {
    Serial.println("FIRMWARE:ZOY_STEAM:v0.5.8");
    return;
  }

  // Se o comando não foi reconhecido
  Serial.println("ERRO:COMANDO_INVALIDO");
}