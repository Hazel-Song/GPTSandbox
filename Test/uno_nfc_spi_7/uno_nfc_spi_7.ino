#include <SPI.h>                // 包含 SPI 库
#include <Adafruit_PN532.h>     // 包含 PN532 NFC 库

#define PN532_SS 4              // PN532 的 NSS（片选引脚），连接 Arduino UNO 的 D4
Adafruit_PN532 nfc(PN532_SS);   // 使用 SPI 模式初始化 PN532

String lastDetectedUID = "";
bool cardPresent = false;       // 标记当前是否有卡片在检测范围内

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing NFC...");

    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("Didn't find PN53x board");
        while (1);
    }

    Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

    nfc.SAMConfig();
    Serial.println("NFC Reader Ready!");
}

void loop() {
    // 检测 NFC 卡片
    if (nfc.inListPassiveTarget()) {
        uint8_t uid[7] = {0}; // 最大支持 7 字节 UID
        uint8_t uidLength;

        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
            String detectedUID = "";
            for (uint8_t i = 0; i < uidLength; i++) {
                if (uid[i] <= 0xF) {
                    detectedUID += "0"; // 添加前导零
                }
                detectedUID += String(uid[i], HEX);
            }
            detectedUID.toUpperCase();

            // 如果检测到的新 UID 与上次不同，或者卡片是第一次检测到
            if (!cardPresent || detectedUID != lastDetectedUID) {
                lastDetectedUID = detectedUID;
                cardPresent = true; // 更新状态，表示卡片在范围内
                String shortUID = detectedUID.substring(0, 6); // 提取前 6 位
                Serial.println(shortUID);
            }
        }
    } else {
        // 如果没有检测到卡片，重置状态
        if (cardPresent) {
            Serial.println("Card removed.");
        }
        cardPresent = false; // 卡片移开，重置状态
        lastDetectedUID = ""; // 清空上一次 UID
    }

    delay(500); // 增加检测间隔，避免频繁循环
}
