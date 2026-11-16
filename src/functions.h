#pragma once

#include <Arduino.h>
#include <constants.h>
#include <string>
#include <vector>
#include <ESP8266WiFi.h>

void playBeeps(int count, int frequency, int duration)
{
  for (int i = 0; i < count; i++)
  {
    tone(buzzerPin, frequency);
    delay(duration);
    noTone(buzzerPin);
    delay(100);
  }
}

struct cursor_position* getVerticalCenterPosition(screen_division &division)
{
  struct cursor_position *center = new struct cursor_position;
//   center->x = (division.x + division.width / 2); 
  center->x = 0; // Align to the left edge of the division
  center->y = (division.y + division.height / 2);
  return center;
}

void clearDisplayDivision(Adafruit_SSD1306 &display, screen_division &division){
    display.fillRect(division.x, division.y, division.width, division.height, SSD1306_BLACK);
    display.display();
}

void displayMemoryPercentage(Adafruit_SSD1306 &display){
  uint32_t freeHeap = ESP.getFreeHeap();
  float memoryPercentage=100.0f*freeHeap/80000.0f;

  clearDisplayDivision(display, memoryPercentageDivision);
  display.setCursor(statusBarDivision.x, statusBarDivision.y);
  display.print("Mem: "+String(memoryPercentage)+"%");
  display.display();
}
void displayBatteryPercentage(Adafruit_SSD1306 &display){
  // uint32_t freeHeap = ESP.getFreeHeap();
  int batteryPercentage=100.0f*100.0f/100.0f;

  clearDisplayDivision(display, batteryPercentageDivision);
  display.setCursor(batteryPercentageDivision.x, batteryPercentageDivision.y);
  display.print("Bat: "+String(batteryPercentage)+"%");
  display.display();
}
void renderStatusBar(Adafruit_SSD1306 &display){
  displayMemoryPercentage(display);
  displayBatteryPercentage(display);
}
void displayAnnouncement(Adafruit_SSD1306 &display, std::string message)
{
    Serial.println(message.c_str());
//   display.clearDisplay();
  clearDisplayDivision(display, rootDivision);
  struct cursor_position* center = getVerticalCenterPosition(mainContentDivision);
  center->y -= (TEXT_SIZE * 8);
  int noOfLines= message.length() / (SCREEN_WIDTH / (TEXT_SIZE * 6)) + 1; // Calculate number of lines based on text width
  center->y -= (noOfLines * TEXT_SIZE * 8) / 2; // Adjust for text height, half of the total lines
  display.setCursor(center->x, center->y);
  display.print(message.c_str());
  display.setCursor(footerDivision.x, footerDivision.y); // Move cursor to the footer position
  display.print("- by qadratic <3");
  display.display();
}

void displayMACs(Adafruit_SSD1306 &display, std::vector<std::string> &MACs, int offset, std::string message)
{
  clearDisplayDivision(display, mainContentDivision);
  display.setCursor(mainContentDivision.x, mainContentDivision.y);
  display.print(message.c_str());
  display.println("(" + String(MACs.size()) + ")");
  for (uint i=offset;i<MACs.size();i++) {
    display.print(i+1);
    display.print(F(": "));
    display.println(MACs[i].c_str());
  }
  display.display();
}

bool insertUniqueMAC(std::vector<std::string> &MACs, const std::string &mac)
{
    if (std::find(MACs.begin(), MACs.end(), mac) == MACs.end())
    {
        MACs.push_back(mac);
        return true;
    }
    return false;
}

wifi_ap* getAPInfo(std::string ssid)
{
    int n = WiFi.scanNetworks(false, true);
    for (int i = 0; i < n; i++)
    {
        if (WiFi.SSID(i) == ssid.c_str())
        {
            wifi_ap* apInfo = new wifi_ap();
            memcpy(apInfo->bssid, WiFi.BSSID(i), sizeof(apInfo->bssid));
            apInfo->rssi = WiFi.RSSI(i);
            apInfo->channel = WiFi.channel(i);
            apInfo->authmode = WiFi.encryptionType(i);
            return apInfo;
        }
    }
    return nullptr; // Return nullptr if the SSID is not found
}

void wifi_sniffer_packet_handler(uint8_t *buff, uint16_t len)
{
  if (packetQueue.size() >= MAX_QUEUE_SIZE) {
    return;
  }
  // Type cast the received buffer into the wifi_promiscuous_pkt_t structure
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buff;

  // Type cast the payload of the received packet into the wifi_ieee80211_packet_t structure
  wifi_ieee80211_packet_t *pk = (wifi_ieee80211_packet_t*)&pkt->frame;

  // Access the MAC header and payload of the packet
  wifi_ieee80211_mac_hdr_t *hdr = &pk->hdr;

//   uint8_t *frame = buff + 12;
  wifi_header_frame_control_t *FC = (wifi_header_frame_control_t *)&hdr->frame_ctrl;
  if ((*(uint8_t*)FC & 0x0C) != 0x08) {
        return; // Not a data frame, ignore it
    }
//     uint8_t *addr1_destination = frame + 4;

    if (*hdr->addr1 & 0x01) {
        return; 
    }
    if (memcmp(hdr->addr1, null_mac, 6) == 0) {
        return;
    }
    // uint8_t *addr2_transmitter = frame + 10;

  // uint8_t *data = pk->payload;

  // Access the Frame Control field of the MAC header

//   Serial.println("Type: " + String(FC->type) + ", Subtype: " + String(FC->subtype) + ", To DS: " + String(FC->to_ds) + ", From DS: " + String(FC->from_ds));
//   Serial.println("Source MAC: " + String(hdr->addr2[0], HEX) + ":" + String(hdr->addr2[1], HEX) + ":" + String(hdr->addr2[2], HEX) + ":" + String(hdr->addr2[3], HEX) + ":" + String(hdr->addr2[4], HEX) + ":" + String(hdr->addr2[5], HEX));
//   Serial.println("Destination MAC: " + String(hdr->addr1[0], HEX) + ":" + String(hdr->addr1[1], HEX) + ":" + String(hdr->addr1[2], HEX) + ":" + String(hdr->addr1[3], HEX) + ":" + String(hdr->addr1[4], HEX) + ":" + String(hdr->addr1[5], HEX));
//   Serial.println("BSSID: " + String(hdr->addr3[0], HEX) + ":" + String(hdr->addr3[1], HEX) + ":" + String(hdr->addr3[2], HEX) + ":" + String(hdr->addr3[3], HEX) + ":" + String(hdr->addr3[4], HEX) + ":" + String(hdr->addr3[5], HEX));

  // Check if the packet is a data frame (type 2)

//   Serial.println("Struct MAC2: "+String(hdr->addr2[0], HEX) + ":" + String(hdr->addr2[1], HEX) + ":" + String(hdr->addr2[2], HEX) + ":" + String(hdr->addr2[3], HEX) + ":" + String(hdr->addr2[4], HEX) + ":" + String(hdr->addr2[5], HEX));
//   Serial.println("Math MAC2:   "+String(addr2_transmitter[0], HEX) + ":" + String(addr2_transmitter[1], HEX) + ":" + String(addr2_transmitter[2], HEX) + ":" + String(addr2_transmitter[3], HEX) + ":" + String(addr2_transmitter[4], HEX) + ":" + String(addr2_transmitter[5], HEX));
    // Serial.println("Struct pointer: "+String((uintptr_t)hdr->addr1));
    // Serial.println("Math pointer:   "+ String((uintptr_t)addr1_destination));

    MonitorPacket monitorPacket;
    monitorPacket.rssi = pkt->rx_ctrl.rssi;
    memcpy(monitorPacket.source_mac, hdr->addr2, 6);
    memcpy(monitorPacket.dest_mac, hdr->addr1, 6);
    memcpy(monitorPacket.addr3_mac, hdr->addr3, 6);
    packetQueue.push(&monitorPacket);
  

}
