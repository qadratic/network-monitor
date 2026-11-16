#pragma once

#include <network_monitor_types.h>

#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C // Default I2C address for 0.96" OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define TEXT_SIZE 1

// Null MAC
uint8_t null_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Pin definitions
const int theButton = 14;
const int ledPin = 12;
const int buzzerPin = 13;

// Queue to hold MonitorPacket structures

std::queue<MonitorPacket*> packetQueue;
const size_t MAX_QUEUE_SIZE = 60; // Prevents running out of RAM

// Layout for the OLED display

struct screen_division rootDivision = {0, 0, 128, 64};

struct screen_division statusBarDivision = {rootDivision.x + 0, rootDivision.y + 0, 128, 10};
struct screen_division memoryPercentageDivision = {statusBarDivision.x, statusBarDivision.y + 0, 11*6, 10};
struct screen_division batteryPercentageDivision = {memoryPercentageDivision.width + 6, statusBarDivision.y + 0, 7*6, 10};

struct screen_division mainContentDivision = {0, statusBarDivision.height+1, 128, 54};

struct screen_division footerDivision = {0, SCREEN_HEIGHT - 10, 128, 10};


