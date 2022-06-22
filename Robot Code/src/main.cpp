#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
#define IR_INPUT PA6
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(4);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(IR_INPUT, INPUT_ANALOG);
}

void loop() {
    display_handler.clearDisplay();
    display_handler.setCursor(0,0);
    display_handler.println(analogRead(IR_INPUT));
    display_handler.display();
};
