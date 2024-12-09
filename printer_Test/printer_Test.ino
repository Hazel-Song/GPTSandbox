#include <SoftwareSerial.h>

// 定义 SoftwareSerial 引脚
// 初始化 DP-EH900 打印机
#define PRINTER_TX 9 // Arduino TX (连接打印机 RX)
#define PRINTER_RX 8 // Arduino RX (连接打印机 TX)
SoftwareSerial printer(PRINTER_TX, PRINTER_RX);

void setup() {
  // 初始化串口
  Serial.begin(9600); // 调试串口
  printer.begin(9600); // 打印机默认波特率为 9600

  Serial.println("DP-EH900 Printer Test");
  delay(1000);

  // 初始化打印机
  printer.write(0x1B);  // ESC
  printer.write(0x40);  // @ (初始化打印机)

  // 打印测试文本
  printer.print("Hello, DP-EH900 Printer!\n"); 
  printer.print("This is a test print from Arduino.\n");
  printer.print("Line 3: Printing with DP-EH900\n");

  // 换行指令，确保文本正确显示
  printer.write(0x0A); // Line feed (换行)

  // 发送切纸指令（确保打印完后切纸）
  uint8_t cutCommand[] = {0x1D, 0x56, 0x00}; // ESC + V + m
  printer.write(cutCommand, sizeof(cutCommand));

  // 延迟 2 秒，确保打印完成
  delay(2000);

  Serial.println("Test completed.");
}

void loop() {
  // 循环内无需代码，打印机操作一般在事件触发时进行
}
