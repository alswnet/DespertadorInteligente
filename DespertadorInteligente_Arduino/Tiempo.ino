
unsigned long intervalNTP = 60000; // Request NTP time every minute
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;
unsigned long prevActualTime = 0;


WiFiUDP UDP;
IPAddress timeServerIP;
const char* NTPServerName = "0.north-america.pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte NTPBuffer[NTP_PACKET_SIZE];
int Diferente = 18;

void ConfigurarTiempo() {
  if (Estado[Wifi] >= Conectado && Estado[Tiempo] == NoConfigurado) {
    Serial.println("Condigurarando Tiempo UDP");

    startUDP();

    if (!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
      Serial.println("DNS lookup failed. Rebooting.");
      Serial.flush();
      Estado[Tiempo] = Error;
      //ESP.reset();
    }
    Serial.print("IP del Servidor Tiempo IP:\t");
    Serial.println(timeServerIP);

    Serial.println("Enciando solisitud de NTP ...");
    sendNTPpacket(timeServerIP);
    Estado[Tiempo] = Configurado;
  }
}


void startUDP() {
  Serial.println("Empezando UDP");
  UDP.begin(123);
}

uint32_t getTime() {

  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);
  NTPBuffer[0] = 0b11100011;
  UDP.beginPacket(address, 123);
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
  delay(100);
}

void ActualizarTiempo() {
  if (Estado[Tiempo] == NoConfigurado) {
    ConfigurarTiempo();
  }
  else if (Estado[Tiempo] >= Configurado) {
    unsigned long currentMillis = millis();

    if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
      prevNTP = currentMillis;
      Serial.println("\r\nSending NTP request ...");
      sendNTPpacket(timeServerIP);               // Send an NTP request
    }

    uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
    if (time) {                                  // If a new timestamp has been received
      timeUNIX = time;
      Serial.print("NTP response:\t");
      Serial.println(timeUNIX);
      lastNTPResponse = currentMillis;
    } else if ((currentMillis - lastNTPResponse) > 6000 * 60 * 5) {
      Serial.println("More than 1 hour since last NTP response. Rebooting.");
      Serial.flush();
      ESP.restart();
    }

    uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;
    if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print

      CambiarActualHora(getHours(actualTime));
      CambiarActualMinuto(getMinutes(actualTime));

      prevActualTime = actualTime;
      Estado[Tiempo] = Conectado;
     }
  }
}




inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  int D = UNIXTime / 3600 % 24;
  D = D + Diferente;
  if (D < 0) {
    D = 24 + D;
  }
  return D;
}
