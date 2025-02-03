const char *fileatur = "/atur.json";
int adjhijr, adjcerah, adjvolume, adjvolumeadzan, beepstatus, modetampil, speedtext, msholbi, msholju, jambesar;
char relayon[10], relayoff[10];

const char *filewarna = "/warna.json";
char col_jam[15], col_nama[15];
char col_waktu[15], col_info[15];
char col_jam_adzan[15], col_jam_iqomah[15];
char col_adzan[15], col_iqomah[15];
char col_shaf[15];

const char *filelokasi = "/lokasi.json";
float latitude, longitude; int zonawaktu;

const char *filekoreksi = "/koreksi.json";
int b_a[8];

const char *filemasjidinfo = "/masjidinfo.json";
char namamasjid[100], info1[100], info2[100], info3[100], stlhiqomah[100];

const char *filetampilinfo = "/tampilinfo.json";
int tpl_nama, tpl_pesan, tpl_pesan2, tpl_pesan3, tpl_hijriyah, tpl_masehi, tpl_matikan;

const char *filetampiljws = "/tampiljws.json";
int tpl_imsak,  tpl_subuh,  tpl_terbit,  tpl_dhuha,  tpl_dzuhur,  tpl_ashar,  tpl_maghrib,  tpl_isya;

const char *fileiqomah = "/iqomah.json";
int lamaadzan, lamaimsak, iqomahsubuh, iqomahdzuhur, iqomahashar, iqomahmaghrib, iqomahisya;

// Variable untuk pecah warna RGB
int col_jam_r, col_jam_g, col_jam_b;
int col_nama_r, col_nama_g, col_nama_b;
int col_waktu_r, col_waktu_g, col_waktu_b;
int col_info_r, col_info_g, col_info_b;
int col_jam_adzan_r, col_jam_adzan_g, col_jam_adzan_b;
int col_jam_iqomah_r, col_jam_iqomah_g, col_jam_iqomah_b;
int col_adzan_r, col_adzan_g, col_adzan_b;
int col_iqomah_r, col_iqomah_g, col_iqomah_b;
int col_shaf_r, col_shaf_g, col_shaf_b;

const char *filewaktutartil = "/waktutartil.json";
char imsak[10], subuh[10], dzuhur[10], ashar[10], maghrib[10], isya[10], jumat[10];

const char *fileuser = "/user.json";
char pengguna[10], kunci[10];

// LOAD PENGATURAN DASAR //
void loadAturConfig(const char *fileatur) {
  File configFileatur = SPIFFS.open(fileatur, "r");

  if (!configFileatur) {
    ESP.restart();
  }

  size_t size = configFileatur.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileatur.readBytes(buf.get(), size);

  DynamicJsonDocument doc(384);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  adjhijr = doc["adjhijr"];
  adjcerah = doc["adjcerah"];
  adjvolumeadzan = doc["adjvolumeadzan"];
  adjvolume = doc["adjvolume"];
  beepstatus = doc["beepstatus"];
  modetampil = doc["modetampil"];
  speedtext = doc["speedtext"];
  msholbi = doc["msholbi"];
  msholju = doc["msholju"];
  jambesar = doc["jambesar"];
  strlcpy(relayon, doc["relayon"] | "", sizeof(relayon));
  strlcpy(relayoff, doc["relayoff"] | "", sizeof(relayoff));

  configFileatur.close();
}

// LOAD PENGATURAN WARNA //
void loadWarna(const char *filewarna) {
  File configFileWarna = SPIFFS.open(filewarna, "r");

  if (!configFileWarna) {
    ESP.restart();
  }

  size_t size = configFileWarna.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileWarna.readBytes(buf.get(), size);

  DynamicJsonDocument doc(500); //384
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  strlcpy(col_jam, doc["col_jam"] | "", sizeof(col_jam));
  strlcpy(col_nama, doc["col_nama"] | "", sizeof(col_nama));
  strlcpy(col_waktu, doc["col_waktu"] | "", sizeof(col_waktu));
  strlcpy(col_info, doc["col_info"] | "", sizeof(col_info));
  strlcpy(col_jam_adzan, doc["col_jam_adzan"] | "", sizeof(col_jam_adzan));
  strlcpy(col_jam_iqomah, doc["col_jam_iqomah"] | "", sizeof(col_jam_iqomah));
  strlcpy(col_adzan, doc["col_adzan"] | "", sizeof(col_adzan));
  strlcpy(col_iqomah, doc["col_iqomah"] | "", sizeof(col_iqomah));
  strlcpy(col_shaf, doc["col_shaf"] | "", sizeof(col_shaf));

  String hex_col_jam = String(col_jam);
  int number_col_jam = (int) strtol( &hex_col_jam[1], NULL, 16); // Buang '#' dan convert ke integer
  col_jam_r = number_col_jam >> 16;
  col_jam_g = number_col_jam >> 8 & 0xFF;
  col_jam_b = number_col_jam & 0xFF;

  String hex_col_nama = String(col_nama);
  int number_col_nama = (int) strtol( &hex_col_nama[1], NULL, 16);
  col_nama_r = number_col_nama >> 16;
  col_nama_g = number_col_nama >> 8 & 0xFF;
  col_nama_b = number_col_nama & 0xFF;

  String hex_col_waktu = String(col_waktu);
  int number_col_waktu = (int) strtol( &hex_col_waktu[1], NULL, 16);
  col_waktu_r = number_col_waktu >> 16;
  col_waktu_g = number_col_waktu >> 8 & 0xFF;
  col_waktu_b = number_col_waktu & 0xFF;

  String hex_col_info = String(col_info);
  int number_col_info = (int) strtol( &hex_col_info[1], NULL, 16);
  col_info_r = number_col_info >> 16;
  col_info_g = number_col_info >> 8 & 0xFF;
  col_info_b = number_col_info & 0xFF;

  String hex_col_jam_adzan = String(col_jam_adzan);
  int number_col_jam_adzan = (int) strtol( &hex_col_jam_adzan[1], NULL, 16);
  col_jam_adzan_r = number_col_jam_adzan >> 16;
  col_jam_adzan_g = number_col_jam_adzan >> 8 & 0xFF;
  col_jam_adzan_b = number_col_jam_adzan & 0xFF;

  String hex_col_jam_iqomah = String(col_jam_iqomah);
  int number_col_jam_iqomah = (int) strtol( &hex_col_jam_iqomah[1], NULL, 16);
  col_jam_iqomah_r = number_col_jam_iqomah >> 16;
  col_jam_iqomah_g = number_col_jam_iqomah >> 8 & 0xFF;
  col_jam_iqomah_b = number_col_jam_iqomah & 0xFF;

  String hex_col_adzan = String(col_adzan);
  int number_col_adzan = (int) strtol( &hex_col_adzan[1], NULL, 16);
  col_adzan_r = number_col_adzan >> 16;
  col_adzan_g = number_col_adzan >> 8 & 0xFF;
  col_adzan_b = number_col_adzan & 0xFF;

  String hex_col_iqomah = String(col_iqomah);
  int number_col_iqomah = (int) strtol( &hex_col_iqomah[1], NULL, 16);
  col_iqomah_r = number_col_iqomah >> 16;
  col_iqomah_g = number_col_iqomah >> 8 & 0xFF;
  col_iqomah_b = number_col_iqomah & 0xFF;

  String hex_col_shaf = String(col_shaf);
  int number_col_shaf = (int) strtol( &hex_col_shaf[1], NULL, 16);
  col_shaf_r = number_col_shaf >> 16;
  col_shaf_g = number_col_shaf >> 8 & 0xFF;
  col_shaf_b = number_col_shaf & 0xFF;

  configFileWarna.close();
}

// LOAD PENGATURAN LOKASI //
void loadLokasiConfig(const char *filelokasi) {
  File configFilelokasi = SPIFFS.open(filelokasi, "r");

  if (!configFilelokasi) {
    ESP.restart();
  }

  size_t size = configFilelokasi.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFilelokasi.readBytes(buf.get(), size);

  DynamicJsonDocument doc(150);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  latitude = doc["latitude"];
  longitude = doc["longitude"];
  zonawaktu = doc["zonawaktu"];

  configFilelokasi.close();
}

// LOAD PENGATURAN KOREKSI //
void loadKoreksiConfig(const char *filekoreksi) {
  File configKoreksi = SPIFFS.open(filekoreksi, "r");

  if (!configKoreksi) {
    // Serial.println("Gagal membuka pengaturan Koreksi Waktu");
    //makeKoreksi();
    // Serial.println("Sistem restart...");
    ESP.restart();
  }

  size_t size = configKoreksi.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configKoreksi.readBytes(buf.get(), size);

  // DynamicJsonDocument doc(1024);
  DynamicJsonDocument doc(300);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    // Serial.println("Gagal parse pengaturan Koreksi Waktu");
    return;
  }

  b_a[0] = doc["korimsak"];
  b_a[1] = doc["korsubuh"];
  b_a[2] = doc["korterbit"];
  b_a[3] = doc["kordhuha"];
  b_a[4] = doc["kordzuhur"];
  b_a[5] = doc["korashar"];
  b_a[6] = doc["kormaghrib"];
  b_a[7] = doc["korisya"];

  configKoreksi.close();
}

// LOAD PENGATURAN NAMA MASJID DAN INFO //
void loadMasjidinfoConfig(const char *filemasjidinfo) {
  File configMasjidinfo = SPIFFS.open(filemasjidinfo, "r");

  if (!configMasjidinfo) {
    ESP.restart();
  }

  size_t size = configMasjidinfo.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configMasjidinfo.readBytes(buf.get(), size);

  DynamicJsonDocument doc(800);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  strlcpy(namamasjid, doc["namamasjid"] | "", sizeof(namamasjid));
  strlcpy(info1, doc["info1"] | "", sizeof(info1));
  strlcpy(info2, doc["info2"] | "", sizeof(info2));
  strlcpy(info3, doc["info3"] | "", sizeof(info3));
  strlcpy(stlhiqomah, doc["stlhiqomah"] | "", sizeof(stlhiqomah));

  configMasjidinfo.close();
}

// LOAD USER //
void loadUserConfig(const char *fileuser) {
  File configUser = SPIFFS.open(fileuser, "r");

  if (!configUser) {
    ESP.restart();
  }

  size_t size = configUser.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configUser.readBytes(buf.get(), size);

  DynamicJsonDocument doc(100);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  strlcpy(pengguna, doc["pengguna"] | "", sizeof(pengguna));
  strlcpy(kunci, doc["kunci"] | "", sizeof(kunci));

  configUser.close();
}

// LOAD PENGATURAN TAMPILKAN JADWAL //
void loadTampiljwsConfig(const char *filetampiljws) {
  File configTampiljws = SPIFFS.open(filetampiljws, "r");

  if (!configTampiljws) {
    ESP.restart();
  }

  size_t size = configTampiljws.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configTampiljws.readBytes(buf.get(), size);


  DynamicJsonDocument doc(260);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  tpl_imsak = doc["tpl_imsak"];
  tpl_subuh = doc["tpl_subuh"];
  tpl_terbit = doc["tpl_terbit"];
  tpl_dhuha = doc["tpl_dhuha"];
  tpl_dzuhur = doc["tpl_dzuhur"];
  tpl_ashar = doc["tpl_ashar"];
  tpl_maghrib = doc["tpl_maghrib"];
  tpl_isya = doc["tpl_isya"];

  configTampiljws.close();
}

// LOAD PENGATURAN TAMPILKAN NAMA MASJID DAN INFO //
void loadTampilinfoConfig(const char *filetampilinfo) {
  File configTampilinfo = SPIFFS.open(filetampilinfo, "r");

  if (!configTampilinfo) {
    ESP.restart();
  }

  size_t size = configTampilinfo.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configTampilinfo.readBytes(buf.get(), size);


  DynamicJsonDocument doc(260);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  tpl_nama = doc["tpl_nama"];
  tpl_pesan = doc["tpl_pesan"];
  tpl_pesan2 = doc["tpl_pesan2"];
  tpl_pesan3 = doc["tpl_pesan3"];
  tpl_hijriyah = doc["tpl_hijriyah"];
  tpl_masehi = doc["tpl_masehi"];
  tpl_matikan = doc["tpl_matikan"];

  configTampilinfo.close();
}

// LOAD PENGATURAN ADZAN DAN IQOMAH //
void loadIqomahConfig(const char *fileiqomah) {
  File configIqomah = SPIFFS.open(fileiqomah, "r");

  if (!configIqomah) {
    ESP.restart();
  }

  size_t size = configIqomah.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configIqomah.readBytes(buf.get(), size);

  DynamicJsonDocument doc(300);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  lamaadzan = doc["lamaadzan"];
  lamaimsak = doc["lamaimsak"];
  iqomahsubuh = doc["iqomahsubuh"];
  iqomahdzuhur = doc["iqomahdzuhur"];
  iqomahashar = doc["iqomahashar"];
  iqomahmaghrib = doc["iqomahmaghrib"];
  iqomahisya = doc["iqomahisya"];

  configIqomah.close();
}

// LOAD WAKTU TARTIL //
void loadwaktuTartilConfig(const char *filewaktutartil) {
  File configwaktuTartil = SPIFFS.open(filewaktutartil, "r");

  if (!configwaktuTartil) {
    ESP.restart();
  }

  size_t size = configwaktuTartil.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configwaktuTartil.readBytes(buf.get(), size);

  DynamicJsonDocument doc(300);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    return;
  }

  strlcpy(imsak, doc["imsak"] | "", sizeof(imsak));
  strlcpy(subuh, doc["subuh"] | "", sizeof(subuh));
  strlcpy(dzuhur, doc["dzuhur"] | "", sizeof(dzuhur));
  strlcpy(ashar, doc["ashar"] | "", sizeof(ashar));
  strlcpy(maghrib, doc["maghrib"] | "", sizeof(maghrib));
  strlcpy(isya, doc["isya"] | "", sizeof(isya));
  strlcpy(jumat, doc["jumat"] | "", sizeof(jumat));

  configwaktuTartil.close();
}
