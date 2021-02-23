const byte buttonPin = 2;  // Pin to control the Simulation
const byte stopPin = 3;    // Input pin detecting when system has registered a stop
const byte floatPin = 4;   // Output pin sending simulated float switch signals to circuit
const byte led_arduino = 13;


#define SEND_DELAY 1000   // Set time between messages to Serial port 
unsigned long last_send = 0;      // Keeps track of how often to send serial data

volatile bool circuitState = false;         // For sending circuit state to serial
volatile bool simulateFloat = true;         // For sending simulation state to serial

// PUMPSIMULATION                             // 15ml/m - 350ml/m
int flowrate = 15;                            // Slagvolym 0.7ml/slag
int pump_delay = 1000/(flowrate/0.7)*60;      // 0.33 - 8.333 slag/s

bool handshake = false;



void setup() {
  pinMode(floatPin, OUTPUT);
  pinMode(led_arduino, OUTPUT);
  pinMode(stopPin, INPUT);
  pinMode(buttonPin, INPUT);
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
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), simulateStop, RISING);
  attachInterrupt(digitalPinToInterrupt(stopPin), circuitStop, CHANGE);
  sei();
}



void loop() { 
  

  // SIMULATE FLOAT SWITCHES
  if (simulateFloat){
    digitalWrite(floatPin, HIGH);
    delay(pump_delay/2);
    digitalWrite(floatPin, LOW); 
    delay(pump_delay/2);

    sendInfo();
  } 
  else 
  {
   delay(1000); 
   sendInfo();
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

  if (circuitState) {
    digitalWrite(led_arduino, HIGH);
  }
  
}

// SEND DATA
void sendInfo() {
  // Serial.println(millis());
  Serial.print('S');
  Serial.print(false);
  Serial.print(simulateFloat);
  Serial.print(circuitState);

}

// RECIEVE DATA
void RecieveData() {
  if (Serial.available() > 0)
    {
        int rate = Serial.read();
        Serial.print(rate);
        Serial.print('\n');
        
        flowrate = rate;
        delay(1000);
        
    }
    else
    {
      Serial.print(10); // Failed.
    }

  
}
