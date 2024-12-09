#include <SPI.h>                // 包含 SPI 库
#include <Adafruit_PN532.h>     // 包含 PN532 NFC 库

#define PN532_SS 10              // PN532 的 NSS（片选引脚），连接 Arduino UNO 的 D4
Adafruit_PN532 nfc(PN532_SS);   // 使用 SPI 模式初始化 PN532

String lastDetectedUID = "";

void setup() {
    Serial.begin(9600);
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
    //Serial.println("Waiting for an NFC card...");

    if (nfc.inListPassiveTarget()) {
        uint8_t uid[7] = {0}; // 最大支持 7 字节 UID
        uint8_t uidLength;

        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
            //Serial.println("Card detected!");
            String detectedUID = "";
            for (uint8_t i = 0; i < uidLength; i++) {
                if (uid[i] <= 0xF) {
                    detectedUID += "0"; // 添加前导零
                }
                detectedUID += String(uid[i], HEX);
            }
            detectedUID.toUpperCase();

            if (detectedUID != lastDetectedUID) {
                lastDetectedUID = detectedUID;
                String shortUID = detectedUID.substring(0, 6); // 提取前 6 位
                Serial.println(shortUID);
            }
        }
    } else {
        //Serial.println("No card detected.");
        lastDetectedUID = ""; // 清空上一次 UID 状态
    }

    // 模块重新初始化，确保检测不会锁定
    nfc.begin();

    delay(200); // 增加循环间隔
}
