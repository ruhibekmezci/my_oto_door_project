# Arduino RFID Kapı Kilidi Projesi (RFID Door Lock System)

Bu proje, Arduino ve RC522 RFID modülü kullanılarak geliştirilmiş bir otomatik kapı kilidi prototipidir. Tanımlı bir RFID kartı veya anahtarlığı okutulduğunda servo motor tetiklenerek kapı mekanizmasını açar.

**Geliştiriciler:**
* Ruhi A. BEKMEZCİ
* İbrahim CİNCİ

## 🎥 Proje Görselleri

![Proje Genel Görünüm](https://user-images.githubusercontent.com/58533173/147697938-8b2762c8-1e5d-438b-beb2-96956da0576b.jpg)

## 🛠 Kullanılan Malzemeler

* Arduino Uno (veya uyumlu bir kart)
* MFRC522 RFID Okuyucu Modülü
* SG90 Servo Motor (veya benzeri)
* Jumper Kablolar
* Breadboard

## 🔌 Devre Şeması ve Bağlantılar

### Pin Konfigürasyonu

| RC522 Modülü | Arduino Pin |
|--------------|-------------|
| RST          | 9           |
| SDA (SS)     | 10          |
| MOSI         | 11 (SPI)    |
| MISO         | 12 (SPI)    |
| SCK          | 13 (SPI)    |
| 3.3V         | 3.3V        |
| GND          | GND         |

| Servo Motor  | Arduino Pin |
|--------------|-------------|
| Sinyal (Turuncu)| 8        |
| VCC (Kırmızı)| 5V          |
| GND (Kahve)  | GND         |

### Devre Görselleri
![Devre Şeması](https://user-images.githubusercontent.com/58533173/147698049-7a8f1b78-53c3-4269-992f-5e4eddafb9d0.jpg)
![Bağlantı Detayı 1](https://user-images.githubusercontent.com/58533173/147698950-54d3dc21-f6d2-41b0-adc8-fe2fd9af8944.JPEG)

## ⚙️ Kurulum ve Kullanım

1. **Kütüphaneleri Yükleyin:**
   Arduino IDE üzerinden aşağıdaki kütüphanelerin yüklü olduğundan emin olun:
   * `MFRC522` (Miguel Balboa)
   * `Servo` (Built-in)
   * `SPI` (Built-in)

2. **Kodu Yükleyin:**
   `ruhi_ibrahim_arduino_devrelab1_project.ino` dosyasını Arduino kartınıza yükleyin.

3. **Kart Tanımlama (Opsiyonel):**
   Kod içerisindeki `byte ID[4] = {142,111,230,63};` satırını kendi kartınızın UID değerleri ile değiştirerek yetkili kartı güncelleyebilirsiniz.

## 🚀 Çalışma Mantığı

1. Sistem başladığında servo motor kapıyı kilitli pozisyona (0 derece) getirir.
2. RFID okuyucu sürekli olarak kart taraması yapar.
3. Okunan kartın ID'si, kodda tanımlı yetkili ID ile eşleşirse:
    * Seri porta "Kapi acildi" mesajı gönderilir.
    * Servo motor 180 derece dönerek kapıyı açar.
    * 3 saniye beklendikten sonra kapı otomatik olarak kilitlenir.
4. Yetkisiz bir kart okutulursa seri porta "Yetkisiz Kart" uyarısı düşer ve kilit açılmaz.

---
*Not: Bu proje eğitim amaçlı geliştirilmiştir.*
