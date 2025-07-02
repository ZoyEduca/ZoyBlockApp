void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);   // PWM A
  pinMode(5, OUTPUT);   // DIR A
  pinMode(6, OUTPUT);   // PWM B
  pinMode(11, OUTPUT);  // DIR B
  pinMode(13, OUTPUT);  // LED
  pinMode(12, OUTPUT);  // Buzzer
}

void loop() {
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando.startsWith("MOTOR_ESQUERDO_FRENTE")) {
      int sep = comando.indexOf(' ', 22);
      int velA = comando.substring(22, sep).toInt();
    
      analogWrite(3, velA);
      digitalWrite(5, LOW);
      Serial.println("OK");
     
      }else if (comando.startsWith("MOTOR_ESQUERDO_TRAS")) {
      int sep = comando.indexOf(' ', 20);
      int velA = comando.substring(20, sep).toInt();
      int velB = comando.substring(sep + 1).toInt();

      analogWrite(5, velA);
      digitalWrite(3, LOW);
      Serial.println("OK");

      }else if (comando.startsWith("MOTOR_DIREITO_FRENTE")) {
      int sep = comando.indexOf(' ', 21);
      int velA = comando.substring(21, sep).toInt();
    
      analogWrite(6, velA);
      digitalWrite(11, LOW);
      Serial.println("OK");
     
      }else if (comando.startsWith("MOTOR_DIREITO_TRAS")) {
      int sep = comando.indexOf(' ', 19);
      int velA = comando.substring(19, sep).toInt();
      int velB = comando.substring(sep + 1).toInt();

      analogWrite(11, velA);
      digitalWrite(6, LOW);
      Serial.println("OK");

    }else if (comando.startsWith("MOTOR_TRAS")) {
      int sep = comando.indexOf(' ', 11);
      int velA = comando.substring(11, sep).toInt();
      int velB = comando.substring(sep + 1).toInt();

      digitalWrite(3, LOW);
      analogWrite(5, velA);
      digitalWrite(6, LOW);
      analogWrite(11, velB);
    
    }else if (comando == "PARAR") {
      digitalWrite(3, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);

      Serial.println("OK");

    } else if (comando.startsWith("MOTOR_TRAS")) {
      int sep = comando.indexOf(' ', 11);
      int velA = comando.substring(11, sep).toInt();
      int velB = comando.substring(sep + 1).toInt();

      digitalWrite(3, LOW);
      analogWrite(5, velA);
      digitalWrite(6, LOW);
      analogWrite(11, velB);

      Serial.println("OK");

    } else if (comando.startsWith("LED")) {
      int valor = comando.substring(4).toInt();
      digitalWrite(13, valor > 0 ? HIGH : LOW);
      Serial.println("OK");

    } else if (comando.startsWith("BEEP")) {
      int freq = comando.substring(5).toInt();
      tone(12, freq, 300);
      Serial.println("OK");

    } else {
      Serial.println("ERRO:CMD_DESCONHECIDO");
    }
  }
}
