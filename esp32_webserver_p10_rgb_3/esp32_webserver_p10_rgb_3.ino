/*
 
2025-01-28 
04:23

2025-01-28
14:54

{"adjhijr":"4","adjcerah":"13","adjvolumeadzan":"15","adjvolume":"19","beepstatus":"0","modetampil":"1","relayon":"15:05","relayoff":"19:09","speedtext":"6","msholbi":"9","msholju":"5","jambesar":"135"}
{"latitude":"-6.21","longitude":"106.85","zonawaktu":"7"}
{"korimsak":"2","korsubuh":"2","korterbit":"2","kordhuha":"2","kordzuhur":"2","korashar":"2","kormaghrib":"2","korisya":"2"}
{"tpl_imsak":"1","tpl_subuh":"1","tpl_terbit":"0","tpl_dhuha":"0","tpl_dzuhur":"1","tpl_ashar":"1","tpl_maghrib":"1","tpl_isya":"1"}
{"namamasjid":"Nama Masjid","info1":"Info 1","info2":"Info 2","info3":"Tempat Info 3","stlhiqomah":"Matikan HP - Luruskan Shaf"}
{"tpl_nama":"1","tpl_pesan":"0","tpl_pesan2":"1","tpl_pesan3":"0","tpl_hijriyah":"0","tpl_masehi":"1","tpl_matikan":"0"}
{"lamaadzan":"4","lamaimsak":"1","iqomahsubuh":"2","iqomahdzuhur":"2","iqomahashar":"2","iqomahmaghrib":"2","iqomahisya":"2"}
{"imsak":"24|08:14","subuh":"29|06:27","dzuhur":"22|18:48","ashar":"23|15:07","maghrib":"25|18:51","isya":"27|14:36","jumat":"21|22:14"}
{"col_jam":"#ff0000","col_waktu":"#ffff00","col_info":"#00ffff","col_jam_adzan":"#00ff00","col_adzan":"#ff00ff","col_jam_iqomah":"#00ffff","col_iqomah":"#a600ff","col_shaf":"#0000ff"}

==================================
esp32 versi 1.0.6 & 3.1.2 
Board : DOIT ESP32 DEVKIT V1

Library :

arduinojson versi 7.1.0
RTClib versi 2.1.4
SholahTimeCalulation
ESP32 HUB75 LED MATRIX PANEL DMA Display versi 3.0.8
Adafruit GFX Library versi 1.2.3
Adafruit BusIO versi 1.16.1
DFPlayer_Mini_Mp3

==================================

dibuat oleh Yopi Ardinal
ilmunya diambil dari berbagai sumber
( maaf ga bisa disebut satu persatu karena udah lupa )

*/

/* Library */
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPUpdateServer.h>
#include "RTClib.h"
#include "SholahTimeCalulation.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "Adafruit_GFX.h"
#include "DFPlayer_Mini_Mp3.h"
#include "ArduinoJson.h"

#include "Fonts/BigNumber.h"           // Untuk jam besar
#include "Fonts/Font4x7.h"             // Untuk nama sholat
#include "Fonts/Font5x7.h"             // Untuk jam sholat
#include "Fonts/arabangka.h"
#include "Fonts/fonttebal.h"

#include "tanggal_hijriyah.h"
#include "json.h"

/* MP3 Player */
#define RXD 3  //RXD
#define TXD 1  //TXD
HardwareSerial mp3player(2);

/* Access Point */
const char* ssid = "ESP32-P10-RGB";
const char* password = "00000000";

/* Web Server */
WebServer httpServer(80);
HTTPUpdateServer httpUpdater;

/* P10 Panel */
#define PANEL_WIDTH 32   // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_HEIGHT 16  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_NUMBER 1

MatrixPanel_I2S_DMA *Disp = nullptr;

/* Buzzer & Relay */
#define Buzzer 32  // 32 buat di pcb

/* RTC */
RTC_DS3231 rtc;

/* Pilihan Mode */
byte mode = 1;

String pesan;

/* Waktu */
int Tahun;
byte Jam, Menit, Detik;
byte Bulan, Tanggal, hariIni;
String txhh, txmm, txss, cck, cck1, str_dt_plus;  // Untuk cocokkan waktu sekarang dengan waktu sholat
String tx_hh_mm, waktu_sholat, jws_skrng;

int j_s_s = 0;//, wkt;
/* Ganti Tanggal Maghrib */
float floatnow = 0;
float floatmagrib = 0;

/* Millis */
unsigned long p_e_1_jam = 0;

byte flag_mp3 = 0, flag_tartil = 1, flag_dt_plus = 0, flag_relay = 0; // 0 untuk true - 1 untuk false;
String waktu_tbt, waktu_dhu, waktu_sbh, waktu_dzr, waktu_ims, waktu_asr, waktu_mgr, waktu_isy; //, jss;
int durasi_tampil_adzan;
String wkt_tartil_ims, wkt_tartil_sbh, wkt_tartil_dzr, wkt_tartil_asr, wkt_tartil_mgr, wkt_tartil_isy;

String wkt_iqomah;//, tpl_iqo_ss;
int iqm_ee, du_iq, iqm_menit, iqm_menit_sisa, iqm_detik;  // sis_wkt_iqo Ngasih nama variable seenaknya aja

int ash, bsh;
String tx_ser, i_pesan;
int16_t textMin, textJam, textAdzan, textIqo, textCdiqo;
int16_t textX = 32;
int16_t panjangstring;

const char namaBulanMasehi[12][12] PROGMEM = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
const char namaHariMasehi[7][7] PROGMEM = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};
const char namaWaktuSholat[9][12] PROGMEM = {"Imsak ", " Subuh ", " Terbit ", " Dhuha ", " Dzuhur ", " Ashar ", " Maghrib ", " Isya ", " Jumat "};
const char pasar[5][7] = { "Wage", "Kliwon", "Legi", "Pahing", "Pon" };

String slt_sek;
byte wkt_msk_skrg = 0;

String b_tar_all, wkt_tartil_all;
int tar_now;

void setup() {
  Serial.begin(9600);
  mp3player.begin(9600, SERIAL_8N1, -1, TXD);

  rtc.begin();

  /* ===== LOAD FILE SYSTEM ===== */
  SPIFFS.begin();
  loadAturConfig(fileatur);
  loadWarna(filewarna);
  loadLokasiConfig(filelokasi);
  loadKoreksiConfig(filekoreksi);
  loadMasjidinfoConfig(filemasjidinfo);
  loadTampilinfoConfig(filetampilinfo);
  loadIqomahConfig(fileiqomah);
  loadwaktuTartilConfig(filewaktutartil);
  loadTampiljwsConfig(filetampiljws);
  loadUserConfig(fileuser);

  /* ===== 1. DISPLAY (JANGAN TERBALIK) ===== */
  HUB75_I2S_CFG mxconfig;
  mxconfig.mx_width = PANEL_WIDTH;
  mxconfig.mx_height = PANEL_HEIGHT;
  mxconfig.chain_length = PANEL_NUMBER;
  mxconfig.double_buff = true;
  mxconfig.clkphase = false;

  Disp = new MatrixPanel_I2S_DMA(mxconfig);
  Disp->begin();
  Disp->setBrightness8(adjcerah); //0-255
  Disp->setTextWrap(false);
  Disp->clearScreen();
  
  /* ===== 2. ACCESS POINT (JANGAN TERBALIK) ===== */
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  /* ===== 3. WEB SERVER ===== */
  
  // =========================
  // waktu
  // =========================
  httpServer.on("/waktu", []() {  // Pengaturan jam dan tanggal

    if (httpServer.hasArg("isi_tanggal")) {

      // isi_tanggal=2022-10-13&isi_jam=16:04  // tgl_auto=2023-05-15&wkt_auto=06:07

      int dt_tanggal = (httpServer.arg("isi_tanggal")).substring(8, 10).toInt();
      int dt_bulan = (httpServer.arg("isi_tanggal")).substring(5, 7).toInt();
      int dt_tahun = (httpServer.arg("isi_tanggal")).substring(0, 4).toInt();

      String arg_jam = httpServer.arg("isi_jam");

      int dt_jam = (arg_jam.substring(0, 2)).toInt();
      int dt_menit = (arg_jam.substring(3, 5)).toInt();

      Serial.println(httpServer.arg("isi_tanggal"));
      Serial.println(httpServer.arg("isi_jam"));

       rtc.adjust(DateTime(dt_tahun, dt_bulan, dt_tanggal, dt_jam, dt_menit, 0));
       if (beepstatus == 1) { beep(100); }
    }
  });
  
  // koordinat
  // =========================
  httpServer.on("/koordinat", []() {
    if (httpServer.hasArg("latitude")) {
      String datakordinat = "{\"latitude\":\"" + httpServer.arg("latitude") +
                            "\",\"longitude\":\"" + httpServer.arg("longitude") +
                            "\",\"zonawaktu\":\"" + httpServer.arg("zonawaktu") +
                            "\"}";
      // Serial.println(datakordinat);
      handleSimpanJson(datakordinat, 150, filelokasi);
    }
  });
  
  // informasi
  // =========================
  httpServer.on("/info", []() {

    if (httpServer.hasArg("namamasjid")) {
      String datanamamasjid = "{\"namamasjid\":\"" + httpServer.arg("namamasjid") +
                              "\",\"info1\":\"" + httpServer.arg("info1") +
                              "\",\"info2\":\"" + httpServer.arg("info2") +
                              "\",\"info3\":\"" + httpServer.arg("info3") +
                              "\",\"stlhiqomah\":\"" + httpServer.arg("stlhiqomah") +
                              "\"}";
      // Serial.println(datanamamasjid);
      handleSimpanJson(datanamamasjid, 800, filemasjidinfo);
    }
  });
  // tampilkan jadwal
  // =========================
  httpServer.on("/tampilkanjadwal", []() {
    if (httpServer.hasArg("tpl_imsak")) {
      String datatampiljadwal = "{\"tpl_imsak\":\"" + httpServer.arg("tpl_imsak") +
                                "\",\"tpl_subuh\":\"" + httpServer.arg("tpl_subuh") +
                                "\",\"tpl_terbit\":\"" + httpServer.arg("tpl_terbit") +
                                "\",\"tpl_dhuha\":\"" + httpServer.arg("tpl_dhuha") +
                                "\",\"tpl_dzuhur\":\"" + httpServer.arg("tpl_dzuhur") +
                                "\",\"tpl_ashar\":\"" + httpServer.arg("tpl_ashar") +
                                "\",\"tpl_maghrib\":\"" + httpServer.arg("tpl_maghrib") +
                                "\",\"tpl_isya\":\"" + httpServer.arg("tpl_isya") +
                                "\"}";
      // Serial.println(datatampiljadwal);
      handleSimpanJson(datatampiljadwal, 260, filetampiljws);
    }
  });
  
  // koreksi
  // =========================
  httpServer.on("/koreksi", []() {

    if (httpServer.hasArg("korimsak")) {
      String datakoreksi = "{\"korimsak\":\"" + httpServer.arg("korimsak") +
                           "\",\"korsubuh\":\"" + httpServer.arg("korsubuh") +
                           "\",\"korterbit\":\"" + httpServer.arg("korterbit") +
                           "\",\"kordhuha\":\"" + httpServer.arg("kordhuha") +
                           "\",\"kordzuhur\":\"" + httpServer.arg("kordzuhur") +
                           "\",\"korashar\":\"" + httpServer.arg("korashar") +
                           "\",\"kormaghrib\":\"" + httpServer.arg("kormaghrib") +
                           "\",\"korisya\":\"" + httpServer.arg("korisya") +
                           "\"}";
      // Serial.println(datakoreksi);
      handleSimpanJson(datakoreksi, 300, filekoreksi);
    }
  });  
  
  // tampilkan informasi
  // =========================
  httpServer.on("/tampilkaninfo", []() {
    if (httpServer.hasArg("tpl_nama")) {
      String datatplinfo = "{\"tpl_nama\":\"" + httpServer.arg("tpl_nama") +
                           "\",\"tpl_pesan\":\"" + httpServer.arg("tpl_pesan") +
                           "\",\"tpl_pesan2\":\"" + httpServer.arg("tpl_pesan2") +
                           "\",\"tpl_pesan3\":\"" + httpServer.arg("tpl_pesan3") +
                           "\",\"tpl_hijriyah\":\"" + httpServer.arg("tpl_hijriyah") +
                           "\",\"tpl_masehi\":\"" + httpServer.arg("tpl_masehi") +
                           "\",\"tpl_matikan\":\"" + httpServer.arg("tpl_matikan") +
                           "\"}";
      // Serial.println(datatplinfo);
      handleSimpanJson(datatplinfo, 260, filetampilinfo);
    }
  }); 
  
  // adzan & iqomah
  // =========================
  httpServer.on("/adzaniqomah", []() {
    if (httpServer.hasArg("lamaadzan")) {
      String dataadzaniqo = "{\"lamaadzan\":\"" + httpServer.arg("lamaadzan") +
                            "\",\"lamaimsak\":\"" + 1 +
                            "\",\"iqomahsubuh\":\"" + httpServer.arg("iqomahsubuh") +
                            "\",\"iqomahdzuhur\":\"" + httpServer.arg("iqomahdzuhur") +
                            "\",\"iqomahashar\":\"" + httpServer.arg("iqomahashar") +
                            "\",\"iqomahmaghrib\":\"" + httpServer.arg("iqomahmaghrib") +
                            "\",\"iqomahisya\":\"" + httpServer.arg("iqomahisya") +
                            "\"}";
      // Serial.println(dataadzaniqo);
      handleSimpanJson(dataadzaniqo, 300, fileiqomah);
    }
  });
  
  // tartil
  // =========================
  httpServer.on("/tartil", []() {
    if (httpServer.hasArg("imsak")) {
      String dataatartil = "{\"imsak\":\"" + httpServer.arg("imsak") +
                           "\",\"subuh\":\"" + httpServer.arg("subuh") +
                           "\",\"dzuhur\":\"" + httpServer.arg("dzuhur") +
                           "\",\"ashar\":\"" + httpServer.arg("ashar") +
                           "\",\"maghrib\":\"" + httpServer.arg("maghrib") +
                           "\",\"isya\":\"" + httpServer.arg("isya") +
                           "\",\"jumat\":\"" + httpServer.arg("jumat") +
                           "\"}";
      // Serial.println(dataatartil);
      handleSimpanJson(dataatartil, 300, filewaktutartil);
    }
  });  
  
  // warna
  // =========================
  httpServer.on("/warna", []() {
    if (httpServer.hasArg("col_jam")) {
      String dataawarna = "{\"col_jam\":\"" + httpServer.arg("col_jam") +
                           "\",\"col_waktu\":\"" + httpServer.arg("col_waktu") +
                           "\",\"col_info\":\"" + httpServer.arg("col_info") +
                           "\",\"col_jam_adzan\":\"" + httpServer.arg("col_jam_adzan") +
                           "\",\"col_adzan\":\"" + httpServer.arg("col_adzan") +
                           "\",\"col_jam_iqomah\":\"" + httpServer.arg("col_jam_iqomah") +
                           "\",\"col_iqomah\":\"" + httpServer.arg("col_iqomah") +
                           "\",\"col_shaf\":\"" + httpServer.arg("col_shaf") +
                           "\"}";
      // Serial.println(dataawarna);
      handleSimpanJson(dataawarna, 300, filewarna);
 
    }
  }); 
   
  // pengaturan
  // =========================
  httpServer.on("/atur", []() {
    if (httpServer.hasArg("adjhijr")) {
      String dataatur = "{\"adjhijr\":\"" + httpServer.arg("adjhijr") +
                        "\",\"adjcerah\":\"" + httpServer.arg("adjcerah") +
                        "\",\"adjvolumeadzan\":\"" + httpServer.arg("adjvolumeadzan") +
                        "\",\"adjvolume\":\"" + httpServer.arg("adjvolume") +
                        "\",\"beepstatus\":\"" + httpServer.arg("beepstatus") +
                        "\",\"modetampil\":\"" + httpServer.arg("modetampil") +
                        "\",\"relayon\":\"" + httpServer.arg("relayon") +
                        "\",\"relayoff\":\"" + httpServer.arg("relayoff") +
                        "\",\"speedtext\":\"" + httpServer.arg("speedtext") +
                        "\",\"msholbi\":\"" + httpServer.arg("msholbi") +
                        "\",\"msholju\":\"" + httpServer.arg("msholju") +
                        "\",\"jambesar\":\"" + httpServer.arg("jambesar") +
                        "\"}";
      // Serial.println(dataatur);
      handleSimpanJson(dataatur, 300, fileatur);
    }
  });
  
  // handle response
  // =========================
  httpServer.on("/kirimlokasi", handlekirimlokasi);
  httpServer.on("/kirimmasjidinfo", handlekirimMasjidinfo);
  httpServer.on("/kirimtpljdwl", handlekirimTampilJadwal);
  httpServer.on("/kirimkoreksi", handlekirimKoreksi);
  httpServer.on("/kirimtampilinfo", handlekirimTampilinfo);
  httpServer.on("/kirimadzan", handlekirimAdzan);
  httpServer.on("/kirimtartil", handlekirimTartil);
  httpServer.on("/kirimwarna", handlekirimWarna);
  httpServer.on("/kirimatur", handlekirimAtur);

  // Mulai Server
  // =========================

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  /* ===== BUZZER & RELAY ===== */
  pinMode(Buzzer, OUTPUT);  // 32 pcb
  
  /* ===== BEEP AWAL ===== */
  if (beepstatus == 1) {

    for (int i = 0; i < 4; i++) {
      if (i % 2 == 0) {
        digitalWrite(Buzzer, HIGH);
      } else {
        digitalWrite(Buzzer, LOW);
      }
      delay(250);
    }
  }
  /* ===== MP3 ===== */
  mp3_set_serial(mp3player);

  loadAtur();
}

void loop() {
  // mendeteksi request client
  httpServer.handleClient();

  // mode tampilan
  switch (mode) {
    case 0:
      if (tpl_nama == 1) {
        TPL_HH_TGL(3);
      } else {
        mode = 10;
      }
      break;
    case 1:
      TPL_HH(0);
      break;
    case 2:
      if (tpl_hijriyah == 1) {
        TPL_HH_TGL(1);
      } else {
        mode = 9;
      }
      break;
    case 3:
      TPL_HH(1);
      break;
    case 4:
      TPL_HH_TGL(5);
      break;
    case 5:
      TPL_HH_WKT_MSK();
      break;
    case 6:
      TPL_QD_IQM();
      break;
    case 7:
      if (tpl_matikan == 1) {
        TPL_HH_TGL(4);
      } else {
        mode = 13;
      }
      break;
    case 8:
      TPL_HH(0);
      break;
    case 9:
      if (tpl_masehi == 1) {
        TPL_HH_TGL(2);
      } else {
        mode = 8;
      }
      break;
    case 10:
      if (tpl_pesan == 1) {
        TPL_HH_TGL(6);
      } else {
        mode = 11;
      }
      break;
    case 11:
      if (tpl_pesan2 == 1) {
        TPL_HH_TGL(7);
      } else {
        mode = 12;
      }
      break;
    case 12:
      if (tpl_pesan3 == 1) {
        TPL_HH_TGL(8);
      } else {
        mode = 3;
      }
      break;
    case 13:
      KOSONG();
      break;
  }
}

// Tampilkan jam besar & jam arab 
void TPL_HH(int arab) {
  waktu();
  int jam, menit, detik;

  char jam_aja[3];
  char menit_aja[3];

  sprintf(jam_aja, "%02d", Jam);
  sprintf(menit_aja, "%02d", Menit);

  if ((millis() - p_e_1_jam) > 50) { // 50 x 250 = 12000 = 12 detik. 250 masukkan di pengaturan apk
    p_e_1_jam = millis();

    if (ash < jambesar) {  // if(ash < 240){
      ash++;
      Disp->flipDMABuffer();
      scan();
      matrix_init();

      if (arab == 0) {
        Disp->setFont(&BigNumber7pt7b);
      }
      else
      {
        Disp->setFont(&arabangka);
      }

      Disp->setTextSize(1);
      Disp->setTextColor(Disp->color565(col_jam_r, col_jam_g, col_jam_b));  // Warna udah dari SPIFFS

      if (ash < 17) {
        if (arab == 1) {
          drawText(1, ash , jam_aja);
          drawText(18, ash , menit_aja);
        } else {
          drawText(1, ash - 5, jam_aja);
          drawText(18, ash - 5, menit_aja);
        }

      } else if (ash > 16 && ash < (jambesar - 17)) {
        if (arab == 1) {
          drawText(1, 16, jam_aja);
          drawText(18, 16, menit_aja);
        } else {
          drawText(1, 11, jam_aja);
          drawText(18, 11, menit_aja);
        }

        if (Detik % 2 == 0) {  // Detik kedip
          Disp->fillRect(15, 4, 2, 2, Disp->color565(0, 0, 0));
          Disp->fillRect(15, 11, 2, 2, Disp->color565(0, 0, 0));
        } else {
          Disp->fillRect(15, 4, 2, 2, Disp->color565(col_jam_r, col_jam_g, col_jam_b));   // Warna udah dari SPIFFS
          Disp->fillRect(15, 11, 2, 2, Disp->color565(col_jam_r, col_jam_g, col_jam_b));  // Warna udah dari SPIFFS
        }
      } else if (ash > (jambesar - 18)) { //else if(ash>142)
        if (arab == 1) {
          drawText(1, (-1 * (-(jambesar - 7) + ash) + 5), jam_aja);
          drawText(18, (-1 * (-(jambesar - 7) + ash) + 5), menit_aja);
        } else {
          drawText(1, (-1 * (-(jambesar - 7) + ash)), jam_aja);
          drawText(18, (-1 * (-(jambesar - 7) + ash)), menit_aja);
        }
      }
      AMBIL_WAKTU_SHOLAT();
    }
    else
    {
      ash = 0; bsh = 0;
      if (mode == 1) {
        mode = 0;
      } else if (mode == 3) {
        mode = 2;
      } else if (mode == 8) {
        mode = 4;  // WAKTU SHOLAT
      }
    }
  }
}


void TPL_HH_TGL(int pilih) { // MENAMPILKAN NAMA MASJID, INFO, JADWAL SHOLAT DLL
  j_s_s = 0;
  waktu();
  AMBIL_WAKTU_SHOLAT();

  if (pilih == 1) { // ============= TANGGAL HIJRIYAH
    if (floatnow > floatmagrib) {
      pesan = Kuwaiti_algorithm(Tanggal, Bulan, Tahun, hariIni, adjhijr - 1);
    }
    else
    {
      pesan = Kuwaiti_algorithm(Tanggal, Bulan, Tahun, hariIni, adjhijr);
    }
  }
  else if (pilih == 2) { // ============= TANGGAL MASEHI
    // Pasaran
    char PasaranName[8];
    memset(PasaranName, 0, 8);
    strcpy_P(PasaranName, pasar[jumlahhari(Tanggal, Bulan, Tahun) % 5]);

    // Hari Masehi
    char DayName[7];
    memset(DayName, 0, 7);
    strcpy_P(DayName, namaHariMasehi[hariIni]);

    // Bulan Masehi
    char BulanName[12];
    memset(BulanName, 0, 12);
    strcpy_P(BulanName, namaBulanMasehi[Bulan - 1]);

    pesan =  String(DayName) + " " + String(PasaranName) + ", " + String(Tanggal) + " " + String(BulanName) + " " + String(Tahun);
  }
  else if (pilih == 3) { // ============= NAMA MASJID
    pesan = namamasjid;
  }
  else if (pilih == 4) { // ============= LURUSKAN
    pesan = stlhiqomah;
  }
  else if (pilih == 5) { // ============= JWS
    pesan = "";

    // ===== waktu imsak ===== //
    if (tpl_imsak == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[0]);

      pesan = pesan + nama_waktu + waktu_ims;
    }
    else
    {
      pesan = pesan;
    }

    // ===== waktu subuh ===== //
    if (tpl_subuh == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[1]);

      pesan = pesan + nama_waktu + waktu_sbh;
    }
    else
    {
      pesan = pesan;
    }

    // ===== waktu terbit ===== //
    if (tpl_terbit == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[2]);

      pesan = pesan + nama_waktu + waktu_tbt;
    }
    else
    {
      pesan = pesan;
    }

    // ===== waktu dhuha ===== //
    if (tpl_dhuha == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[3]);
      pesan = pesan + nama_waktu + waktu_dhu;
    }
    else
    {
      pesan = pesan;
    }

    // ===== waktu dzuhur ===== //
    if (tpl_dzuhur == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);

      if (hariIni == 5) {
        strcpy_P(nama_waktu, namaWaktuSholat[8]);
        pesan = pesan + nama_waktu + waktu_dzr;
      }
      else
      {
        strcpy_P(nama_waktu, namaWaktuSholat[4]);
        pesan = pesan + nama_waktu + waktu_dzr;
      }
    }
    else
    {
      pesan = pesan;
    }

    // ===== waktu ashar ===== //
    if (tpl_ashar == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[5]);
      pesan = pesan + nama_waktu + waktu_asr;
    } else {
      pesan = pesan;
    }

    // ===== waktu maghrib ===== //
    if (tpl_maghrib == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[6]);
      pesan = pesan + nama_waktu + waktu_mgr;
    } else {
      pesan = pesan;
    }

    // ===== waktu isya ===== //
    if (tpl_isya == 1) {
      char nama_waktu[8];
      memset(nama_waktu, 0, 8);
      strcpy_P(nama_waktu, namaWaktuSholat[7]);
      pesan = pesan + nama_waktu + waktu_isy;
    } else {
      pesan = pesan;
    }


  }
  else if (pilih == 6) { // ============= INFO 1
    pesan = info1;
  }
  else if (pilih == 7) { // ============= INFO 2
    pesan = info2;
  }
  else if (pilih == 8) { // ============= INFO 3
    pesan = info3;
  }

  if ((millis() - p_e_1_jam) > speedtext) {
    p_e_1_jam = millis();

    Disp->flipDMABuffer();
    scan();
    matrix_init();

    textMin = -(textWidth(pesan));


    // TAMPILAN JAM DARI ATAS KE BAWAH
    //======================

    if (bsh > (-32 + textMin)) {  // Animasi Nama Sholat dari atas ke bawah ke atas

      Disp->setFont(&fonttebal);
      Disp->setTextSize(1);
      textJam = (textWidth(tx_hh_mm));
      Disp->setTextColor(Disp->color565(col_waktu_r, col_waktu_g, col_waktu_b));

      if (bsh > -8) {
        Disp->setCursor((32 - textJam) / 2, (-9 + (-1 * bsh)) + 9);
        Disp->print(tx_hh_mm);
      } else if ((bsh < -7) && (bsh > ((-32 + textMin) + 7))) {
        Disp->setCursor((32 - textJam) / 2, 8);
        Disp->print(tx_hh_mm);
      } else if (bsh < (-32 + textMin) + 8) {
        Disp->setCursor((32 - textJam) / 2, (-1 * (-32 + textMin)) + bsh + 1);
        Disp->print(tx_hh_mm);
      }

      bsh--;
    }
    else {
      ash = 0; bsh = 0;
      if (pilih == 1) {
        mode = 9;  // TANGGAL HIJRIYAH >> TANGGAL MASEHI
      } else if (pilih == 2) {
        mode = 8;  // TANGGAL MASEHI >> JAM BESAR
      } else if (pilih == 3) {
        mode = 10;  // NAMA MASJID >> INFO
      } else if (pilih == 4) {
        mode = 13;  // SETELAH IQOMAH >> KOSONG
      } else if (pilih == 5) {
        mode = 1;  // JWS >> JAM BESAR
      } else if (pilih == 6) {
        mode = 11;  // INFO 1 >> INFO 2
      } else if (pilih == 7) {
        mode = 12;  // INFO 2 >> INFO 3
      } else if (pilih == 8) {
        mode = 3;  // INFO 2 >> Jam Besar
      }
    }

    // TAMPILAN INFO
    //======================

    Disp->setFont(&Font5x78pt7b);
    Disp->setTextSize(1);
    if (pilih == 4) { // stlh iqomah beda warna
      Disp->setTextColor(Disp->color565(col_shaf_r, col_shaf_g, col_shaf_b));
    }
    else
    {
      Disp->setTextColor(Disp->color565(col_info_r, col_info_g, col_info_b));
    }
    Disp->setCursor(textX, 15);
    Disp->print(pesan);

    if ((--textX) < textMin) {
      textX = Disp->width();
      // tpl_info++;
    }
  }
}
void TPL_HH_WKT_MSK() {  // MENAMPILKAN WAKTU MASUK ADZAN
  waktu();
  int b_lama;

  if (wkt_msk_skrg == 0) {  // Imsak
    b_lama = lamaimsak; // kalo lama imsak disetting 0 maka +1
  } else {
    b_lama = lamaadzan;
  }

  if ((millis() - p_e_1_jam) > 1000) {
    p_e_1_jam = millis();

    Disp->flipDMABuffer();
    scan();
    matrix_init();

    if (Detik % 2 == 0) {
      Disp->setFont(&fonttebal);
      textJam = textWidth(tx_hh_mm);
      Disp->setTextSize(1);
      Disp->setTextColor(Disp->color565(col_jam_adzan_r, col_jam_adzan_g, col_jam_adzan_b));
      Disp->setCursor((32 - textJam) / 2, 8);
      Disp->print(tx_hh_mm);

      //textAdzan = textWidth(slt_sek);

      Disp->setFont(&Font4x78pt7b);
      Disp->setTextSize(1);
      textAdzan = textWidth(slt_sek);
      Disp->setTextColor(Disp->color565(col_adzan_r, col_adzan_g, col_adzan_b));
      Disp->setCursor(((32 - textAdzan) / 2) + 1, 20);
      Disp->print(slt_sek);

    }
    else
    {
      Disp->setFont(&fonttebal);
      textJam = textWidth(tx_hh_mm);
      Disp->setTextSize(1);
      Disp->setTextColor(Disp->color565(col_jam_adzan_r, col_jam_adzan_g, col_jam_adzan_b));
      Disp->setCursor((32 - textJam) / 2, 8);
      Disp->print(tx_hh_mm);

      //textAdzan = textWidth(slt_sek);

      Disp->setFont(&Font4x78pt7b);
      Disp->setTextSize(1);
      textAdzan = textWidth(slt_sek);
      Disp->setTextColor(Disp->color565(0, 0, 0));
      Disp->setCursor(((32 - textAdzan) / 2) + 1, 20);
      Disp->print(slt_sek);
    }

    if (durasi_tampil_adzan <= (60 * b_lama) - 2) {  // Masih belum nemu kenapa lebih 2 detik
      if (durasi_tampil_adzan < 10) {                // Beep 10 detik sebelum adzan
        if (wkt_msk_skrg == 0) {
          if (beepstatus == 1) {
            beep(500);
          }
        }
        if (wkt_msk_skrg == 1) {
          if (beepstatus == 1) {
            beep(1000);
          }
        }
        if (wkt_msk_skrg == 4 || wkt_msk_skrg == 5 || wkt_msk_skrg == 6 || wkt_msk_skrg == 7) {
          if (beepstatus == 1) {
            beep(1000);
          }
        }
      }

      else if (durasi_tampil_adzan == 11) {

        flag_tartil = 1;  // Untuk mencegah jam bunyi

        if (wkt_msk_skrg == 1) {
          mp3_set_volume(adjvolumeadzan);
          mp3_play(2);
        }
        if (wkt_msk_skrg == 4 || wkt_msk_skrg == 5 || wkt_msk_skrg == 6 || wkt_msk_skrg == 7) {
          mp3_set_volume(adjvolumeadzan);
          mp3_play(1);
        }
      }
      durasi_tampil_adzan++;
    } else {
      ash = 0; bsh = 0;
      // Jika (IMSAK) abis masuk waktu ga iqomah
      if ( wkt_msk_skrg == 0 ) {
        j_s_s = 0;
        durasi_tampil_adzan = 0;
        wkt_msk_skrg = 0;
        textX = 32;
        mode = 1;
      }
      //hari JUMAT abis adzan ga iqomah masuk kosong
      else if ( wkt_msk_skrg == 8 ) {
        j_s_s = 0;
        durasi_tampil_adzan = 0;
        wkt_msk_skrg = 0;
        textX = 32;
        mode = 13;
      }
      else {
        j_s_s = 1;
        durasi_tampil_adzan = 0;
        mode = 6;  // Tampilkan Iqomah
      }
    }
  }
}
void TPL_QD_IQM() {  // MENAMPILKAN IQOMAH

  j_s_s = 0;

  flag_mp3 = 0; // MP3 enable
  //  wkt_msk_skrg = 1; // test aja

  if (wkt_msk_skrg == 1 /*Subuh*/) {
    iqm_ee = (iqomahsubuh * 60);
  }
  if (wkt_msk_skrg == 4 /*Dzuhur*/) {
    iqm_ee = (iqomahdzuhur * 60);
  }
  if (wkt_msk_skrg == 5 /*Ashar*/) {
    iqm_ee = (iqomahashar * 60);
  }
  if (wkt_msk_skrg == 6 /*Magrib*/) {
    iqm_ee = (iqomahmaghrib * 60);
  }
  if (wkt_msk_skrg == 7 /*Isya*/) {
    iqm_ee = (iqomahisya * 60);
  }

  if ((millis() - p_e_1_jam) > 1000) {
    p_e_1_jam = millis();

    Disp->flipDMABuffer();
    scan();
    matrix_init();

    if (du_iq <= iqm_ee) {

      // Beep 10 detik terakhir
      if (beepstatus == 1) {

        if (du_iq > (iqm_ee - 10) && du_iq < iqm_ee) {
          beep(1000);
        }
        // Buzzer pengingat akhir iqomah detik terakhir
        if (du_iq == iqm_ee) {
          digitalWrite(Buzzer, HIGH);
          delay(2000);
          digitalWrite(Buzzer, LOW);
        }
      }

      iqm_detik = (iqm_ee - du_iq) % 60;
      iqm_menit = (((iqm_ee - du_iq) - iqm_detik) / 60) % 60;

      wkt_iqomah = Konversi(iqm_menit) + ":" + Konversi(iqm_detik);

      // Tulisan Iqomah

      Disp->setFont(&Font4x78pt7b);
      Disp->setTextSize(1);
      textIqo = textWidth("IQOMAH");
      Disp->setTextColor(Disp->color565(col_iqomah_r, col_iqomah_g, col_iqomah_b));
      Disp->setCursor((32 - textIqo) / 2, 11);
      Disp->print("IQOMAH");


      // Countdown Iqomah

      Disp->setFont(&fonttebal);
      Disp->setTextSize(1);
      textCdiqo = textWidth(wkt_iqomah);
      Disp->setTextColor(Disp->color565(col_jam_iqomah_r, col_jam_iqomah_g, col_jam_iqomah_b));
      Disp->setCursor((32 - textCdiqo) / 2, 16);
      Disp->print(wkt_iqomah);

      du_iq++;

    } else {
      j_s_s = 0;
      ash = 0; bsh = 0;
      wkt_msk_skrg = 0;
      textX = 32; // lebar panel
      du_iq = 0;
      mode = 7;
    }
  }
}
void KOSONG() { // KOSONGKAN TAMPILAN SAAT SEDANG SHOLAT
  waktu();
  AMBIL_WAKTU_SHOLAT();
  DateTime dt_plus;
  
  if (flag_dt_plus == 0) {
    if (hariIni == 5 && wkt_msk_skrg == 4) {
      DateTime dt_now(Tahun, Bulan, Tanggal, Jam, Menit, Detik);
      dt_plus = dt_now + TimeSpan(0, 0, msholju, 0);  // diatur di msholju
    } else {
      DateTime dt_now(Tahun, Bulan, Tanggal, Jam, Menit, Detik);
      dt_plus = dt_now + TimeSpan(0, 0, msholbi, 0);  // diatur di msholbi
    }
    str_dt_plus = (dt_plus.hour() / 10) + String(dt_plus.hour() % 10) + ":" + String(dt_plus.minute() / 10) + String(dt_plus.minute() % 10);
    flag_dt_plus = 1;
  }

  if ((millis() - p_e_1_jam) > 60) {
    p_e_1_jam = millis();

    Disp->flipDMABuffer();
    scan();
    matrix_init();
    if (cck == str_dt_plus) {
      wkt_msk_skrg = 0;
      textX = 32;
      flag_dt_plus = 0;
      mode = 1;
    }
  }
}

/* ===================== */
/* KIRIM DATA KE APK */
/* ===================== */
void handlekirimMasjidinfo() {
  String kirim_masjidinfo = "{\"namamasjid\":\"" + String(namamasjid) + "\",\"info1\":\"" + String(info1) +
                            "\",\"info2\":\"" + String(info2) + "\",\"info3\":\"" + String(info3) +
                            "\",\"stlhiqomah\":\"" + String(stlhiqomah) + "\"}";

  httpServer.send(200, "text/plain", kirim_masjidinfo);
}
void handlekirimlokasi() {
  String kirim_lokasi = "{\"latitude\":\"" + String(latitude) +
                        "\",\"longitude\":\"" + String(longitude) +
                        "\",\"zonawaktu\":\"" + String(zonawaktu) +
                        "\"}";

  httpServer.send(200, "text/plain", kirim_lokasi);
}
void handlekirimTampilJadwal() {
  String kirim_tampil_jadwal = "{\"tpl_imsak\":\"" + String(tpl_imsak) +
                               "\",\"tpl_subuh\":\"" + String(tpl_subuh) +
                               "\",\"tpl_terbit\":\"" + String(tpl_terbit) +
                               "\",\"tpl_dhuha\":\"" + String(tpl_dhuha) +
                               "\",\"tpl_dzuhur\":\"" + String(tpl_dzuhur) +
                               "\",\"tpl_ashar\":\"" + String(tpl_ashar) +
                               "\",\"tpl_maghrib\":\"" + String(tpl_maghrib) +
                               "\",\"tpl_isya\":\"" + String(tpl_isya) +
                               "\"}";

  httpServer.send(200, "text/plain", kirim_tampil_jadwal);
}
void handlekirimKoreksi() {
  String kirim_koreksi = "{\"korimsak\":\"" + String(b_a[0]) +
                         "\",\"korsubuh\":\"" + String(b_a[1]) +
                         "\",\"korterbit\":\"" + String(b_a[2]) +
                         "\",\"kordhuha\":\"" + String(b_a[3]) +
                         "\",\"kordzuhur\":\"" + String(b_a[4]) +
                         "\",\"korashar\":\"" + String(b_a[5]) +
                         "\",\"kormaghrib\":\"" + String(b_a[6]) +
                         "\",\"korisya\":\"" + String(b_a[7]) +
                         "\"}";

  httpServer.send(200, "text/plain", kirim_koreksi);
}
void handlekirimAdzan() {
  String kirim_adzan = "{\"lamaadzan\":\"" + String(lamaadzan) +
                       "\",\"iqomahsubuh\":\"" + String(iqomahsubuh) +
                       "\",\"iqomahdzuhur\":\"" + String(iqomahdzuhur) +
                       "\",\"iqomahashar\":\"" + String(iqomahashar) +
                       "\",\"iqomahmaghrib\":\"" + String(iqomahmaghrib) +
                       "\",\"iqomahisya\":\"" + String(iqomahisya) +
                       "\"}";

  httpServer.send(200, "text/plain", kirim_adzan);
}
void handlekirimTampilinfo() {
  String kirim_tampilinfo = "{\"tpl_nama\":\"" + String(tpl_nama) +
                            "\",\"tpl_pesan\":\"" + String(tpl_pesan) +
                            "\",\"tpl_pesan2\":\"" + String(tpl_pesan2) +
                            "\",\"tpl_pesan3\":\"" + String(tpl_pesan3) +
                            "\",\"tpl_hijriyah\":\"" + String(tpl_hijriyah) +
                            "\",\"tpl_masehi\":\"" + String(tpl_masehi) +
                            "\",\"tpl_matikan\":\"" + String(tpl_matikan) +
                            "\"}";

  httpServer.send(200, "text/plain", kirim_tampilinfo);
}
void handlekirimTartil() {
  String kirim_tartil = "{\"imsak\":\"" + String(imsak) +
                        "\",\"subuh\":\"" + String(subuh) +
                        "\",\"dzuhur\":\"" + String(dzuhur) +
                        "\",\"ashar\":\"" + String(ashar) +
                        "\",\"maghrib\":\"" + String(maghrib) +
                        "\",\"isya\":\"" + String(isya) +
                        "\",\"jumat\":\"" + String(jumat) +
                        "\"}";

  httpServer.send(200, "text/plain", kirim_tartil);
}
void handlekirimWarna() {
  String kirim_warna = "{\"col_jam\":\"" + String(col_jam) +
                       "\",\"col_waktu\":\"" + String(col_waktu) +
                       "\",\"col_info\":\"" + String(col_info) +
                       "\",\"col_jam_adzan\":\"" + String(col_jam_adzan) +
                       "\",\"col_adzan\":\"" + String(col_adzan) +
                       "\",\"col_jam_iqomah\":\"" + String(col_jam_iqomah) +
                       "\",\"col_iqomah\":\"" + String(col_iqomah) +
                       "\",\"col_shaf\":\"" + String(col_shaf) +
                       "\"}";

  httpServer.send(200, "text/plain", kirim_warna);
}
void handlekirimAtur() {
  String kirim_atur = "{\"adjhijr\":\"" + String(adjhijr) +
                        "\",\"adjcerah\":\"" + String(adjcerah) +
                        "\",\"adjvolumeadzan\":\"" + String(adjvolumeadzan) +
                        "\",\"adjvolume\":\"" + String(adjvolume) +
                        "\",\"beepstatus\":\"" + String(beepstatus) +
                        "\",\"modetampil\":\"" + String(modetampil) +
                        "\",\"relayon\":\"" + String(relayon) +
                        "\",\"relayoff\":\"" + String(relayoff) +
                        "\",\"speedtext\":\"" + String(speedtext) +
                        "\",\"msholbi\":\"" + String(msholbi) +
                        "\",\"msholju\":\"" + String(msholju) +
                        "\",\"jambesar\":\"" + String(jambesar) +
                        "\"}";

  httpServer.send(200, "text/plain", kirim_atur);
}

/* ===================== */
/* SIMPAN UPDATE DATA */
/* ===================== */
void handleSimpanJson(String isidata, int ukuran, const char *namafile) {
  String datain = isidata;
  DynamicJsonDocument doc(ukuran);
  DeserializationError error = deserializeJson(doc, datain);
  File configFileJson = SPIFFS.open(namafile, "w");
  if (!configFileJson) {
    return;
  }
  serializeJson(doc, configFileJson);
  if (error) {
    return;
  } else {
    configFileJson.close();

    loadAturConfig(fileatur);
    loadWarna(filewarna);
    loadLokasiConfig(filelokasi);
    loadKoreksiConfig(filekoreksi);
    loadMasjidinfoConfig(filemasjidinfo);
    loadTampilinfoConfig(filetampilinfo);
    loadIqomahConfig(fileiqomah);
    loadwaktuTartilConfig(filewaktutartil);
    loadTampiljwsConfig(filetampiljws);
    loadUserConfig(fileuser);
    loadAtur();

    if (beepstatus == 1) { beep(100); }
  }
}

// Fungsi-fungsi lainnya
void waktu() {
  DateTime now = rtc.now();
  Tahun = now.year();
  Bulan = now.month();
  Tanggal = now.day();
  Jam = now.hour();
  Menit = now.minute();
  Detik = now.second();
  hariIni = now.dayOfTheWeek();

  // Untuk mencocokkan waktu sholat dan jam sekarang
  txhh = Konversi(now.hour());
  txmm = Konversi(now.minute());
  txss = Konversi(now.second());

  if (Detik % 2 == 0) {
    tx_hh_mm = txhh + " " + txmm;
  } else {
    tx_hh_mm = txhh + ":" + txmm;
  }

  cck = txhh + ":" + txmm;                // untuk jadwal sholat
  cck1 = txhh + ":" + txmm + ":" + txss;  // untuk mulai tartil

  floatnow = (float)now.hour() + (float)now.minute() / 60 + (float)now.second() / 3600;  // untuk pergantian tanggal di maghrib
}
String Konversi(int sInput) {
  if (sInput < 10) {
    return "0" + String(sInput);
  } else {
    return String(sInput);
  }
}
void drawText(uint16_t x, uint16_t y, const char* msg) {
  Disp->setCursor(x, y);
  Disp->print(msg);
}
void matrix_init() {
  Disp->setBrightness8(adjcerah);
  Disp->clearScreen();
}
void scan() {
  DateTime now = rtc.now();
  static uint16_t lsRn;
  uint16_t Tmr = millis();
  if ((int)now.minute() == 0) {
    Disp->fillScreen(0);
  }
}
void loadAtur() {
  mp3_set_volume (5);
  Disp->setBrightness8(adjcerah);
}

// ====== panjang text ====== //
char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}
uint16_t textWidth(String text) {
  int16_t x1, y1;
  uint16_t w = Disp->width(), h = Disp->height();
  Disp->getTextBounds(string2char(text), 0, 0, &x1, &y1, &w, &h);
  return w;
}
// ====== panjang text ====== //

void AMBIL_WAKTU_SHOLAT() {           // AMBIL WAKTU SHOLAT

  DateTime now = rtc.now();
  int sekarang = now.dayOfTheWeek();

  SholahTimeCalulation sholat;
  sholat.coordinatSet(latitude, longitude, zonawaktu);
  sholat.dateSet(now.year(), now.month(), now.day());
  sholat.ihtiyatiSet(0);
  sholat.calculate();

  int hh_sol, mm_sol;
  int mm_ims, hh_ims;
  int hh_sol_res, mm_sol_res;

  for (int ws = 0; ws < SholatCount; ws++) {

    sholat.getTimePart(ws, hh_sol, mm_sol);

    if (ws == 3) { // Dhuha
      // mm_sol = mm_sol + b_a[ws]-2;
      mm_sol = mm_sol + b_a[ws]; // Normal Dulu ya...
    }

    else if (ws == 2) { // Terbit
      // mm_sol = mm_sol + b_a[ws]-2;
      mm_sol = mm_sol + b_a[ws]; // Normal Dulu ya...
    }
    else
    {
      mm_sol = mm_sol + b_a[ws];
    }

    if (mm_sol >= 60) {
      mm_sol = mm_sol - 60;
      hh_sol ++;
    }

    String wkt_sol_sat =  String(hh_sol / 10) + String(hh_sol % 10) + ":" + String(mm_sol / 10) + String(mm_sol % 10);

    if (ws == 0) {
      waktu_ims = wkt_sol_sat;
      CEKMASUKWAKTU(ws, waktu_ims);
      CEKTARTIL(ws, waktu_ims, 0);
    }
    if (ws == 1) {
      waktu_sbh = wkt_sol_sat;
      CEKMASUKWAKTU(ws, waktu_sbh);
      CEKTARTIL(ws, waktu_sbh, 0);
    }
    if (ws == 2) {
      waktu_tbt = wkt_sol_sat;
    }
    if (ws == 3) {
      waktu_dhu = wkt_sol_sat;
    }
    if (ws == 4) {
      waktu_dzr = wkt_sol_sat;
      if (sekarang == 5) {
        CEKMASUKWAKTU(ws, waktu_dzr);
        CEKTARTIL(ws, waktu_dzr, 1);
      } else {
        CEKMASUKWAKTU(ws, waktu_dzr);
        CEKTARTIL(ws, waktu_dzr, 0);
      }
    }
    if (ws == 5) {
      waktu_asr = wkt_sol_sat;
      CEKMASUKWAKTU(ws, waktu_asr);
      CEKTARTIL(ws, waktu_asr, 0);
    }
    if (ws == 6) {
      waktu_mgr = wkt_sol_sat;
      CEKMASUKWAKTU(ws, waktu_mgr);
      CEKTARTIL(ws, waktu_mgr, 0);
      floatmagrib = (float)hh_sol + (float)mm_sol / 60 + (float)0 / 3600;  // untuk kalender hijriyah perbarui setiap maghrib
    }
    if (ws == 7) {
      waktu_isy = wkt_sol_sat;
      CEKMASUKWAKTU(ws, waktu_isy);
      CEKTARTIL(ws, waktu_isy, 0);
    }
  }


  // Relay //
  //  if (flag_relay == 0) {
  //    if (cck1 == relayon) {
  //      digitalWrite(Relay, HIGH);
  //      wkt_msk_skrg = 0; // buat cek di bunyi jam
  //    }
  //    if (cck1 == relayoff) {
  //      digitalWrite(Relay, LOW);
  //      wkt_msk_skrg = 10;
  //    }
  //    flag_relay = 1;
  //  } else {
  //    flag_relay = 0;
  //  }

  if (Menit == 0 && Detik == 0 && wkt_msk_skrg == 0) {
    if (flag_mp3 == 0) { // jika enable
      mp3_set_volume(adjvolume);
      if (Jam == 1 || Jam == 13)      {
        mp3_play(5);
      }
      else if (Jam == 2 || Jam == 14) {
        mp3_play(6);
      }
      else if (Jam == 3 || Jam == 15) {
        mp3_play(7);
      }
      else if (Jam == 4 || Jam == 16) {
        mp3_play(8);
      }
      else if (Jam == 5 || Jam == 17) {
        mp3_play(9);
      }
      else if (Jam == 6 || Jam == 18) {
        mp3_play(10);
      }
      else if (Jam == 7 || Jam == 19) {
        mp3_play(11);
      }
      else if (Jam == 8 || Jam == 20) {
        mp3_play(12);
      }
      else if (Jam == 9 || Jam == 21) {
        mp3_play(13);
      }
      else if (Jam == 10 || Jam == 22) {
        mp3_play(14);
      }
      else if (Jam == 11 || Jam == 23) {
        mp3_play(15);
      }
      else if (Jam == 12 || Jam == 0) {
        mp3_play(16);
      }
      flag_mp3 = 1;
    }
  }

  if (Menit == 30 && Detik == 0 && wkt_msk_skrg == 0) {
    if (flag_mp3 == 0) {
      mp3_set_volume (adjvolume);
      mp3_play(18);
      flag_mp3 = 1;
    }
  }

  if (Detik == 3 && flag_tartil == 1) {
    flag_mp3 = 0;
  }

  //  Serial.print("flag_mp3 :"); Serial.println(flag_mp3);
  //  Serial.print("flag_tartil :"); Serial.println(flag_tartil);
  //  Serial.println("=============");
  //  Serial.println();


}
void CEKMASUKWAKTU(int waktu, String jam_masuk) {
  DateTime now = rtc.now();
  if (cck == jam_masuk && j_s_s == 0) {

    if (waktu == 4 && now.dayOfTheWeek() == 5) {
      wkt_msk_skrg = 8; // JUMAT
      slt_sek = "JUM'AT";
    } else if (waktu == 4 && now.dayOfTheWeek() != 5) {  // dzuhur biasa
      wkt_msk_skrg = waktu;
      slt_sek = "DZUHUR";
    } else {
      wkt_msk_skrg = waktu;
    }

    if ( waktu == 0 ) {
      slt_sek = "IMSAK";
    }
    if ( waktu == 1 ) {
      slt_sek = "SUBUH";
    }
    if ( waktu == 5 ) {
      slt_sek = "ASHAR";
    }
    if ( waktu == 6 ) {
      slt_sek = "MAGRIB";
    }
    if ( waktu == 7 ) {
      slt_sek = "ISYA";
    }

    ash = 0; bsh = 0;
    mode = 5;
  }

}
void CEKTARTIL(int waktu, String jam_masuk, int h_jumat) {
  DateTime now = rtc.now();
  if (waktu == 0) {  //imsak
    b_tar_all = imsak;
  }
  if (waktu == 1) {  //subuh
    b_tar_all = subuh;
  }
  if (waktu == 4) {
    if (h_jumat == 0) {  //dzuhur
      b_tar_all = dzuhur;
    } else {  //jumat
      b_tar_all = jumat;
    }
  }
  if (waktu == 5) {  //ashar
    b_tar_all = ashar;
  }
  if (waktu == 6) {  //maghrib
    b_tar_all = maghrib;
  }
  if (waktu == 7) {  //subuh
    b_tar_all = isya;
  }

  int jam_now = jam_masuk.substring(0, 2).toInt();
  int menit_now = jam_masuk.substring(3, 5).toInt();
  tar_now = b_tar_all.substring(0, 2).toInt();

  DateTime dt_now(now.year(), now.month(), now.day(), jam_now, menit_now, 0);
  DateTime dt_tar_wkt = dt_now - TimeSpan(0, 0, String(b_tar_all).substring(3, 5).toInt(), String(b_tar_all).substring(6, 8).toInt());
  wkt_tartil_all = String(dt_tar_wkt.hour() / 10) + String(dt_tar_wkt.hour() % 10) + ":" + String(dt_tar_wkt.minute() / 10) + String(dt_tar_wkt.minute() % 10) + ":" + String(dt_tar_wkt.second() / 10) + String(dt_tar_wkt.second() % 10);

  if (cck1 == wkt_tartil_all && wkt_tartil_all != "00:00") {
    flag_mp3 = 1; // set MP3 di disable
    flag_tartil = 0; // set Tartil di enable
    if (flag_mp3 == 1) {
      mp3_set_volume (adjvolumeadzan);
      mp3_play(String(tar_now).substring(0, 2).toInt());
    }
    // flag_mp3 = 1;
  }
}
void beep(unsigned char delayms) {
  digitalWrite(Buzzer, HIGH);
  delay(delayms);
  digitalWrite(Buzzer, LOW);
  delay(delayms);
}
int jumlahhari(int tgl_pas, int bln_pas, int thn_pas) {

  int maxday[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  int hb[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
  int ht = (thn_pas - 1970) * 365 - 1;
  int hs = hb[bln_pas - 1] + tgl_pas;
  int kab = 0;
  int i;
  if (thn_pas % 4 == 0) {
    if (bln_pas > 2) {
      hs++;
    }
  }
  for (i = 1970; i < thn_pas; i++) {
    if (i % 4 == 0) {
      kab++;
    }
  }
  return (ht + hs + kab);
}
