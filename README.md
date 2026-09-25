# TLS_Mikrokontroler_Tourmaline
Sistem alarm keamanan penyebrangan pedestrian berbasis ESP32 dengan sensor PIR, buzzer

# Sistem Alarm Keamanan Penyeberangan Pedestrian Berbasis ESP32

Sistem peringatan otomatis berbasis ESP32 untuk meningkatkan keamanan penyeberangan 
pedestrian (zebra cross). Sensor PIR mendeteksi keberadaan/gerakan pejalan kaki di 
area penyeberangan, lalu sistem memberikan peringatan melalui buzzer sebagai tanda 
bagi pengendara untuk berhati-hati, sekaligus mengirim notifikasi real-time ke 
Telegram sebagai laporan aktivitas penyeberangan.

## Fitur
- Deteksi keberadaan/gerakan pejalan kaki di zona penyeberangan menggunakan sensor PIR
- Buzzer otomatis berbunyi sebagai peringatan dini bagi pengendara saat ada pejalan kaki menyeberang
- Notifikasi instan ke Telegram setiap kali terdeteksi aktivitas penyeberangan
- Tombol fisik untuk mengaktifkan/menonaktifkan (arm/disarm) sistem
- Indikator LED sebagai penanda visual tambahan saat sistem mendeteksi pejalan kaki
- Terhubung ke Wi-Fi (ESP32) untuk komunikasi dengan Telegram Bot API

## Komponen yang digunakan
- ESP32 (mikrokontroler utama)
- Sensor PIR (deteksi gerakan pejalan kaki)
- LED indikator
- Passive Buzzer (peringatan suara)
- Push Button 4-pin (kontrol arm/disarm sistem)

## Cara Kerja
1. ESP32 terhubung ke jaringan Wi-Fi saat dinyalakan
2. Saat sistem dalam status *armed*, sensor PIR aktif memantau area penyeberangan
3. Jika sensor mendeteksi pejalan kaki → LED menyala dan buzzer berbunyi sebagai 
   peringatan bagi pengendara di sekitar area penyeberangan
4. Sistem otomatis mengirim notifikasi ke Telegram sebagai laporan/log aktivitas 
   penyeberangan
5. Petugas/operator dapat mengaktifkan atau menonaktifkan sistem melalui tombol fisik, 
   atau menerima perintah balik lewat Telegram

## Simulasi & Demo
- 🔗 Simulasi Wokwi (Tanpa Telegram Bot): https://wokwi.com/projects/476143781894578177
- 🔗 Simulasi Wokwi (Telegram Bot): https://wokwi.com/projects/476143621384806401
- 🎥 Video Demonstrasi

## Library yang dibutuhkan
- WiFi.h
- HTTPClient.h
- WiFiClientSecure.h
