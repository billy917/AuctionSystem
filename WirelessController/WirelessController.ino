 int ledPin = 13;
int gatePin = 12;
int inputPin1 = 8;
int lastPin1State;

void setup()
{
  Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  pinMode(gatePin, OUTPUT);
  pinMode(inputPin1, INPUT);
  
  digitalWrite(ledPin, LOW);
  digitalWrite(gatePin, HIGH);
  lastPin1State = digitalRead(inputPin1);
}


void loop()
{
  int pin1State = digitalRead(inputPin1);
  Serial.println(pin1State);
  if(inputPin1 != lastPin1State){
    if(pin1State == HIGH){
      digitalWrite(ledPin, HIGH);
      digitalWrite(gatePin, LO);
    } else {
      digitalWrite(ledPin, LOW);
      digitalWrite(gatePin, HIGH);
    }
  }
  
  lastPin1State = pin1State;
  delay(200);
}

