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

int PinBomba = 2;
#define ActivaMensaje 1
#define ActivaRTC 2

int EstadoBomba = 0;//0 Apagada 1 Encendida
int PinBoton = 4;

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

int LedIndicado[CantidadIndicadores] = {12, 5, 12, 14};
boolean EstadoLed[CantidadIndicadores] = {false, false, false, false};
int Estado[CantidadIndicadores] = { NoConfigurado, NoConfigurado, NoConfigurado, NoConfigurado };
unsigned long TiempoUltimo[CantidadIndicadores] = {0, 0, 0, 0};
unsigned long TiempoIntervalo[CantidadIndicadores] = {500, 500, 500, 500};

void setup() {
  for (int i = 0; i < CantidadIndicadores ; i++) {
    pinMode(LedIndicado[i], OUTPUT);
  }
  pinMode(PinBoton, INPUT);
  pinMode(PinBomba, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n");

  Serial.println("Inciando Sistema de Alarma Ingeligente");
  ConfigutarRedes();
}

void loop() {
  DepurarSerial();
  CambiarLed(Wifi);
  CambiarLed(Alarma);
  ActualizarWifi();
  ActualizarTiempo();
  ActualizarMQTT();
  ActualizarAlarma();
  switch (Estado[Alarma]) {
    case 0://Aun no Activo
      TiempoIntervalo[Alarma] = 400;

      break;
    case 1:

      break;
  }
  switch (Estado[Wifi]) {
    case 0://Aun no Activo
      TiempoIntervalo[Wifi] = 400;
      break;
    case 1:
      TiempoIntervalo[Wifi] = 1000;
      break;
    case 2:
      TiempoIntervalo[Wifi] = 2000;
      break;
  }

}

void DepurarSerial() {
  Serial.print("Estado Wifi:");
  Serial.print(Estado[Wifi]);
  Serial.print(" MQTT:");
  Serial.print(Estado[MQTT]);
  Serial.print(" Alarma:");
  Serial.print(Estado[Alarma]);
  Serial.print(" Bonba:" );
  Serial.print(ObtenerBonba());
  Serial.print(" Tiempo:");
  Serial.print(Estado[Tiempo]);
  Serial.print(" Actual ");
  Serial.print( ActualHora());
  Serial.print(":");
  Serial.print( ActualMinuto());
  Serial.print(" Programado ");
  Serial.print( AlarmaHora());
  Serial.print(":");
  Serial.print( AlarmaMinuto());
  Serial.println();
  delay(100);
}

void CambiarLed(int i) {
  if (millis() > TiempoUltimo[i] + TiempoIntervalo[i] ) {
    TiempoUltimo[i] = millis();
    EstadoLed[i] = !  EstadoLed[i];
    if ( EstadoLed[i]) {
      digitalWrite(LedIndicado[i], 0);
    }
    else {
      digitalWrite(LedIndicado[i], 1);
    }
  }
}
