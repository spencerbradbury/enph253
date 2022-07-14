#include <Wire.h>
#include <Adafruit_SSD1306.h>

//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
#define HALL_INPUT PA6
#define GRABBER_FORWARD PA5
#define SWITCH PA7
#define GRABBER_BACKWARD PA3

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(HALL_INPUT, INPUT_PULLUP);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(GRABBER_FORWARD, OUTPUT);
  pinMode(GRABBER_BACKWARD, OUTPUT);
}

void loop() {
    display_handler.clearDisplay();
    display_handler.setCursor(0,0);
    if (!digitalRead(HALL_INPUT)){
      display_handler.println("Bomb");
    }
    else{
      display_handler.println("No Bomb");
    }
    if (!digitalRead(SWITCH)){
      display_handler.println("Pressed");
      digitalWrite(GRABBER_BACKWARD, LOW);
      digitalWrite(GRABBER_FORWARD, HIGH);
    }
    else{
      display_handler.println("Not Pressed");
      digitalWrite(GRABBER_BACKWARD, LOW);
      digitalWrite(GRABBER_FORWARD, LOW);
    }
    display_handler.display();
};
