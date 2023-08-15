#include <Arduino.h>
#include <M5Stack.h>
#include <Serial_CAN_Module.h>

#define STANDARD_CAN_11BIT 1 // That depands on your car. some 1 some 0.

Serial_CAN can;

#define can_tx 21
#define can_rx 22

#define PID_ENGIN_PRM 0x0C
#define PID_VEHICLE_SPEED 0x0D
#define PID_COOLANT_TEMP 0x05

void sendPid(unsigned char __pid) {
  M5.Lcd.println("sendPid: start");
  unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};

  // can.send(0x7DF, 0, 0, 8, tmp); // SEND TO ID:0X55
  can.send(0x18db33f1, 1, 0, 8, tmp); // SEND TO ID:0X55
  M5.Lcd.println("sendPid: end");
}

bool getRPM(int *r) {
  M5.Lcd.println("Start getRPM: ");
  sendPid(PID_ENGIN_PRM);
  unsigned long __timeout = millis();

  while (millis() - __timeout < 1000) {
    unsigned long id = 0;
    unsigned char buf[8];
    M5.Lcd.println("Received: ");

    if (can.recv(&id, buf)) { // check if get data

      if (buf[1] == 0x41) {
        *r = (256 * buf[3] + buf[4]) / 4;
        return 1;
      }
    }
  }

  return 0;
}

void setup() {
  M5.begin(true, false, true, false);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN, BLACK);
  // 文字描画
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  // Serial1.begin(115200, SERIAL_8N1, can_rx, can_tx); // RX,TX

  // can.begin(Serial1, 115200);
}

bool isStart = false;
unsigned long baud = 9600;

void loop() {
  M5.update();
  M5.Lcd.setCursor(M5.Lcd.width() * 0.7, 0);
  M5.Lcd.printf("%6ld", baud);

  if (M5.BtnA.wasReleased()) {
    if (baud == 9600) {
      baud = 115200;
    }
    else if (baud == 115200) {
      baud = 500000;
    }
    else if (baud == 500000) {
      baud = 9600;
    }
  }
  if (M5.BtnB.wasReleased()) {
    isStart = true;
  }

  if (isStart) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("CAN setup");
    M5.Lcd.println("end");
    Serial1.begin(baud, SERIAL_8N1, can_rx, can_tx); // RX,TX

    can.begin(Serial1, baud);

    M5.Lcd.println("Start: ");
    int __rpm = 0;
    int ret = getRPM(&__rpm);
    if (ret) {
      M5.Lcd.print("Engin Speed: ");
      M5.Lcd.print(__rpm);
      M5.Lcd.println(" rpm");
    }
    delay(500);
  }
  delay(1);
}
