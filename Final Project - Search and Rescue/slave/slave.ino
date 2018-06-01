//DEFINING PINS
//Motor A
int IN1 = 2; //Direction 1
int IN2 = 3; //Direction 2
int ENA = 9; //Speed

//Motor B
int IN3 = 4; //Direction 1
int IN4 = 5; //Direction 2
int ENB = 10; //Speed

//Ultrasonic sensor
int trig = A0;
int echo = A1;
long duration;
int distance;
int cm_apart;
bool found;

//IR Emitter
int IR_mttr = 8;

//Bluetooth
int BT_Value_S;

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

  //IR Emitter
  pinMode(IR_mttr, OUTPUT);

  //Serial communication
  Serial.begin(38400);

  found = false;
}

//MAIN
void loop() {
  //Waiting for master to find slave
  to_be_found();

  //Slave following Master
  follow_master();
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
  analogWrite(ENA, 50);
  analogWrite(ENB, 250);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turn_left(){
  analogWrite(ENA, 250);
  analogWrite(ENB, 50);
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
    
    return distance;
}

//EMITTER FUNCTION
void IR_LED(){
  //IR light ON/OFF 2mS
  digitalWrite(IR_mttr, HIGH);
  //Serial.println(IR1);
  delay(20);
  digitalWrite(IR_mttr, LOW);
  //Serial.println(IR1);
  delay(20);
}

//WAITING FUNCTION
void to_be_found(){
  Serial.println("Entered to_be_found");
  while(found == false){
    //Emitting IR LED signal
    Serial.println("Entered while(found == false)");
    IR_LED();

    //Reading Master's message
    if (Serial.available() > 0){
      Serial.println("Serial.available");
      BT_Value_S = Serial.read();
      Serial.println(BT_Value_S);
      
      if (BT_Value_S == 0){
        Serial.println("BTValue = 0");
        if (ultrasonic_dist() > 2 && ultrasonic_dist < 25){
          Serial.println("ultrasonic_dist() > 2 && ultrasonic_dist < 25");
          Serial.write(1); //Master is in front of slave
          delay(600);
          
          BT_Value_S = Serial.read();
          if (BT_Value_S == 1){ //Master has found slave
            Serial.println("Master has found slave");
            found = true; //Ready to follow
          }
          
          else{
            Serial.println("Master not in front");
            Serial.write(0);
            // If master isn't in front of slave, master must place itself in front of slave, 
            // or slave must turn towards master
          }
        }
      }
    }
  }
}

//OBJECT FOLLOWING
//1) Receive master's directions
void follow_master(){
  if (found == true){
    Serial.println("found = true");
    while (true){
      if (Serial.available() > 0){
        int master_dir = Serial.read();
        Serial.println(master_dir);

        if (master_dir == 1){
          Serial.println("master_dir = 1"); 
          keep_distance();
        }

        else if (master_dir == 2){ 
          Serial.println("master_dir = 2");
          stop_motors();
        }

        else if (master_dir == 3){
          Serial.println("master_dir = 3");
          turning_360();
        }
      } 
    }
  }
}

//2) Maintain established distance between Master and Slave
void keep_distance(){
  int current_dist = ultrasonic_dist();
  Serial.println(current_dist);

  if (4 < current_dist <= 20){
     int new_dist = ultrasonic_dist();

     while (4 < new_dist <= 20){
      backward();
      new_dist = ultrasonic_dist();
     }

     stop_motors();
  }

  else if (20 < current_dist < 50){
     int new_dist = ultrasonic_dist();
  
     while (20 < current_dist < 50){
      forward();
      new_dist = ultrasonic_dist();
     }
  
     stop_motors();
  }
}

//3) Identify Master's direction
void turning_360(){
  int current_dist = ultrasonic_dist();
  Serial.println(current_dist);

  while (current_dist > 25){
    Serial.println("Mayor a 0");
    turn_right();
    Serial.println("Turn right");
    delay (100);
    stop_motors();
    Serial.println("Stop motors");
    
    delay(100);
  }

  stop_motors();
  keep_distance();
}

