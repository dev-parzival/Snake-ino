/*
 Name:		Snake-ino.ino
 Created:	23.11.2020 11:07:52
 Author:	parzival
*/

// imortiere bibilotheken
#include<LedControl.h>

// analog stick pins
const int		VRx = 4;
const int		VRy = 3;
const int		SW = 2;

// matrix pins
const int		DIN = 8;
const int		CS = 9;
const int		CLK = 10;

// initializing matrix
LedControl lc = LedControl(DIN, CLK, CS);

// game stat
int				TPS = 5;
int				GRID[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0}
};
int				PLAYER[2] = { 4, 4 };		// PLAYER X Y
int				APPLE[3] = { 1, 1, 0 };	// APPLE X Y T
int				DIRECTION = 1;				// 1: /\ | 2: -> | 3: \/ | 4: <- | 0: none
int				LASTPOS[2] = { 0, 0 };		// Position of the last snake part X Y
int				PARTS[8][8];
int				PARTMAX = 7;
int				PARTNUM = 1;
bool			BOTMODE = false;
int				POINTS = 0;

void resetSnake() {
	if ((digitalRead(SW) == 0)) BOTMODE = !BOTMODE;
	if ((BOTMODE == true)) {
		if (APPLE[0] < PLAYER[0] - 1) DIRECTION = 4;
		if (APPLE[0] > PLAYER[0] - 1) DIRECTION = 2;
		if (APPLE[1] < PLAYER[1] - 1) DIRECTION = 1;
		if (APPLE[1] > PLAYER[1] - 1) DIRECTION = 3;
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
	lc.shutdown(0, false);
	lc.setIntensity(0, 0.3);
	lc.clearDisplay(0);

	// pin setup
	pinMode(VRx, INPUT);
	pinMode(VRy, INPUT);
	pinMode(SW, INPUT);
	digitalWrite(SW, HIGH);

	// drawing start position
	lc.setLed(0, 4, 4, true);

	// spawn apple
	APPLE[0] = random(0, 8);
	APPLE[1] = random(0, 8);
	APPLE[2] = 0;

	Serial.begin(9600);
}

void updateDisplay() {
	lc.clearDisplay(0);
	for (int _y = 0; _y < 8; _y++) {
		for (int _x = 0; _x < 8; _x++) {
			GRID[_y][_x] = 0;
		}
	}

	drawApple();
	playerTick();
	updateParts();

	for (int _y = 0; _y < 8; _y++) {
		for (int _x = 0; _x < 8; _x++) {
			int __x = _x;
			int __y = (7 - _y);
			lc.setLed(0, __x, __y, (GRID[_y][_x] == 1) ? true : false);
		}
	}
}

void playerTick() {
	DIRECTION = getDirection();
	LASTPOS[0] = PLAYER[0];
	LASTPOS[1] = PLAYER[1];
	switch (DIRECTION)
	{
	case 1:
		PLAYER[1] = PLAYER[1] - 1;
		if (PLAYER[1] < 1) PLAYER[1] = 8;
		break;
	case 2:
		PLAYER[0] = PLAYER[0] + 1;
		if (PLAYER[0] > 8) PLAYER[0] = 1;
		break;
	case 3:
		PLAYER[1] = PLAYER[1] + 1;
		if (PLAYER[1] > 8) PLAYER[1] = 1;
		break;
	case 4:
		PLAYER[0] = PLAYER[0] - 1;
		if (PLAYER[0] < 1) PLAYER[0] = 8;
		break;
	default:
		break;
	}

	int _x = PLAYER[0] - 1;
	int _y = PLAYER[1] - 1;
	GRID[_y][_x] = 1;
}

void updateParts() {
	for (int p = 0; p < PARTNUM; p++)
	{
		int _x = LASTPOS[0] - 1;
		int _y = LASTPOS[1] - 1;
		LASTPOS[0] = PARTS[p][0] + 1;
		LASTPOS[1] = PARTS[p][1] + 1;
		PARTS[p][0] = _x;
		PARTS[p][1] = _y;

		GRID[_y][_x] = 1;
	}
}

int getDirection() {
	int _x = analogRead(VRx);
	int _y = analogRead(VRy);
	int _a = 1024 / 3;

	if (_x < _a) return 3;
	if (_x > (_a * 2)) return 1;

	if (_y < _a) return 4;
	if (_y > (_a * 2)) return 2;

	return DIRECTION;
}
void drawApple() {
	if ((PLAYER[0] - 1 == APPLE[0]) && (PLAYER[1] - 1 == APPLE[1])) {
		APPLE[0] = random(0, 8);
		APPLE[1] = random(0, 8);
		APPLE[2] = 0;

		PARTNUM++;

		if (PARTNUM > PARTMAX) PARTNUM = 5;

		POINTS++;
	}

	if (APPLE[2] == 0) {
		GRID[APPLE[1]][APPLE[0]] = 1;
		APPLE[2] = 1;
	}
	else {
		APPLE[2] = 0;
	}
}

void loop() {
	resetSnake();

	updateDisplay();

	Serial.print("\n\n\n\n\Score: ");
	Serial.println(POINTS);

	delay(1000 / TPS);
}