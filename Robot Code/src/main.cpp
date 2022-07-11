#include <Wire.h>
#include <Adafruit_SSD1306.h>

//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
#define MOTOR_LEFT PA_1
#define MOTOR_RIGHT PA_2
#define POT_PIN PA_6
#define MOTOR_CLOCK_FREQ 100
#define trigPin PA8
#define echoPin PA9

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// void pwm_start(PinName pin, uint32_t clock_freq, uint32_t value, TimerCompareFormat_t resolution){}

int getDistance(){
  int duration;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return((duration/2) / 29.1);
}

void setup() {
  // Serial.begin(9600);
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(4);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(MOTOR_LEFT, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  }

void loop() {
  int distance;
  distance = getDistance();
  display_handler.clearDisplay();
  display_handler.setCursor(0,0);
  display_handler.println(distance);
  // display_handler.println("hEllo");
  display_handler.display();  
  // delay(400);
  // display_handler.setCursor(0,1);
  // display_handler.clearDisplay();
  // display_handler.print("world");
  // display_handler.display();
  // delay(400);
  // Serial.print(distance);
  };


