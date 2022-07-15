#include <Wire.h>
#include <Adafruit_SSD1306.h>


#define REFLECTANCE_THRESHOLD 300
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
#define REFLECTANCE_INPUT_LEFT PA6
#define REFLECTANCE_INPUT_RIGHT PA7
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(REFLECTANCE_INPUT_LEFT, INPUT_ANALOG);
  pinMode(REFLECTANCE_INPUT_RIGHT, INPUT_ANALOG);

}

void loop() {
    bool leftValue = (analogRead(REFLECTANCE_INPUT_LEFT) > REFLECTANCE_THRESHOLD) ? true : false;
    bool rightValue = (analogRead(REFLECTANCE_INPUT_RIGHT) > REFLECTANCE_THRESHOLD) ? true : false;
    display_handler.clearDisplay();
    display_handler.setCursor(0,0);
    display_handler.print("Left: ");
    display_handler.println(leftValue);
    display_handler.print("Right: ");
    display_handler.println(rightValue);
    display_handler.display();
};
