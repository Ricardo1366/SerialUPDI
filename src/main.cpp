#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <avr/sleep.h>

// Pines módulo LoRa RFM95W
#define LORA_SSPIN PIN_PA7
#define LORA_SCKPIN PIN_PA6
#define LORA_MISOPIN PIN_PA5
#define LORA_MOSIPIN PIN_PA4
#define LORA_DIO0PIN PIN_PC2
#define LORA_DIO1PIN PIN_PC3
#define LORA_RESETPIN PIN_PD6

// Pines temporizador TPL5010
#define TEMP_WAKEPIN PIN_PC1
#define TEMP_DONEPIN PIN_PC0

#define PINLED DD0

const long frecuencia = 868E6;    // 868 MHz
long intervalo = 1000;
long tiempo = 0;
volatile bool despierta = false;
long contadorPaqutes;

void despertar();

void setup() {
  delay(10000);
  // Configuramos el monitor serial a 115200
  Serial2.begin(115200);
  while (!Serial2);
  // set_sleep_mode(SLEEP_MODE_STANDBY);
  // sleep_enable();

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
  attachInterrupt(TEMP_WAKEPIN, despertar, RISING);

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
    Serial2.print("Paquete recibido "); Serial2.print(contadorPaqutes++); Serial2.print(" - ") ;Serial2.print(packetSize); Serial2.println(F(" bytes de longitud."));

    // read packet
    while (LoRa.available()) {
      Serial2.print((char)LoRa.read());
    }

    if (contadorPaqutes == 5)
    {
      contadorPaqutes = 0;
      delay(200);
      // sleep_mode();
      // sleep_disable();
      Serial2.println(F("¡Despierto!"));
    }
    
    // print RSSI of packet
    Serial2.println();
    Serial2.print("  Potencia señal = ");
    Serial2.println(LoRa.packetRssi());
    digitalWrite(PINLED, !digitalRead(PINLED));
  }
  if(millis() - tiempo >= intervalo){
    tiempo += intervalo;
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