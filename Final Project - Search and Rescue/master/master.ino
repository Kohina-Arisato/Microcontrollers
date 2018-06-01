//DEFINING PINS
//Motor A
int IN1 = 2; //Direction 1
int IN2 = 3; //Direction 2
int ENA = 9; //Speed

//Motor B
int IN3 = 4; //Direction 1
int IN4 = 5; //Direction 2
int ENB = 10; //Speed

//Bluetooth
int BT_Value_M;

//Ultrasonic sensor
int trig = 6;
int echo = 7;
long duration;
int distance;
int safe_dist;

//IR Receiver
int IR_rcvr = 12;
int IR_detection; 
int turn = 0; 

//Booleans
bool found_slave = false;
bool found_home = false;
bool following_master = false;
bool go_right = false;

//Counters
int degree_counter = 40;
int stops = 1;

//INITIALIZATION
void setup() {
  //DEFINING PIN MODES AND VARIABLES
  //Motor driver inputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  //Ultrasonic sensor
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //IR receiver
  pinMode(IR_rcvr, INPUT);

  //Serial communication
  Serial.begin(38400);
}

//MAIN
void loop() {
  Serial.println("Loop start");

  while(found_home == false){
    Serial.println("Enter while");
    turning_360();
  }

  //Serial.println("Below while");
  //follow_me();
}

//Turning 360°
void turning_360(){
  IR_detection = digitalRead(IR_rcvr);
  Serial.println(IR_detection);
  Serial.println(following_master);
   
  if (IR_detection == 0){
    if (following_master != true){
      found_slave = false;
    }

    else if (following_master == true){
      found_home = false;
    }

    Serial.println("degree_counter: ");
    Serial.println(degree_counter);
    
    if (degree_counter > 0){
      if (following_master == true){
        Serial.write(2); //"Stop"
      }
      
      Serial.println("Mayor a 0");
      turn_right();
      Serial.println("Turn right");
      delay (100);
      stop_motors();
      Serial.println("Stop motors");
      
      delay(100);
      degree_counter -= 1;
    }
    
    else {
      if (following_master == true){
        Serial.write(1); //"Follow me"
      }
      
      Serial.println("Degree counter = 0");
      forward();
      Serial.println("forward");
      delay(1000);
      stop_motors();
      Serial.println("stop motors");
      stops += 1;
      Serial.println(stops);
      
      Serial.println(go_right);
    
      if (go_right == true){
        Serial.println("go_right = true");
        turn_right();
        Serial.println("turn right");
        delay(200);
        forward();
        delay(500);
        stop_motors();

        Serial.write(3); //Find master
        
        Serial.println("stop motors");
        go_right = false;
      }
    
      else if (stops == 4){
        Serial.println("fourth stop");
        go_right = true;
        Serial.println(go_right);
        stops = 1;
      }

      degree_counter = 40;
    }
  }
      
  else{
    Serial.println("IR detection == 1");
    stop_motors();
    detected();
    //not_found = false;
  }
}

//Detected 
void detected(){
  Serial.println("IR detection was 1");
  IR_detection = digitalRead(IR_rcvr);
  Serial.println(IR_detection);

  safe_dist = ultrasonic_dist();
  Serial.println("Safe distance? ");
  Serial.println(safe_dist);

  if (IR_detection == 1 && safe_dist == 0){
    Serial.println("IR = 1 and dist > 10");
    forward();
    delay(100);
  }

  else if (IR_detection == 1 && safe_dist == 1){
    Serial.println("IR = 1 and dist = 10");
    Serial.println(following_master);
    Serial.println(found_home);
    Serial.println(found_slave);
    
    if (following_master == true){
      Serial.println("IR = 1 and dist = 10 - following master = true");
      found_home = true;
    }

    else{
      Serial.println("IR = 1 and dist = 10 - following master = false");
      if (found_slave == true){
        follow_me();
      }
      found_slave = true;
    }
  }

  else{
    Serial.println("IR = 0");
    
    if (following_master == true){
      found_home = false;
    }

    else{
      found_slave = false;
    }
  }
}

//Follow me
void follow_me(){
  Serial.println("Reached slave, now follow master");
  Serial.println(found_slave);
  stop_motors();

  if (found_slave == true){
    Serial.println("Follow me - not_found == false");
    Serial.write(0); //Found
    delay(500);

    if (Serial.available() > 0){
      Serial.println("Serial.available() > 0");
      BT_Value_M = Serial.read();
      
      if (BT_Value_M == 1){ //Slave detecta Master
        Serial.println("Received a Serial.read == 1");
        turn_right(); //180° turn
        Serial.println("turn right");
        delay(500);

        Serial.println("Writing \"follow me\"");
        Serial.write(1); //"Follow me"

        Serial.println("forward");
        forward();
        delay(1000);
        
        Serial.println("stop motors");
        stop_motors();

        following_master = true; 
      }
    }
  }
}

//MOTOR FUNCTIONS
void forward(){
  analogWrite(ENA, 250);
  analogWrite(ENB, 250);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward(){
  analogWrite(ENA, 250);
  analogWrite(ENB, 250);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turn_right(){
  analogWrite(ENA, 120);
  analogWrite(ENB, 175);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turn_left(){
  analogWrite(ENA, 175);
  analogWrite(ENB, 120);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stop_motors(){
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

//MEASURING DISTANCE FUNCTION
int ultrasonic_dist(){
     // clears the trigPin
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
  
    // sets the trigPin on HIGH state for 10 microseconds
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
  
    // reads echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);
  
    // calculate distance
    distance = duration*0.034/2;
    Serial.println("Distance: ");
    Serial.println(distance);
  
    if(distance <= 10){
      Serial.println("Close enough");
      return 1;
    }
    
    else{
      Serial.println(distance);
      return 0;
    }
    delay(500);
}
