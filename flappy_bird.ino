#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Button
#define BUTTON_PIN 4

// OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// ---------------- BIRD ----------------

float birdX = 35;
float birdY = 30;

float velocity = 0;
float gravity = 0.4;
float jumpStrength = -2;


// ---------------- PIPE ----------------

float pipeX = 128;

int pipeGapY = 32;
int pipeWidth = 12;
int pipeGap = 22;

float pipeSpeed = 2;


// ---------------- SCORE ----------------

int score = 0;
int highScore = 0;


// ---------------- GAME ----------------

bool gameOver = false;


void setup() {

  // I2C
  Wire.begin(21, 22);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Random numbers
  randomSeed(analogRead(0));

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
}


void loop() {

  // =================================================
  // GAME OVER SCREEN
  // =================================================

  if (gameOver) {

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // GAME OVER
    display.setCursor(37, 15);
    display.println("GAME OVER");

    // Current score
    display.setCursor(32, 30);
    display.print("SCORE: ");
    display.println(score);

    // Highest score
    display.setCursor(32, 40);
    display.print("BEST:  ");
    display.println(highScore);

    // Restart instruction
    display.setCursor(27, 54);
    display.println("PRESS BUTTON");

    display.display();


    // Restart when button is pressed
    if (digitalRead(BUTTON_PIN) == LOW) {

      birdY = 30;
      velocity = 0;

      pipeX = 128;
      pipeGapY = random(18, 42);
      pipeGap = random(18, 28);

      score = 0;

      gameOver = false;

      delay(300);
    }

    return;
  }


  // =================================================
  // BIRD JUMP
  // =================================================

  if (digitalRead(BUTTON_PIN) == LOW) {

    velocity = jumpStrength;
  }


  // =================================================
  // BIRD PHYSICS
  // =================================================

  velocity = velocity + gravity;

  birdY = birdY + velocity;


  // Stop bird at ground
  if (birdY > 52) {

    birdY = 52;
    velocity = 0;
  }


  // =================================================
  // PIPE MOVEMENT
  // =================================================

  pipeX = pipeX - pipeSpeed;


  // =================================================
  // PIPE PASSED
  // =================================================

  if (pipeX < -20) {

    // Increase score
    score++;

    // Update highest score
    if (score > highScore) {
      highScore = score;
    }

    // New pipe
    pipeX = 128;

    pipeGapY = random(18, 42);

    pipeGap = random(18, 28);
  }


  // =================================================
  // COLLISION DETECTION
  // =================================================

  // Horizontal collision
  if (birdX + 3 >= pipeX &&
      birdX - 3 <= pipeX + pipeWidth) {

    // Vertical collision
    if (birdY - 3 < pipeGapY - pipeGap / 2 ||
        birdY + 3 > pipeGapY + pipeGap / 2) {

      gameOver = true;
    }
  }


  // =================================================
  // DRAW EVERYTHING
  // =================================================

  display.clearDisplay();


  // ---------------- PIPES ----------------

  // Top pipe
  display.fillRect(
    pipeX,
    0,
    pipeWidth,
    pipeGapY - pipeGap / 2,
    SSD1306_WHITE
  );


  // Bottom pipe
  display.fillRect(
    pipeX,
    pipeGapY + pipeGap / 2,
    pipeWidth,
    SCREEN_HEIGHT - (pipeGapY + pipeGap / 2),
    SSD1306_WHITE
  );


  // ---------------- BIRD ----------------

  display.fillCircle(
    birdX,
    birdY,
    3,
    SSD1306_WHITE
  );


  // ---------------- GROUND ----------------

  display.drawLine(
    0,
    55,
    127,
    55,
    SSD1306_WHITE
  );


  // =================================================
  // SCORE DISPLAY
  // =================================================

  // Small black background for score
  display.fillRect(0, 0, 60, 9, SSD1306_BLACK);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(1, 0);
  display.print("S:");
  display.print(score);

  display.setCursor(30, 0);
  display.print("BEST:");
  display.print(highScore);


  // Send to OLED
  display.display();


  delay(30);
}