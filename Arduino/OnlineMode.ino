void onlineMode() {
  if (once) {
    WiFi.begin(ssid, pass);

    if (WiFi.status() == WL_NO_MODULE) {
      
    }
  }
}