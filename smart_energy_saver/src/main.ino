// Versão OFFLINE/SERIAL - Garante o funcionamento no Wokwi sem problemas de conexão Wi-Fi.

// Pinos do ESP32 (Wokwi)
const int pirPin = 27;   // Pino para o Sensor PIR (Entrada)
const int relayPin = 26; // Pino para o LED/Relé (Saída)

// Variáveis de Estado
bool presenceDetected = false;
unsigned long lastPresenceTime = 0;
const long timeoutNoPresence = 10000; // 10 segundos de timeout sem presença

// Endpoints HTTP Simulados (para documentação e demonstração)
const char* http_status_url = "http://api.smartoffice.com/v1/energy_saver/status";
const char* http_alert_url = "http://api.smartoffice.com/v1/energy_saver/alert";

void print_http_status(bool presence, bool relay_on) {
  // Simula o envio do payload HTTP na Saída Serial
  Serial.println("--- SIMULAÇÃO DE REQUISIÇÃO HTTP (POST) ---");
  Serial.print("ENDPOINT: ");
  Serial.println(http_status_url);
  Serial.print("PAYLOAD: {\"device_id\": \"ES-001\", \"presence\": \"");
  Serial.print(presence ? "true" : "false");
  Serial.print("\", \"relay_status\": \"");
  Serial.print(relay_on ? "on" : "off");
  Serial.println("\"}");
  Serial.println("-------------------------------------------");
}

void print_http_alert(const char* action, const char* reason) {
  // Simula o envio do payload HTTP de Alerta na Saída Serial
  Serial.println("--- SIMULAÇÃO DE ALERTA HTTP (POST) ---");
  Serial.print("ENDPOINT: ");
  Serial.println(http_alert_url);
  Serial.print("PAYLOAD: {\"device_id\": \"ES-001\", \"action\": \"");
  Serial.print(action);
  Serial.print("\", \"reason\": \"");
  Serial.print(reason);
  Serial.println("\"}");
  Serial.println("---------------------------------------");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Smart Energy Saver - Versão OFFLINE/SERIAL");
  Serial.println("Iniciando...");
  
  pinMode(pirPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Começa com o relé desligado
  
  // Simula o envio do status inicial
  print_http_status(false, false);
}

void loop() {
  // 1. Leitura do Sensor PIR
  int pirState = digitalRead(pirPin);

  if (pirState == HIGH) {
    // Presença detectada
    if (!presenceDetected) {
      Serial.println(">>> Presença detectada! Ligando dispositivo.");
      digitalWrite(relayPin, HIGH); // Liga o dispositivo
      print_http_status(true, true);
    }
    presenceDetected = true;
    lastPresenceTime = millis(); // Atualiza o tempo da última presença
  } else {
    // Nenhuma presença detectada
    if (presenceDetected) {
      // Verifica se o tempo limite de ausência foi atingido
      if (millis() - lastPresenceTime > timeoutNoPresence) {
        if (digitalRead(relayPin) == HIGH) {
          Serial.println("<<< Ausência por muito tempo. Desligando dispositivo para economia.");
          digitalWrite(relayPin, LOW); // Desliga o dispositivo
          print_http_status(false, false);
          
          // Envia alerta de economia
          print_http_alert("power_off", "no_presence");
        }
        presenceDetected = false; // Reseta o estado de presença
      }
    }
  }

  // Envia o status a cada 5 segundos (para manter o "Keep Alive" simulado)
  static unsigned long lastPublishTime = 0;
  if (millis() - lastPublishTime > 5000) {
    print_http_status(presenceDetected, digitalRead(relayPin) == HIGH);
    lastPublishTime = millis();
  }

  delay(500); // Pequeno delay para estabilidade
}
