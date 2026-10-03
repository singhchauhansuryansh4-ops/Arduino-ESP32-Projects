int irSensor = 2;
int buzzer = 8;

void setup() {
  pinMode(irSensor, INPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  int sensorValue = digitalRead(irSensor);

  if (sensorValue == LOW) {
    // Object detected
    digitalWrite(buzzer, HIGH);
    Serial.println("Object Detected!");
  }
  else {
    // No object detected
    digitalWrite(buzzer, LOW);
    Serial.println("No Object");
  }

  delay(100);
}