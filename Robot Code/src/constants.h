#if !defined(CONSTANTS_H)
#define CONSTANTS_H

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // This display does not have a reset pin accessible

#define REFLECTANCE_THRESHOLD 200
#define EDGE_THRESHOLD 800
#define MOTOR_SPEED 65

#define PID_MAX_INT MOTOR_SPEED / 3
#define LEFT_CLAW_OPEN -10
#define LEFT_CLAW_CLOSED 70
#define LEFT_CLAW_NEUTRAL 10
#define LEFT_ARM_UP 30
#define LEFT_ARM_DOWN -85
#define LEFT_ARM_VERTICAL 15
#define RIGHT_CLAW_OPEN -30
#define RIGHT_CLAW_CLOSED 55
#define RIGHT_CLAW_NEUTRAL 5
#define RIGHT_ARM_UP -70
#define RIGHT_ARM_DOWN 45
#define RIGHT_ARM_VERTICAL -45

#endif // CONSTANTS_H
