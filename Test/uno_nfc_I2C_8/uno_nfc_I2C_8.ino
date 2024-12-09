#include "pn532.h"
#include "pn532_uno.h"

uint8_t uid[MIFARE_UID_MAX_LENGTH];
int32_t uid_len = 0;

PN532 pn532;

// 上一次检测到的 UID
String lastDetectedUID = "";

void setup() {
  Serial.begin(115200);
  PN532_I2C_Init(&pn532);
  
  if (PN532_GetFirmwareVersion(&pn532, uid) == PN532_STATUS_OK) {
    Serial.println("PN532 initialized.");
  } else {
    Serial.println("PN532 initialization failed.");
    while (1);  // 停止程序
  }

  PN532_SamConfiguration(&pn532);  // 配置 PN532
}

void loop() {
  uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
  
  if (uid_len != PN532_STATUS_ERROR) {
    String detectedUID = "";
    for (uint8_t i = 0; i < uid_len; i++) {
      if (uid[i] <= 0xF) {
        detectedUID += "0";
      }
      detectedUID += String(uid[i], HEX);
    }
    detectedUID.toUpperCase();

    // 如果当前 UID 与上一次检测到的不一样，输出 UID
    if (detectedUID != lastDetectedUID) {
      lastDetectedUID = detectedUID;  // 更新状态
      String shortUID = detectedUID.substring(0, 6);  // 提取前 6 位
      //Serial.print("Detected UID: ");
      //Serial.println(detectedUID);
      //Serial.print("Short UID: ");
      Serial.println(shortUID);
    }
  } else {
    // 如果没有检测到卡片，清空上一次检测的 UID
    lastDetectedUID = "";
  }
}
