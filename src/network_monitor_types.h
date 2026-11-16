#pragma once

#ifndef NETWORK_MONITOR_TYPES_H
#define NETWORK_MONITOR_TYPES_H

#include <Arduino.h>
#include <queue>

// Guard the structures so they only apply to the correct architecture
#if defined(ESP8266)

struct __attribute__((packed)) wifi_pkt_rx_ctrl_t {
    signed rssi:8;            //< signal intensity of packet
    unsigned rate:4;          //< data rate
    unsigned is_group:1;
    unsigned :1;              //< reserve
    unsigned sig_mode:2;      //< 0:is not 11n packet; 1:is 11n packet
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned mcs:7;           //< if is 11n packet, shows the modulation(range from 0 to 76)
    unsigned cwb:1;           //< if is 11n packet, shows if is HT40 packet or not
    unsigned HT_length:16;    //< reserve
    unsigned smoothing:1;     //< reserve
    unsigned not_sounding:1;  //< reserve
    unsigned :1;              //< reserve
    unsigned aggregation:1;   //< Aggregation
    unsigned stbc:2;          //< STBC
    unsigned fec_coding:1;    //< Flag is set for 11n packets which are LDPC
    unsigned sgi:1;           //< SGI
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;     //< ampdu cnt
    unsigned channel:4;       //< which channel this packet in
    unsigned :4;              //< reserve
    signed noise_floor:8;
};



// 802.11 standard strucures

// Structure for the FRAME CONTROL field of the MAC header
struct __attribute__((packed)) wifi_header_frame_control_t{
    unsigned protocol:2;
    unsigned type:2;
    unsigned subtype:4;
    unsigned to_ds:1;
    unsigned from_ds:1;
    unsigned more_frag:1;
    unsigned retry:1;
    unsigned pwr_mgmt:1;
    unsigned more_data:1;
    unsigned wep:1;
    unsigned strict:1;
};

// Structure for the MAC header of an 802.11 packet
struct __attribute__((packed)) wifi_ieee80211_mac_hdr_t {
    struct wifi_header_frame_control_t frame_ctrl; // 2 bytes
    uint16_t duration_id; // 2 bytes
    uint8_t addr1[6]; // Destination Address (DA)
    uint8_t addr2[6]; // Source Address (SA)
    uint8_t addr3[6]; // BSSID (Access Point MAC)
    uint16_t seq_ctrl;
    uint8_t addr4[6]; // Optional Address 4 (used in WDS)
    uint16_t qos_ctrl;
};

struct __attribute__((packed)) wifi_ieee80211_packet_t {
    struct wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[2]; /* network data ended with 4 bytes csum (CRC32) */
};

// Structure for the promiscuous packet received from the WiFi driver
struct __attribute__((packed)) wifi_promiscuous_pkt_t {
    struct wifi_pkt_rx_ctrl_t rx_ctrl; //12 bytes
    struct wifi_ieee80211_packet_t frame;
};

struct wifi_ap {
    uint8_t bssid[6];
    char bssid_c_str[18]; // String representation of the BSSID
    int8_t rssi;
    uint8_t channel;
    uint8_t authmode;
};

struct MonitorPacket {
    int rssi;
    uint8_t source_mac[6];
    uint8_t dest_mac[6];
    uint8_t addr3_mac[6]; // BSSID (Access Point MAC)
    // Add extra metadata or payload arrays here if needed
};



#endif // ESP8266

#endif // NETWORK_MONITOR_TYPES_H

// Screen, OLED display

struct cursor_position{
    int x;
    int y;
};

struct screen_division{
    int x;
    int y;
    int width;
    int height;
};