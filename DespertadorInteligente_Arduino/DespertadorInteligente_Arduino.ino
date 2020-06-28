#include <PubSubClient.h>
#include <WiFiUdp.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#else
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#endif

WiFiClient espClient;
PubSubClient client(espClient);

int PinBomba = 4;
#define ActivaMensaje 1
#define ActivaRTC 2

int EstadoBomba = 0;//0 Apagada 1 Encendida
int PinBoton = 13;

#define NoConfigurado 0
#define Configurado 1
#define Desconectado 2
#define Conectado 3
#define Error 4

#define Wifi 0
#define Alarma 1
#define MQTT 2
#define Tiempo 3

#define CantidadIndicadores 4

#define LedRojo 25
#define LedAmarillo 26
#define LedBlanco 27
#define LedAzul 14

unsigned int LedIndicado[CantidadIndicadores] = {LedRojo, LedAmarillo, LedBlanco, LedAzul};
boolean EstadoLed[CantidadIndicadores] = {true, true, true, true};
unsigned int Estado[CantidadIndicadores] = { NoConfigurado, NoConfigurado, NoConfigurado, NoConfigurado };
unsigned long TiempoUltimo[CantidadIndicadores] = {0, 0, 0, 0};
unsigned long TiempoIntervalo[CantidadIndicadores] = {500, 500, 500, 500};

void setup() {
  for (int i = 0; i < CantidadIndicadores ; i++) {
    pinMode(LedIndicado[i], OUTPUT);
    digitalWrite(LedIndicado[i], HIGH);
  }
  pinMode(PinBoton, INPUT);
  pinMode(PinBomba, OUTPUT);
  Serial.begin(115200);
  
  delay(500);
  Serial.println("\r\n");
  Serial.println("Inciando Sistema de Alarma Ingeligente");
  ConfigutarRedes();
}

void loop() {
  DepurarSerial();
  ActualizarLed();
  ActualizarWifi();
  ActualizarTiempo();
  ActualizarMQTT();
  ActualizarAlarma();
}

void DepurarSerial() {
  Serial.print("Estado Wifi:");
  Serial.print(Estado[Wifi]);
  Serial.print(" MQTT:");
  Serial.print(Estado[MQTT]);
  Serial.print(" Alarma:");
  Serial.print(Estado[Alarma]);
  Serial.print(" Tiempo:");
  Serial.print(Estado[Tiempo]);
  Serial.print(" Bonba:" );
  Serial.print(ObtenerBonba());
  Serial.print(" Actual ");
  Serial.print(ActualHora());
  Serial.print(":");
  Serial.print(ActualMinuto());
  Serial.print(" Programado ");
  Serial.print(AlarmaHora());
  Serial.print(":");
  Serial.print( AlarmaMinuto());
  Serial.println();
  delay(100);
}

void ActualizarLed() {
  for (int i = 0; i < CantidadIndicadores ; i++) {
    switch (Estado[i]) {
      case NoConfigurado:
        TiempoIntervalo[i] = 4000;
        break;
      case Configurado:
        TiempoIntervalo[i] = 2000;
        break;
      case Desconectado:
        TiempoIntervalo[i] = 500;
        break;
      case Conectado:
        TiempoIntervalo[i] = 100;
        break;
      case Error:
        TiempoIntervalo[i] = 7000;
        break;
    }
    CambiarLed(i);
  }
}

void CambiarLed(int i) {
  if (millis() > TiempoUltimo[i] + TiempoIntervalo[i] ) {
    TiempoUltimo[i] = millis();
    EstadoLed[i] = !EstadoLed[i];
    if ( EstadoLed[i]) {
      digitalWrite(LedIndicado[i], 0);
    }
    else {
      digitalWrite(LedIndicado[i], 1);
    }
  }
}
