#if !defined(FUNCTIONS_H)
#define FUNCTIONS_H

int irError();

int tapeError();

void modulateMotors(int value);

void turn(int angle);

void correctedTurn(int angle);

bool turnToTape(int angle);

bool turnToIR(int angle);

void turnWide(int angle);

void driveSlowly(int distance);

#endif // FUNCTIONS_H
