const TelegramBot = require('node-telegram-bot-api');
let date = require('date-and-time');
var mqtt = require('mqtt');
const fs = require('fs');
let token = require('./Token.js')
var Estado = 0;
//const token = '520821559:AAHYQSi0MiXtQlckbkkO8oqSye7BZoMGeG0';
const bot = new TelegramBot(token, {
  polling: true
});
var client = mqtt.connect('mqtt://broker.mqtt-dashboard.com')
let Hora, Minuto;
let EstadoAlarma = "No Definido";
bot.on('message', (msg) => {
  const chatId = msg.chat.id;
  var mensaje = msg.text;

  console.log("Mensaje es :" + mensaje);
  if (Estado == 0) {
    if (mensaje == "Hola" || mensaje == "Ayuda") {
      bot.sendMessage(chatId, 'Listo para empezar a usar la Alarma Inteligente  de ALSW, pueva con los siquientes comandos:')
        .then(() => {
          return bot.sendMessage(chatId, '*EncenderAlarma - Enciende la Alarma a la hora programada')
        })
        .then(() => {
          return bot.sendMessage(chatId, '*ApagarAlarma - Apagar la Alarma')
        })
        .then(() => {
          return bot.sendMessage(chatId, '*EncenderBonba - Enciende la bomba de Agua ')
        })
        .then(() => {
          return bot.sendMessage(chatId, '*ApagarBonba - Apagar la bonba de Agua')
        })
        .then(() => {
          return bot.sendMessage(chatId, '*Programar - Asigna Hora y Minuto')
        })
        .then(() => {
          return bot.sendMessage(chatId, '*Estado - Da informacion alarma')
        })
    } else if (mensaje == "EncenderAlarma" || mensaje == "Ea") {
      bot.sendMessage(chatId, 'Activando alarma se encedera a la hora programada');
      client.publish('ALSWAlarmaEstado', '1');
    } else if (mensaje == "ApagarAlarma" || mensaje == "Aa") {
      bot.sendMessage(chatId, 'Desactivando alarma');
      client.publish('ALSWAlarmaEstado', '0');
    } else if (mensaje == "EncenderBonba" || mensaje == "Eb") {
      bot.sendMessage(chatId, 'La bamba de agua esta ACTIVA');
      client.publish('ALSWAlarmaBonba', '1');
    } else if (mensaje == "ApagarBonba" || mensaje == "AB" || mensaje == "Apagar") {
      bot.sendMessage(chatId, 'La bonba de agua esta APAGADA');
      client.publish('ALSWAlarmaBonba', '0');
    } else if (mensaje == "Programar" || mensaje == "P") {
      bot.sendMessage(chatId, 'A que hora quiere programarlo:');
      Estado = 1;
    } else if (mensaje == "Estado" || mensaje == "E") {
      fs.readFile('./UltimaAlarma.cvs', function leer(erro, data) {
        bot.sendMessage(chatId, 'Ultima reporte Alarma ' + data + " " + EstadoAlarma);
        console.log(data + " Id " + chatId);
      });
    } else if (mensaje == "Preguntar" || mensaje == "?") {
      bot.sendMessage(chatId, 'Alarma Programada para ' + Hora + ':' + Minuto);
    }
  } else if (Estado == 1) {
    fs.writeFile('./hora.cvs', mensaje, function(err) {
      console.log("Salvado");
      bot.sendMessage(chatId, 'A que Minuto quiere programarlo:');
      client.publish('ALSWAlarmaHora', mensaje);
      Hora = mensaje;
      Estado = 2;
    })
  } else if (Estado == 2) {
    fs.writeFile('./Minuto.cvs', mensaje, function(err) {
      console.log("Salvado");
      bot.sendMessage(chatId, 'Alarma guarda consulta con Preguntar');
      client.publish('ALSWAlarmaMinuto', mensaje);
      Minuto = mensaje;
      Estado = 0;
    })
  }
});

client.on('connect', function() {
  console.log("Activado Mqtt")
  client.subscribe('EstadoAlarmaALSW', function(err) {})
  client.subscribe('ALSWAlarmaVivo', function(err) {})

  fs.readFile('./hora.cvs', function leer(erro, data) {
    Hora = data;
    console.log("La hora de alarma " + data);
  });


  fs.readFile('./Minuto.cvs', function leer(erro, data) {
    Minuto = data;
    console.log("Lo minuto de alarma " + data);
  });
});

client.on('message', function(topic, message) {
  if (topic.toString() == "ALSWAlarmaVivo") {
    client.publish('ALSWAlarmaHora', Hora);
    client.publish('ALSWAlarmaMinuto', Minuto);
    EstadoAlarma = message;
    let now = new Date();
    let UltimaHora = date.format(now, 'DD/MM/YYYY HH:mm:ss');
    fs.writeFile('./UltimaAlarma.cvs', UltimaHora, function(err) {})
  } else {
    console.log("[" + topic + "] - " + message.toString())
  }
});
