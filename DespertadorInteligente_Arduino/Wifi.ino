
void ConfigutarRedes() {
  wifiMulti.addAP("ALSW", "2526-4897");
  wifiMulti.addAP("ALSW2", "7210-3607");
  wifiMulti.addAP("G-WIFI", "Medicina009");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("\r\n");
    Serial.println("Sistema Conectado a Wifi");
    Serial.print("Conectado SSID: ");
    Serial.print(WiFi.SSID());
    Serial.print(" IP: ");
    Serial.print(WiFi.localIP());
    Serial.println("\r\n");
    Estado[Wifi] = Conectado;
  }
  else {
    Estado[Wifi] = Desconectado;
  }
}

void ActualizarWifi() {
  if (wifiMulti.run() != WL_CONNECTED) {
    Estado[Wifi] = Desconectado;
    Estado[MQTT] = NoConfigurado;
    Estado[Tiempo] = NoConfigurado;
    Serial.println("Sin Wifi");
  }
  if (Estado[Wifi] == Desconectado) {
    Serial.println("\r\n");
    Serial.println("Sistema Conectado a Wifi");
    Serial.print("Conectado SSID: ");
    Serial.print(WiFi.SSID());
    Serial.print(" IP: ");
    Serial.print(WiFi.localIP());
    Serial.println("\r\n");
    Estado[Wifi] = Conectado;
  }
}
