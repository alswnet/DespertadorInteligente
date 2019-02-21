const char* mqtt_server = "broker.mqtt-dashboard.com";

long TiempoActual = 0;
long FrecuenciaMensaje = 1000;
char Mensaje[50];
int value = 0;

void ConfigutarMQTT() {
  if (Estado[MQTT] == NoConfigurado) {
    Serial.println("Condigurar MQTT");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    Estado[MQTT] = Configurado;
  }
}

void ActualizarMQTT() {
  if (Estado[MQTT] == NoConfigurado) {
    ConfigutarMQTT();
  }
  else if (Estado[Wifi] == Conectado && Estado[MQTT] >= Configurado) {
    if (!client.connected()) {
      Serial.print("Atentando Conectar MQTT....");
      if (client.connect("ALSWAlarmaInteligente")) {
        Serial.println("Conectado :D");
        client.subscribe("ALSWAlarmaMinuto");
        client.subscribe("ALSWAlarmaHora");
        client.subscribe("ALSWAlarmaEstado");
        client.subscribe("ALSWAlarmaBonba");
        Estado[MQTT] = Conectado;
      } else {
        Serial.print("Error, rc=");
        Serial.println(client.state());
        Estado[MQTT] = Desconectado;
      }
    } else if (Estado[MQTT] == Conectado) {
      client.loop();
      if (TiempoActual + FrecuenciaMensaje  < millis()) {
        TiempoActual = millis();
        int AlarmaEstado = 0;
        int BonbaEstado = 0;
        if (Estado[Alarma] == Conectado ) {
          AlarmaEstado = 1;
        }
        if (ObtenerBonba()) {
          BonbaEstado = 1;
        }
        snprintf (Mensaje, 75, "Alarma:%ld Bonba:%ld ", AlarmaEstado, BonbaEstado);
        //Serial.println("Avisando a MQTT estar vivo");
        client.publish("ALSWAlarmaVivo", Mensaje);
      }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje Recivido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  if (strcmp ("ALSWAlarmaHora", topic) == 0) {
    char HoraMQTT[3] = {' ', ' ', ' '};
    HoraMQTT[0] = (char)payload[0];
    if (length > 0) {
      HoraMQTT[1] = (char)payload[1];
    }
    CambiarAlarmaHora(atoi(HoraMQTT));
  }
  else if (strcmp ("ALSWAlarmaMinuto", topic) == 0) {
    char MinutoMQTT[3] = {' ', ' ', ' '};
    MinutoMQTT[0] = (char)payload[0];
    if (length > 0) {
      MinutoMQTT[1] = (char)payload[1];
    }
    CambiarAlarmaMinuto(atoi(MinutoMQTT));
  }
  else if (strcmp ("ALSWAlarmaEstado", topic) == 0) {
    if ((char)payload[0] == '1') {
      CambiarAlarma(true);
    }
    else if ((char)payload[0] == '0') {
      CambiarAlarma(false);
    }
  }
  else if (strcmp ("ALSWAlarmaBonba", topic) == 0) {
    if ((char)payload[0] == '1') {
      CambiarBonba(true);
    }
    else if ((char)payload[0] == '0') {
      CambiarBonba(false);
    }
  }
}
