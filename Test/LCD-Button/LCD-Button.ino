#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 初始化 LCD，I2C 地址为 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 定义按钮引脚
#define BUTTON_I 2
#define BUTTON_E 3
#define BUTTON_N 4
#define BUTTON_S 5
#define BUTTON_F 6
#define BUTTON_T 7
#define BUTTON_J 8
#define BUTTON_P 9

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
}

void loop() {
  // 遍历每个按钮
  for (int i = 0; i < 8; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);

    // 检测按钮按下并松开
    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      delay(50); // 消抖
      if (buttonState[i] == HIGH) {
        handleButtonPress(i);
      }
    }
    lastButtonState[i] = buttonState[i];
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