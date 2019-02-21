boolean EstadoAlarma = false;
boolean EstadoBonba = false;
int HoraAlarma = -1;
int MinutoAlarma = -1;

int HoraActual = -1;
int MinutoActual = -1;
long TiempoBonba = 0;

boolean ObtenerAlarma() {
  return EstadoAlarma;
}

void CambiarAlarma(boolean E) {
  if (E) {
    Estado[Alarma] = Conectado;
  }
  else {
    Estado[Alarma] = NoConfigurado;
  }
  EstadoAlarma = E;
}

void CambiarAlarmaHora(int Hora) {
  HoraAlarma = Hora;
  //Serial.print("Actualizando hora Alarma a ");
  //Serial.println(HoraAlarma);
}

int AlarmaHora() {
  return HoraAlarma;
}

void CambiarAlarmaMinuto(int Minuto) {
  MinutoAlarma = Minuto;
  //Serial.print("Actualizando minuto Alarma a ");
  //Serial.println(MinutoAlarma);
}

int AlarmaMinuto() {
  return MinutoAlarma;
}

void CambiarActualHora(int Hora) {
  HoraActual = Hora;
}

int ActualHora() {
  return HoraActual;
}

void CambiarActualMinuto(int Minuto) {
  MinutoActual = Minuto;
}

int ActualMinuto() {
  return MinutoActual;
}

void CambiarBonba(boolean Estado) {
  if (Estado) {
    digitalWrite(PinBomba, 1);
    TiempoBonba = millis();
  }
  else {
    digitalWrite(PinBomba, 0);
  }
  EstadoBonba = Estado;
}

boolean ObtenerBonba() {
  return EstadoBonba;
}

void ActualizarAlarma() {

  if (digitalRead(PinBoton) == 1) {
    digitalWrite(PinBomba, 0);
    Serial.println("Apagnado Alarma");
    while (digitalRead(PinBoton) == 1) {
      delay(100);
    }
    EstadoBonba = false;
  }

  if (EstadoAlarma) {
    if (HoraAlarma != -1 && MinutoAlarma != -1) {
      if (HoraAlarma == HoraActual) {
        if (MinutoActual == MinutoAlarma) {
          EstadoBonba = true;
          Serial.println("Hora de levantarse :) ");
          digitalWrite(PinBomba, 1);
          TiempoBonba = millis();
        }
      }
    }
  }

  if (EstadoBonba) {
    if (millis() - TiempoBonba > 1000 * 60) {
      EstadoBonba = false;
      Serial.println("Sistema AutoApagado");
      digitalWrite(PinBomba, 0);
    }
  }
}
