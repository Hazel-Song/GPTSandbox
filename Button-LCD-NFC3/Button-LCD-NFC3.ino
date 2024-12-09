//可以和本地py控制的GPT实现通讯，4个按钮控制输入MBTI，NFC模块只检测一次
//配套本地文件为Response.py
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// 初始化 LCD，I2C 地址为 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 初始化 PN532 NFC 读取器
#define PN532_SS 10 // PN532 的片选引脚
Adafruit_PN532 nfc(PN532_SS);

// 定义按钮引脚
#define BUTTON_IE 4  // 按钮 IE 对应引脚 4
#define BUTTON_NS 5  // 按钮 NS 对应引脚 5
#define BUTTON_FT 6  // 按钮 FT 对应引脚 6
#define BUTTON_JP 7  // 按钮 JP 对应引脚 7

// 按钮组状态变量
int lastButtonState[4];  // 上一次的按钮状态
int buttonState[4];      // 当前的按钮状态
char mbti[5] = "    ";   // 存储当前的 MBTI 字母，初始为空格

// 按钮对应的选项切换
const char buttonLetters[4][2] = {{'I', 'E'}, {'N', 'S'}, {'F', 'T'}, {'J', 'P'}};

// 按钮引脚数组
const int buttonPins[4] = {BUTTON_IE, BUTTON_NS, BUTTON_FT, BUTTON_JP};

bool mbtiComplete = false;  // 标记 MBTI 是否输入完成
bool isCardDetected = false; // 标记是否已经检测到卡片
bool hasDetectedCard = false; // 标记是否已经成功检测到卡片（只检测一次）

String lastDetectedUID = "";  // 用于记录最后检测到的 UID

void setup() {
  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Choose MBTI:");

  // 初始化按钮引脚
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
    lastButtonState[i] = LOW;
  }

  // 初始化 NFC 读取器
  Serial.begin(115200);  // 设置串口波特率
  Serial.println("Initializing NFC...");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("NFC Reader Ready!");
}

void loop() {
  handleButtonInputs();  // 按钮输入处理
  handleNFCDetection();  // NFC 卡片检测处理
}

// 处理按钮输入逻辑
void handleButtonInputs() {
  if (mbtiComplete) return;  // 如果 MBTI 输入已完成，不再处理按钮输入

  for (int i = 0; i < 4; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      delay(50); // 消抖
      if (buttonState[i] == HIGH) {
        handleButtonPress(i);
      }
    }
    lastButtonState[i] = buttonState[i];
  }

  // 如果 MBTI 已经填满，标记完成并更新显示
  if (mbti[3] != ' ') {
    mbtiComplete = true;
    delay(3000); // 延时 3 秒
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mbti);  // 将 MBTI 结果移动到第一行
    lcd.setCursor(0, 1);
    lcd.print("Choose a Card");

    // 将 MBTI 和卡片编号一起发送到串口
    String mbtiString = String(mbti);
    String cardData = "MBTI: " + mbtiString;
    String shortUID = lastDetectedUID.substring(0, 6); // 提取前 6 位
    Serial.println(cardData + " " + shortUID);
  }
}

// NFC 检测处理逻辑
void handleNFCDetection() {
  if (hasDetectedCard) return; // 如果已经检测到卡片，不再进行任何操作

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

      // 如果是新卡片
      if (!hasDetectedCard) {
        hasDetectedCard = true;  // 设置为已检测状态
        String shortUID = detectedUID.substring(0, 6); // 提取前 6 位
        String mbtiString = String(mbti);

        // 打印 MBTI 和卡片短编号到串口
        Serial.println("MBTI: " + mbtiString + " Card: " + shortUID);

        // 在 LCD 上更新显示
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MBTI: " + mbtiString);
        lcd.setCursor(0, 1);
        lcd.print("Card: " + shortUID);
      }
    }
  }
}

// 处理按钮按下
void handleButtonPress(int buttonIndex) {
  int position = buttonIndex;  // 当前按钮对应的屏幕位置（0-3）

  // 切换当前按钮对应的字母
  char currentLetter = mbti[position];
  char newLetter = (currentLetter == buttonLetters[buttonIndex][0]) ? 
                    buttonLetters[buttonIndex][1] : 
                    buttonLetters[buttonIndex][0];
  
  // 更新 MBTI 和 LCD 显示
  mbti[position] = newLetter;
  updateLCD();
}

// 更新 LCD 显示
void updateLCD() {
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(i, 1);    // 第二行对应的列 (i,1)
    lcd.print(mbti[i]);     // 显示当前 MBTI 字母
  }
}
