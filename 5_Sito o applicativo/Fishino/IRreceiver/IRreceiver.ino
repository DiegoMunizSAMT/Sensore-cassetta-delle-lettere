void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
}

boolean oldVal = 0;

void loop() {
  digitalWrite(13,HIGH);
 int val = !digitalRead(2);
  if(val == !oldVal) {
    Serial.println(val); 
  }

  oldVal = val;
  
  
  delay(50);
}
