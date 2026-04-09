#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <DHT.h>

// --- Konfigurasi WiFi ---
const char* ssid = "Ndy";                 
const char* password = "YakaliBro";       

// --- Konfigurasi Telegram Bot ---
#define BOT_TOKEN "8618799729:AAF0v9WQyqxbrWU8-LBS3_IaLqvt9p8fXHE" 

// --- KONFIGURASI HAK AKSES (TELEGRAM ID) ---
#define ID_MEMBER_A "5569342487"  
#define ID_MEMBER_B "5464650308"  
#define ID_MEMBER_C "8625380459"  
#define ID_MEMBER_D "1339788081"


WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// --- Konfigurasi Pin ESP32-C3 ---
#define DHTPIN 2
#define LED_A_PIN 3      // LED Pribadi Anggota A
#define LED_B_PIN 7      // LED Pribadi Anggota B
#define LED_C_PIN 10     // LED Pribadi Anggota C
#define LED_D_PIN 8
#define LED_UTAMA_PIN 5  // LED Ruang Tamu (Bisa diakses siapa saja)

#define MQ2_AO_PIN 4     // Pin Analog MQ-2

// --- Konfigurasi DHT ---
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id; // Tempat balasan dikirim (Grup / Private)
    String from_id = bot.messages[i].from_id; // ID Orang yang mengirim perintah
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    // 1. CEK HAK AKSES KELOMPOK
    bool isMember = (from_id == ID_MEMBER_A || from_id == ID_MEMBER_B || from_id == ID_MEMBER_C || from_id == ID_MEMBER_D);

    if (!isMember) {
      String unauthorized_msg = "Maaf " + from_name + ", Anda bukan anggota kelompok dan tidak memiliki akses.";
      bot.sendMessage(chat_id, unauthorized_msg, "");
      continue; // Hentikan proses untuk orang asing
    }

    // --- MENU UTAMA ---
    if (text == "/start") {
      String welcome = "🏠 **Smart House Kelompok**\nHalo " + from_name + "!\n\n";
      welcome += "💡 **LED Pribadi:**\n";
      welcome += "/leda_on | /leda_off (Hanya A)\n";
      welcome += "/ledb_on | /ledb_off (Hanya B)\n";
      welcome += "/ledc_on | /ledc_off (Hanya C)\n\n";
      welcome += "/ledD_on | /ledD_off (Hanya D)\n\n";
      welcome += "🛋️ **LED Ruang Tamu (Bebas):**\n";
      welcome += "/ledutama_on | /ledutama_off\n\n";
      welcome += "📊 **Sensor (Bebas):**\n";
      welcome += "/status - Cek Suhu & Kelembapan\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    // --- KONTROL LED A (Hanya untuk Member A) ---
    else if (text == "/leda_on") {
      if (from_id == ID_MEMBER_A) {
        digitalWrite(LED_A_PIN, HIGH);
        bot.sendMessage(chat_id, "💡 LED A dinyalakan oleh " + from_name, "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak! Hanya Anggota A yang bisa menyalakan LED ini.", "");
      }
    }
    else if (text == "/leda_off") {
      if (from_id == ID_MEMBER_A) {
        digitalWrite(LED_A_PIN, LOW);
        bot.sendMessage(chat_id, "🌑 LED A dimatikan.", "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak!", "");
      }
    }

    // --- KONTROL LED B (Hanya untuk Member B) ---
    else if (text == "/ledb_on") {
      if (from_id == ID_MEMBER_B) {
        digitalWrite(LED_B_PIN, HIGH);
        bot.sendMessage(chat_id, "💡 LED B dinyalakan oleh " + from_name, "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak! Hanya Anggota B yang bisa menyalakan LED ini.", "");
      }
    }
    else if (text == "/ledb_off") {
      if (from_id == ID_MEMBER_B) {
        digitalWrite(LED_B_PIN, LOW);
        bot.sendMessage(chat_id, "🌑 LED B dimatikan.", "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak!", "");
      }
    }

    // --- KONTROL LED C (Hanya untuk Member C) ---
    else if (text == "/ledc_on") {
      if (from_id == ID_MEMBER_C) {
        digitalWrite(LED_C_PIN, HIGH);
        bot.sendMessage(chat_id, "💡 LED C dinyalakan oleh " + from_name, "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak! Hanya Anggota C yang bisa menyalakan LED ini.", "");
      }
    }
    else if (text == "/ledc_off") {
      if (from_id == ID_MEMBER_C) {
        digitalWrite(LED_C_PIN, LOW);
        bot.sendMessage(chat_id, "🌑 LED C dimatikan.", "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak!", "");
      }
    }

    // --- KONTROL LED D (Hanya untuk Member D) ---
    else if (text == "/ledD_on") {
      if (from_id == ID_MEMBER_D) {
        digitalWrite(LED_D_PIN, HIGH);
        bot.sendMessage(chat_id, "💡 LED D dinyalakan oleh " + from_name, "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak! Hanya Anggota C yang bisa menyalakan LED ini.", "");
      }
    }
    else if (text == "/ledD_off") {
      if (from_id == ID_MEMBER_D) {
        digitalWrite(LED_D_PIN, LOW);
        bot.sendMessage(chat_id, "🌑 LED D dimatikan.", "");
      } else {
        bot.sendMessage(chat_id, "❌ Akses Ditolak!", "");
      }
    }

    // --- KONTROL LED RUANG TAMU (Semua Anggota Bisa) ---
    else if (text == "/ledutama_on") {
      digitalWrite(LED_UTAMA_PIN, HIGH);
      bot.sendMessage(chat_id, "🛋️💡 LED Ruang Tamu dinyalakan oleh " + from_name, "");
    }
    else if (text == "/ledutama_off") {
      digitalWrite(LED_UTAMA_PIN, LOW);
      bot.sendMessage(chat_id, "🛋️🌑 LED Ruang Tamu dimatikan oleh " + from_name, "");
    }

    // --- CEK STATUS SENSOR (Semua Anggota Bisa) ---
    else if (text == "/status") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      int gasAnalog = analogRead(MQ2_AO_PIN);

      String statusMsg = "📊 **Status Rumah Saat Ini:**\n\n";
      statusMsg += "🌡️ Suhu: " + String(t) + " °C\n";
      statusMsg += "💧 Kelembapan: " + String(h) + " %\n";
      statusMsg += "💨 Kadar Gas (Analog): " + String(gasAnalog) + "\n";
      
      if (gasAnalog > 2500) {
        statusMsg += "⚠️ Status Gas: KEBOCORAN TERDETEKSI!\n";
      } else {
        statusMsg += "✅ Status Gas: Aman\n";
      }

      bot.sendMessage(chat_id, statusMsg, "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(LED_C_PIN, OUTPUT);
  pinMode(LED_D_PIN, OUTPUT);
  pinMode(LED_UTAMA_PIN, OUTPUT);

  digitalWrite(LED_A_PIN, LOW);
  digitalWrite(LED_B_PIN, LOW);
  digitalWrite(LED_C_PIN, LOW);
  digitalWrite(LED_UTAMA_PIN, LOW);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // --- SISTEM PERINGATAN GAS KE GRUP ---
  static bool gasAlertSent = false;
  int currentGasLevel = analogRead(MQ2_AO_PIN); 
  
  if (currentGasLevel > 2500) {
    if (!gasAlertSent) {
      String alertMsg = "🚨 **BAHAYA PANGGILAN DARURAT!**\n";
      alertMsg += "Terdeteksi kebocoran gas di rumah!\n";
      alertMsg += "Kadar Gas: " + String(currentGasLevel) + "\n";
      alertMsg += "Segera evakuasi dan periksa sistem!";
      
      // Kirim Notifikasi langsung ke ID Grup, bukan membalas pesan private!
      
      gasAlertSent = true; 
    }
  } else {
    // Reset jika gas sudah normal kembali
    gasAlertSent = false; 
  }

  delay(10);
}
