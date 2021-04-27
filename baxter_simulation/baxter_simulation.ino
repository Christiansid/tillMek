const byte stopPin = 3;    // Input pin detecting when system has registered a stop
const byte floatPin = 4;   // Output pin sending simulated float switch signals to circuit
const byte led_arduino = 13;


unsigned long last_stop = 0;                // Keeps track of how often to stop the float switch simulation

volatile bool circuitState = false;         // For sending circuit state to serial
volatile bool simulateFloat = true;         // For sending simulation state to serial

// PUMPSIMULATION                             // 15ml/m - 350ml/m
int flowrate = 20;                            // Slagvolym 0.7ml/slag
int pump_delay = 1000/(flowrate/0.7)*60;      // 0.33 - 8.333 slag/s


void setup() {
  pinMode(floatPin, OUTPUT);
  pinMode(led_arduino, OUTPUT);
  pinMode(stopPin, INPUT);
  Serial.begin(9600);

  // SIMPLE HANDSHAKE
  Serial.print("M");              //Marco
  while (true) {
    delay(1000);
    if (Serial.read() == 'P'){    //Polo
      delay(4000);
      break;
    }
  }
  
  attachInterrupt(digitalPinToInterrupt(stopPin), circuitStop, CHANGE);
  sei();
}



void loop() { 
  

  // SIMULATE FLOAT SWITCHES
  if (simulateFloat){
    digitalWrite(floatPin, HIGH);
    digitalWrite(led_arduino, HIGH);
    delay(pump_delay/2);
    digitalWrite(floatPin, LOW);
    digitalWrite(led_arduino, LOW); 
    delay(pump_delay/2);

    sendInfo();
  } 
  else 
  {
   delay(pump_delay); 
   sendInfo();
  }


  int curr_time = millis();
  if (curr_time - last_stop > 10000){
    simulateFloat =! simulateFloat;
    last_stop = curr_time;
  }
 
}


// CONTROLLED BY MANUAL BUTTON (with debounce)
void simulateStop() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200){
      simulateFloat =! simulateFloat; 
      digitalWrite(led_arduino, LOW); 
  }
  last_interrupt_time = interrupt_time;


}


// TRIGGERED WHEN CIRCUIT RECOGNISES PUMP FAILURE
void circuitStop() {               
  circuitState = digitalRead(stopPin);
  sendInfo();

  simulateFloat = true;
  circuitState = false;

}


// SEND DATA
void sendInfo() {
  // Serial.println(millis());
  Serial.print('S');
  Serial.print(false);
  Serial.print(simulateFloat);
  Serial.print(circuitState);

}
