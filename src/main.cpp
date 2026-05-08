#include <Arduino.h>
#include <ArduinoJson.h>
#include <Bounce2.h>

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"
#include "secrets.h"

Bounce botao = Bounce();

const char TOPICO_COMANDO[] = "senai/Nicolas/esp32/comando";
int modoAtualSemaforo = 0;

//Protótipos das funções
void publicarModo(int modo);

void setup()
{
  //Define o pino e modo do botao
  botao.attach(0, INPUT_PULLUP);

  //define o debounce do botao
  botao.interval(50);
  
  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  conectarMQTT();

  debugInfo("ESP Publisher iniciado. Modo atual: normal");
}
void loop()
{
  //atualiza o estado do botao
  botao.update(); 

  garantirWiFiConectado();
  garantirMQTTConectado();
  loopMQTT();

  if(botao.fell())
  {
  modoAtualSemaforo = (modoAtualSemaforo + 1) % 3;
  debugInfo("Modo semáforo: " + String(modoAtualSemaforo));
  publicarModo(modoAtualSemaforo);
  }
}

//publica a mensagem de acordo com o modo do semaforo
void publicarModo(int modo)
{
 JsonDocument doc;

 if(modo == 0)
 {
  doc["lampada"] = false;
  doc["modo_noturno"] = false;

  doc["led_verde"]["r"] = 0;
  doc["led_verde"]["g"] = 255;
  doc["led_verde"]["b"] = 0;

  doc["led_amarelo"]["r"] = 255;
  doc["led_amarelo"]["g"] = 255;
  doc["led_amarelo"]["b"] = 0;

  doc["led_vermelho"]["r"] = 255;
  doc["led_vermelho"]["g"] = 0;
  doc["led_vermelho"]["b"] = 0;

  debugInfo("Modo Fluxo normal");
 }
 
 else if(modo == 1)
 {
  doc["lampada"] = true;
  doc["modo_noturno"] = false;

  doc["led_verde"]["r"] = 0;
  doc["led_verde"]["g"] = 255;
  doc["led_verde"]["b"] = 0;

  doc["led_amarelo"]["r"] = 255;
  doc["led_amarelo"]["g"] = 255;
  doc["led_amarelo"]["b"] = 0;

  doc["led_vermelho"]["r"] = 255;
  doc["led_vermelho"]["g"] = 0;
  doc["led_vermelho"]["b"] = 0;

  debugInfo("Modo Fluxo alto");
 }

 else
 {
  doc["lampada"] = false;
  doc["modo_noturno"] = true;
  
  doc["led_verde"]["r"] = 0;
  doc["led_verde"]["g"] = 255;
  doc["led_verde"]["b"] = 0;
  
  doc["led_amarelo"]["r"] = 255;
  doc["led_amarelo"]["g"] = 255;
  doc["led_amarelo"]["b"] = 0;
  
  doc["led_vermelho"]["r"] = 255;
  doc["led_vermelho"]["g"] = 0;
  doc["led_vermelho"]["b"] = 0;
  
  debugInfo("Modo noturno");
 }
 String mensagem;
 serializeJson(doc, mensagem);
 publicarMensagem(TOPICO_COMANDO, mensagem.c_str());

 debugInfo("Mensagem publicada: " + mensagem);
}
