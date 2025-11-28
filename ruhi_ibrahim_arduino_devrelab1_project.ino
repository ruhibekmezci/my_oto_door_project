#include <MFRC522.h>
#include <Servo.h>
#include <SPI.h>

// Pinleri define ile tanımlıyoruz, RAM'den tasarruf sağlar.
#define RST_PIN 9
#define SS_PIN 10
#define SERVO_PIN 8

// Sabitler
const byte YETKILI_ID[4] = {142, 111, 230, 63}; // Yetkili kart ID'si
const unsigned long KAPI_ACIK_SURESI = 3000;    // Kapının açık kalacağı süre (ms)

// Global Değişkenler
Servo motor;
MFRC522 rfid(SS_PIN, RST_PIN);
unsigned long sonIslemZamani = 0;
bool kapiAcik = false;

void setup() {
  motor.attach(SERVO_PIN);
  motor.write(0); // Başlangıçta kapıyı kilitli pozisyona al
  
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  Serial.println(F("Sistem Hazir. Kart Okutunuz...")); // F() makrosu string'i flash bellekte tutar
}

void loop() {
  // Zamanlayıcı kontrolü (Non-blocking delay mantığı)
  if (kapiAcik && (millis() - sonIslemZamani >= KAPI_ACIK_SURESI)) {
    kapiyiKapat();
  }

  // Yeni kart var mı ve okunabiliyor mu kontrolü
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // ID Kontrolü
  if (idKontrol(rfid.uid.uidByte)) {
    Serial.println(F("Giris Onaylandi."));
    kapiyiAc();
  } else {
    Serial.println(F("Yetkisiz Giris Denemesi!"));
    ekranaYazdir(); // Hangi ID'nin reddedildiğini görelim
  }

  rfid.PICC_HaltA();        // Kartı durdur
  rfid.PCD_StopCrypto1();   // Kriptoyu durdur
}

// ID Karşılaştırma Fonksiyonu
bool idKontrol(byte* okunanID) {
  // Kart ID uzunluğu yetkili ID uzunluğu ile aynı olmalı
  if (rfid.uid.size != sizeof(YETKILI_ID)) {
    return false;
  }

  for (byte i = 0; i < 4; i++) {
    if (okunanID[i] != YETKILI_ID[i]) {
      return false;
    }
  }
  return true;
}

void kapiyiAc() {
  // Kapı zaten açıksa sadece süreyi sıfırla (uzat)
  if (!kapiAcik) {
    motor.write(180);
    Serial.println(F("Kapi Acildi."));
    kapiAcik = true;
  }
  sonIslemZamani = millis(); // Sayacı başlat/sıfırla
}

void kapiyiKapat() {
  motor.write(0);
  Serial.println(F("Sure Doldu, Kapi Kapatildi."));
  kapiAcik = false;
}

void ekranaYazdir() {
  Serial.print("Okunan ID: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(rfid.uid.uidByte[i]);
    Serial.print(" ");
  }
  Serial.println();
}
