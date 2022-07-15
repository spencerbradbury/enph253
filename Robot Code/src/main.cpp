#include <Wire.h>
#include <Adafruit_SSD1306.h>


//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
#define MOTOR_LEFT_F PA_0
#define MOTOR_LEFT_B PA_1
#define MOTOR_RIGHT_F PA_2
#define MOTOR_RIGHT_B PA_3
#define LEFT_ENCODER PA5
#define RIGHT_ENCODER PA6
#define MOTOR_CLOCK_FREQ 100

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// void pwm_start(PinName pin, uint32_t clock_freq, uint32_t value, TimerCompareFormat_t resolution){}

int leftCount = 0;
int rightCount = 0;

int abcdefgh = 0;
#define LED_BUILTIN PC13

void handle_left_motor(){
  leftCount++;
}

void handle_right_motor(){
  rightCount++;
}

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTOR_LEFT_F, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(PA2, OUTPUT); //This and the next 3 lines are because this somehow breaks I2c
  pinMode(PA3, OUTPUT);
  // pinMode(MOTOR_RIGHT_F, OUTPUT);
  // pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER), handle_left_motor, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER), handle_right_motor, RISING);
  }

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  display_handler.clearDisplay();
  display_handler.setCursor(0,0);
  display_handler.println(abcdefgh);
  display_handler.print("Left: ");
  display_handler.println(leftCount);
  display_handler.print("Right: ");
  display_handler.println(rightCount);
  display_handler.display();
  abcdefgh++;

  pwm_start(MOTOR_LEFT_F, MOTOR_CLOCK_FREQ, 1024, RESOLUTION_10B_COMPARE_FORMAT);
  pwm_start(MOTOR_RIGHT_F, MOTOR_CLOCK_FREQ, 512, RESOLUTION_10B_COMPARE_FORMAT);

  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
};
