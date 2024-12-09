//8个button控制mbti输入

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// 初始化 LCD，I2C 地址为 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 初始化 PN532 NFC 读取器
#define PN532_SS 10              // PN532 的 NSS（片选引脚），连接 Arduino UNO 的 D4
Adafruit_PN532 nfc(PN532_SS);

// 定义按钮引脚
#define BUTTON_I 4   // 按钮 I 对应引脚 4
#define BUTTON_E 5   // 按钮 E 对应引脚 5
#define BUTTON_N 6   // 按钮 N 对应引脚 6
#define BUTTON_S 7   // 按钮 S 对应引脚 7
#define BUTTON_F 8   // 按钮 F 对应引脚 8
#define BUTTON_T 9   // 按钮 T 对应引脚 9
#define BUTTON_J 10  // 按钮 J 对应引脚 10
#define BUTTON_P 11  // 按钮 P 对应引脚 11

// 按钮组状态变量
int lastButtonState[8];  // 上一次的按钮状态
int buttonState[8];      // 当前的按钮状态
char mbti[5] = "    ";   // 存储当前的 MBTI 字母，初始为空格
int currentIndex = 0;    // 当前输入的字母位置（0-3）

// 按钮对应的字母
const char buttonLetters[8] = {'I', 'E', 'N', 'S', 'F', 'T', 'J', 'P'};

// 按钮引脚数组
const int buttonPins[8] = {BUTTON_I, BUTTON_E, BUTTON_N, BUTTON_S, BUTTON_F, BUTTON_T, BUTTON_J, BUTTON_P};

void setup() {
  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Choose MBTI:");

  // 初始化按钮引脚
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT);
    lastButtonState[i] = LOW;
  }

  // 初始化 NFC 读取器
  Serial.begin(115200);
  Serial.println("Initializing NFC...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  Serial.print("Found chip PN5"); 
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
  Serial.println("NFC Reader Ready!");
}

void loop() {
  // 遍历每个按钮，检测是否按下并更新 MBTI 字符串
  for (int i = 0; i < 8; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      delay(50); // 消抖
      if (buttonState[i] == HIGH) {
        handleButtonPress(i);
      }
    }
    lastButtonState[i] = buttonState[i];
  }

  // 检查是否有 NFC 卡片接近
  if (nfc.inListPassiveTarget()) {
    uint8_t uid[7] = {0}; // 最大支持 7 字节 UID
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
      // 清空 LCD 屏幕
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Complete Chosen");
      lcd.setCursor(0, 1);
      lcd.print("Printing...");

      // 模拟打印操作，延时显示“Printing...” 2 秒
      delay(2000);
    }
  }
}

// 处理按钮按下
void handleButtonPress(int buttonIndex) {
  int group = buttonIndex / 2;  // 按钮所属组（0-3）

  if (group == currentIndex) {
    // 当前组输入，更新字母
    char letter = buttonLetters[buttonIndex];
    mbti[currentIndex] = letter;  // 更新当前组对应字母
    updateLCD();  // 更新 LCD 显示

  } else if (group == currentIndex + 1) {
    // 切换到下一组输入
    currentIndex++;
    char letter = buttonLetters[buttonIndex];
    mbti[currentIndex] = letter;  // 更新当前组对应字母
    updateLCD();  // 更新 LCD 显示

    // 如果最后一组输入完成，提示下一步
    if (currentIndex == 3) {
      delay(2000); // 停留 2 秒
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(mbti);  // 将 MBTI 结果挪到第一行
      lcd.setCursor(0, 1);
      lcd.print("Choose a Card");
      lcd.setCursor(0, 2);
      lcd.print("card:");
    }
  }
}

// 更新 LCD 显示
void updateLCD() {
  lcd.setCursor(0, 1);
  lcd.print(mbti);  // 显示当前的 MBTI 字母
}
