#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <set>
#include <string>
#include <queue>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "functions.h"
#include "constants.h"
#include "secrets.h"

extern "C"
{
#include "user_interface.h"
}
#include "network_monitor_types.h"

struct wifi_ap ap;

// #define WIFI_SSID <<secrets.h>>
std::set<std::string> knownMACs = {};

std::vector<std::string> currentMACs;

std::vector<std::string> unknownMACs;

wifi_ap *apInfo = nullptr;



void setup()
{
  Serial.begin(115200);

  pinMode(theButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Wire.begin(4, 5);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  displayAnnouncement(display, "Network Monitor Starting...");
  delay(3000);

  WiFi.mode(WIFI_STA);

  while (!apInfo)
  {
    display.clearDisplay();
    display.setCursor(mainContentDivision.x, mainContentDivision.y);
    Serial.printf("Scanning for WiFi networks...\n");
    display.println(F("Scanning for WiFi networks..."));
    display.display();
    apInfo = getAPInfo(WIFI_SSID);
    delay(2000);
  }

  snprintf(apInfo->bssid_c_str, sizeof(apInfo->bssid_c_str), "%02X:%02X:%02X:%02X:%02X:%02X", apInfo->bssid[0], apInfo->bssid[1], apInfo->bssid[2], apInfo->bssid[3], apInfo->bssid[4], apInfo->bssid[5]);

  Serial.printf("SSID '%s' found. Channel: %d, MAC: %s\n", WIFI_SSID, apInfo->channel, apInfo->bssid_c_str);
  display.printf("SSID '%s' found. Channel: %d, MAC: %s\n", WIFI_SSID, apInfo->channel, apInfo->bssid_c_str);
  display.display();
  delay(5000);

  WiFi.disconnect();
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);

  Serial.printf("Filtering for MAC: %s\n", apInfo->bssid_c_str);

  wifi_set_promiscuous_rx_cb(wifi_sniffer_packet_handler);
  wifi_set_channel(apInfo->channel);
  wifi_promiscuous_enable(1);
}

bool reRender = true;
int offset=0;
void loop(){
  // return;
  if (reRender)
  {
    reRender = false;
    // status bar
    renderStatusBar(display);
    if (unknownMACs.size() > 0)
    {
      offset = 0;
      digitalWrite(ledPin, HIGH);
      displayMACs(display, unknownMACs, 0, "!Unknown MACs:");
    } else if (currentMACs.size() > 0){
      displayMACs(display, currentMACs, offset, "Current MACs:");
    } else{
      displayAnnouncement(display, "Radar ON");
    }
  }

  if (digitalRead(theButton) == LOW)
  {
    if (unknownMACs.size() > 0)
    {
      digitalWrite(ledPin, LOW);
      knownMACs.insert(unknownMACs[0]);
      unknownMACs.erase(unknownMACs.begin());
    } else{
      if (offset + 1 < currentMACs.size()) {
        offset += 1;
      } else {
        offset = 0;
      }
    }
    reRender = true;
    delay(500); // Debounce delay
  }

  // display.clearDisplay();
  // display.setCursor(mainContent.x, mainContent.y);
  // display.println(F("Filtering for MAC:"));
  // display.println(apInfo->bssid_c_str);
  // display.print(F("Channel:"));
  // display.println(WiFi.channel());
  // display.display();

  if (!packetQueue.empty())
  {
    MonitorPacket *monitorPacket = packetQueue.front();
    packetQueue.pop();
    // Serial.println("Comparing "+ String(monitorPacket->source_mac[0], HEX) + ":" + String(monitorPacket->source_mac[1], HEX) + ":" + String(monitorPacket->source_mac[2], HEX) + ":" + String(monitorPacket->source_mac[3], HEX) + ":" + String(monitorPacket->source_mac[4], HEX) + ":" + String(monitorPacket->source_mac[5], HEX));
    // Serial.println("With "+ String(apInfo->bssid[0], HEX) + ":" + String(apInfo->bssid[1], HEX) + ":" + String(apInfo->bssid[2], HEX) + ":" + String(apInfo->bssid[3], HEX) + ":" + String(apInfo->bssid[4], HEX) + ":" + String(apInfo->bssid[5], HEX));
    // Serial.println("Checking packet");
    if (memcmp(apInfo->bssid, monitorPacket->source_mac, 6) != 0)
      return;
    // Serial.println("Checked packet");

    char mac_char[18];
    snprintf(mac_char, sizeof(mac_char), "%02X:%02X:%02X:%02X:%02X:%02X", monitorPacket->dest_mac[0], monitorPacket->dest_mac[1], monitorPacket->dest_mac[2], monitorPacket->dest_mac[3], monitorPacket->dest_mac[4], monitorPacket->dest_mac[5]);
    std::string mac(mac_char);
    insertUniqueMAC(currentMACs, mac);

    if (knownMACs.find(mac) == knownMACs.end() && insertUniqueMAC(unknownMACs, mac))
    {
      // Serial.printf("Unknown MAC detected: %s\n", mac.c_str());
      // display.clearDisplay();
      // display.setCursor(mainContent.x, mainContent.y);
      // display.println(F("Unknown MAC detected:"));
      // display.println(mac.c_str());
      // display.display();
      digitalWrite(ledPin, HIGH);

      playBeeps(3, 1000, 100);
      reRender = true;

      // Add the unknown MAC to the known MACs set
    }
    // Serial.println("Queue size: " + String(packetQueue.size()));
  }
}