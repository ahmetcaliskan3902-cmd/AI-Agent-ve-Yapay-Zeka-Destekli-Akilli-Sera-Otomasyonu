#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h> 
#include "DHT.h"
#include "time.h"

// --- WOKWI AĞ AYARLARI ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// N8N Webhook Adresi
const char* n8n_server = "https://ahmetslab3902.app.n8n.cloud/webhook-test/83b2ea0d-e6ab-4305-95a4-0197c50cc5b6";

// --- DONANIM PİNLERİ ---
#define DHTPIN 4       
#define DHTTYPE DHT22 

#define POT_PIN 34          

#define ROLE_SU_PIN 5       
#define ROLE_FAN_PIN 18     
#define ROLE_ISITICI_PIN 19 

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 10800; // UTC+3

//[SANAL DEĞİŞKENLER]
// başlangıç değerleri ideal 
float sanalSicaklik =25;
float sanalNem = 50.0;
int sanalToprak = 50;
int durgunlukSayaci = 0;

// --- ZAMAN FONKSİYONU ---
String gercekZamaniGetir(){
  struct tm zamanBilgisi;
  if(!getLocalTime(&zamanBilgisi)){
    return "2026-01-01T00:00:00Z"; 
  }
  char zamanString[30];
  strftime(zamanString, sizeof(zamanString), "%Y-%m-%dT%H:%M:%SZ", &zamanBilgisi);
  return String(zamanString);
}

// [SIMLUSYON SENARYOSU KODU]
void simulasyonSenaryosu() {
  // 1. rölelerin durumunu kontrol eden kod
  bool fanAcik = digitalRead(ROLE_FAN_PIN);
  bool isiticiAcik = digitalRead(ROLE_ISITICI_PIN);
  bool pompaAcik = digitalRead(ROLE_SU_PIN);

  // [IYILESME MODU]
  if (fanAcik) {
    sanalSicaklik -= 3.5;
    sanalNem -= 2.0;
    // idealin altına inmesini engelle
    if(sanalSicaklik < 24.0) sanalSicaklik = 24.0;
    durgunlukSayaci = 0;
    Serial.println(">> SIMULASYON: Fan calisiyor, sicaklik dusuruluyor...");
  } 
  else if(isiticiAcik){
    sanalSicaklik += 3.5;
    sanalNem -= 1.0;
    // ideal üstüne çıkamsını engelle
    if(sanalSicaklik > 26.0) sanalSicaklik = 26.0;
    durgunlukSayaci = 0;
    Serial.println(">> SIMULASYON: Isitici calisiyor, sicaklik artiyor...");
  }
  if (pompaAcik){
    sanalToprak +=15;
    if(sanalToprak > 60.0) sanalToprak = 60.0;
    durgunlukSayaci = 0;
    Serial.println(">>SIMULASYON: Sulama yapiliyor, nem artiyor..."); 
  }

  //KAOS SENARYOSU VE BEKLEME MODU
  if(!fanAcik && !isiticiAcik && !pompaAcik){

    durgunlukSayaci++;
    Serial.println(">> SIMULASYON: Sistem stabil. sayac: " + String(durgunlukSayaci));
    if (durgunlukSayaci >= 2) {
      Serial.println(">> !!! KAOS MODU TETIKLENDI (Hizli Senaryo) !!!");

      int senaryo = random(0, 2);

      if (senaryo == 0) {
        // SENARYO A: SICAK VE KURAK
        sanalSicaklik = 38.5;
        sanalToprak = 20;
      } else {
        // SENARYO B: Soğuk ve Nemli
        sanalSicaklik = 10.0;
        sanalNem = 85;
      }
      durgunlukSayaci = 0;
    }
  }
}

// --- WEB SUNUCU ---
//void handleSensorData() {
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  //int pot = analogRead(POT_PIN);
  //int nemYuzde = map(pot, 0, 4095, 0, 100);
  
  //String jsonResponse = "{\"sicaklik\":" + String(t) + ", \"hava_nemi\":" + String(h) + ", \"toprak_nemi\":" + String(nemYuzde) + "}";
  //server.send(200, "application/json", jsonResponse);
//}

void handleSensorData() {
  String jsonResponse = "{\"sicaklik\":" + String(sanalSicaklik) + ", \"hava_nemi\":" + String(sanalNem) + ", \"toprak_nemi\":" + String(sanalToprak) + "}";
  server.send(200, "application/json", jsonResponse);
}

void handleControlRelay() {
  server.send(200, "text/plain", "N8N kontrolu aktif.");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ROLE_SU_PIN, OUTPUT);
  pinMode(ROLE_FAN_PIN, OUTPUT);
  pinMode(ROLE_ISITICI_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT); 
  
  // --- DÜZELTME 1: Başlangıçta Hepsini Kapat (LOW yaparak) ---
  digitalWrite(ROLE_SU_PIN, LOW);    
  digitalWrite(ROLE_FAN_PIN, LOW);    
  digitalWrite(ROLE_ISITICI_PIN, LOW);

  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();

  Serial.print("WiFi Baglaniliyor...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Baglandi!");
  
  configTime(gmtOffset_sec, 0, ntpServer);

  server.on("/sensordata", HTTP_GET, handleSensorData);
  server.on("/controlrelay", HTTP_GET, handleControlRelay);
  server.begin();
  
  Serial.println("Sistem Hazir.");
  delay(1000);
}

void loop() {
  server.handleClient(); 

  static unsigned long sonZamanN8N = 0;
  
  if (millis() - sonZamanN8N > 30000){
    sonZamanN8N = millis();

    //senaryoyu işlet
    simulasyonSenaryosu();

    // Değerleri Sanal Değişkenden ata
    float t = sanalSicaklik;
    float h = sanalNem;
    int toprakYuzde = sanalToprak;

    //float t = dht.readTemperature();
    //float h = dht.readHumidity();
    //int hamDeger = analogRead(POT_PIN);
    //int toprakYuzde = map(hamDeger, 0, 4095, 0, 100);

    Serial.println("------------------------------------------------");
    Serial.print("Sicaklik: " + String(t) + "C | Toprak Nemi: %" + String(toprakYuzde));

    if (!isnan(t) && !isnan(h)){
      JsonDocument doc; 
      
      doc["cihaz_id"] = "sera_01";
      doc["sicaklik"] = t;
      doc["nem"] = h;
      doc["toprak_nemi"] = toprakYuzde;
      doc["zaman_damgasi"] = gercekZamaniGetir();

      String jsonPaketi;
      serializeJson(doc, jsonPaketi);

      if (WiFi.status() == WL_CONNECTED) {
          WiFiClientSecure client;
          client.setInsecure(); 
          HTTPClient http;
          
          if(http.begin(client, n8n_server)){
            http.setTimeout(30000);
            http.addHeader("Content-Type", "application/json");
            http.addHeader("ngrok-skip-browser-warning", "true");
            int httpKodu = http.POST(jsonPaketi);
            
            if(httpKodu > 0){
              Serial.println(" -> N8N'e Gonderildi.");
              
              String gelenCevap = http.getString();
              Serial.println("N8N Cevabi: " + gelenCevap);
              
              if(gelenCevap.length() > 0) {
                 JsonDocument cevapDoc;
                 DeserializationError hata = deserializeJson(cevapDoc, gelenCevap);

                 if (!hata) {
                   
                   // --- DÜZELTME 2: Mantık Ters Çevrildi (HIGH = AÇ, LOW = KAPAT) ---
                   
                   // SU VANASI
                   if (cevapDoc["komut_su"]) {
                     String c = cevapDoc["komut_su"].as<String>();
                     if (c == "ac") digitalWrite(ROLE_SU_PIN, HIGH); // Artık HIGH açıyor
                     else digitalWrite(ROLE_SU_PIN, LOW);            // LOW kapatıyor
                   }

                   // FAN
                   if (cevapDoc["komut_fan"]) {
                     String c = cevapDoc["komut_fan"].as<String>();
                     if (c == "ac") digitalWrite(ROLE_FAN_PIN, HIGH);
                     else digitalWrite(ROLE_FAN_PIN, LOW);
                   }

                   // ISITICI
                   if (cevapDoc["komut_isitici"]) {
                     String c = cevapDoc["komut_isitici"].as<String>();
                     if (c == "ac") digitalWrite(ROLE_ISITICI_PIN, HIGH);
                     else digitalWrite(ROLE_ISITICI_PIN, LOW);
                   }

                 } else {
                   Serial.println("JSON Hatasi!");
                 }
              }
            } else {
              Serial.println("HATA: Sunucuya ulasilamadi.");
            }
            http.end();
          }
      }
    } else {
      Serial.println("Sensor okuma hatasi!");
    }
  }
}
