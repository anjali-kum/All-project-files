const int stepPin = 5; 
const int dirPin = 2; 
const int enPin = 8;
bool flag = false; // Initialize the flag to false

void setup() {
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  
  // For testing, you can set the flag to true here
  flag = true; // Remove or modify this line as needed
}

void loop() {
  if (flag) {
    digitalWrite(dirPin, HIGH); // Set the direction to clockwise
    for (int x = 0; x < 800; x++) {
      digitalWrite(stepPin, HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin, LOW); 
      delayMicroseconds(500); 
    }
   
  }
}
