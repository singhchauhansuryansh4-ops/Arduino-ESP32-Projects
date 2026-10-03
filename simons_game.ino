// Simon Says RGB - Serial game (Common Cathode RGB LED)
// During play: sequence flashes (R, B, G)
// Wrong input -> RED stays ON
// Win -> GREEN stays ON
//
// Serial commands:
// - start<Enter> to begin
// - During play: type R or G or B then press Enter for each step

const int PIN_R = 9;    // PWM
const int PIN_G = 10;   // PWM
const int PIN_B = 11;   // PWM

const int MAX_STEPS = 100;       // win condition (increase/decrease to change difficulty)
const int ROUND_START = 1;

// Timing
const int STEP_FLASH_MS = 500;   // how long each color is lit
const int GAP_MS = 200;          // gap between flashes

// Input
const int INPUT_TIMEOUT_MS = 30000; // optional timeout per step

// Store sequence
char sequence[MAX_STEPS];
int roundLen = ROUND_START;

// Common cathode: PWM value ON when analogWrite(pin, value) is > 0
void ledOnChar(char c, int brightness = 255) {
  int r = 0, g = 0, b = 0;

  c = toupper(c);
  if (c == 'R') r = brightness;
  else if (c == 'G') g = brightness;
  else if (c == 'B') b = brightness;
  else return;

  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void allOff() {
  analogWrite(PIN_R, 0);
  analogWrite(PIN_G, 0);
  analogWrite(PIN_B, 0);
}

char randomColor() {
  int v = random(0, 3);
  if (v == 0) return 'R';
  if (v == 1) return 'G';
  return 'B';
}

char readColorInput() {
  unsigned long start = millis();
  while (millis() - start < INPUT_TIMEOUT_MS) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      line.toUpperCase();

      if (line.length() >= 1) {
        char c = line[0];
        if (c == 'R' || c == 'G' || c == 'B') return c;
      }

      Serial.println("Invalid input. Type R, G, or B then press Enter.");
      return '?';
    }
  }
  return '?'; // timeout
}

void playSequence(int len) {
  for (int i = 0; i < len; i++) {
    ledOnChar(sequence[i], 255);
    delay(STEP_FLASH_MS);
    allOff();
    delay(GAP_MS);
  }
}

void stopWithRed() {
  Serial.println("❌ Wrong! Game Over. (RED ON)");
  ledOnChar('R', 255);
  while (true) { /* stop here */ }
}

void stopWithGreen() {
  Serial.println("🎉 You win! (GREEN ON)");
  ledOnChar('G', 255);
  while (true) { /* stop here */ }
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  Serial.begin(9600);

  randomSeed(analogRead(A0)); // seed randomness

  Serial.println("=== Simon Says RGB ===");
  Serial.println("Type 'start' and press Enter to begin.");
  Serial.println("During play: type one of R, G, B then press Enter each step.");
  allOff();
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toLowerCase();

  if (cmd != "start") return;

  // Reset game
  roundLen = ROUND_START;
  allOff();

  while (true) {
    // Add one new random color to the sequence
    sequence[roundLen - 1] = randomColor();

    Serial.print("\nRound length: ");
    Serial.println(roundLen);

    // Show sequence
    playSequence(roundLen);

    // Player input
    for (int i = 0; i < roundLen; i++) {
      Serial.print("Step ");
      Serial.print(i + 1);
      Serial.print("/");
      Serial.print(roundLen);
      Serial.print(" (R/G/B): ");

      char guess = readColorInput();
      if (guess == '?') {
        stopWithRed();
      }

      Serial.println(guess);

      if (guess != sequence[i]) {
        stopWithRed();
      }
    }

    // Win condition
    roundLen++;

    if (roundLen > MAX_STEPS) {
      stopWithGreen();
    }

    Serial.println("✅ Correct! Next round...");
    delay(600);
  }
}