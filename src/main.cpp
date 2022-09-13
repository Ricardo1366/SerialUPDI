#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

// Pines módulo LoRa RFM95W
#define LORA_SSPIN PIN_PA7
#define LORA_SCKPIN PIN_PA6
#define LORA_MISOPIN PIN_PA5
#define LORA_MOSIPIN PIN_PA4
#define LORA_DIO0PIN PIN_PC2
#define LORA_DIO1PIN PIN_PC3
#define LORA_RESETPIN PIN_PD6

// Pines temporizador TPL5010
#define TEMP_WAKEPIN  PIN_PC1
#define TEMP_DONEPIN PIN_PC0

#define PINLED DD0

const long frecuencia = 868E6;    // 868 MHz
long intervalo = 1000;
long tiempo = 0;
volatile bool despierta = false;

void despertar();

void setup() {
  // Configuramos el monitor serial a 115200
  Serial2.begin(115200);
  while (!Serial2);

  // Configuramos el módulo LoRa a 868MHz
  Serial2.println("Inicio configuración LoRa Receiver ");
  LoRa.setPins(LORA_SSPIN, LORA_RESETPIN, LORA_DIO0PIN);

  if (!LoRa.begin(frecuencia)) {
    Serial2.println("Inicialización de LoRa ha fallado.");
    while (1);
  }

  // Configuramos los pines del temporizador.
  // pinMode(TEMP_WAKEPIN, INPUT);
  pinMode(TEMP_DONEPIN, OUTPUT);
  digitalWrite(TEMP_DONEPIN, LOW);

  pinMode(PINLED, OUTPUT);

  // Configuramos la interrupción de control del temporizador
  attachInterrupt(TEMP_WAKEPIN, despertar, FALLING);

  // Enviamos señal DONE (Hecho) para evitar el reseteo.

  digitalWrite(TEMP_DONEPIN, HIGH);
  delay(1);
  digitalWrite(TEMP_DONEPIN, LOW);
  
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial2.print("Paquete recibido: '");

    // read packet
    while (LoRa.available()) {
      Serial2.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial2.print("' with RSSI ");
    Serial2.println(LoRa.packetRssi());
    digitalWrite(PINLED, !digitalRead(PINLED));
  }
  if(millis() - tiempo >= intervalo){
    tiempo += intervalo;
    // Serial2.print("Paso "); Serial2.println(tiempo / intervalo);
  }
  if(despierta){
    digitalWrite(TEMP_DONEPIN, HIGH);
    despierta = false;
    Serial2.println("Evitando el reseteo...");
    digitalWrite(TEMP_DONEPIN, LOW);
  }
}


void despertar(){
  despierta = true;
}