#include<Servo.h>
Servo edgeDetectorServo;

#define QSIZE 10
int queue[QSIZE]; // a queue to store most recent samples
int queue_index = 0;

//Motor Pins
const int motor1pin1 = 4;
const int motor1pin2 = 5;
const int motor2pin1 = 6;
const int motor2pin2 = 7;
const int motor1 = 9;
const int motor2 = 10;

//Calculatons for speed in Hertz to cm/s
float dimension = 154.40;
float seconds = 3.966;
float result = dimension / seconds;
unsigned int speed = (unsigned int) result;

//Table Dimensions
unsigned int tableWidth = 0;
unsigned int tableLength = 0;

unsigned long duration_us;

// Ultrasonic Sensor Pin Declarations
const int trigPin = 2;
const int echoPin = 3;

const int VACPin = 8; //Vacuum
const int servoPin = 11; //Servo
const int errorPin = 6; //Error LED

void setupMotorPins() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
}

void moveForward(int motor) {
  if (motor == 1) {
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
  } else if (motor == 2) {
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
  }
}

void moveBackward(int motor) {
  if (motor == 1) {
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
  } else if (motor == 2) {
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
  }
}
void stopBothMotors(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}
void moveBothMotorsForward() {
  moveForward(1);
  moveForward(2);
}

void moveBothMotorsBackward() {
  moveBackward(1);
  moveBackward(2);
}

void turn90Right() {
  moveForward(1);
  moveBackward(2);
  delay(700);
  stopBothMotors();
}


void sort(int arr[], int arr_size) {
  int i, j;
  for (i = 0; i < arr_size - 1; i++) {
    for (j = 0; j < arr_size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int tmp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = tmp;  
      }
    }  
  }
}

unsigned int getMedian() {
  int queue_sorted[QSIZE];
  int i = 0;
  for (i = 0; i < QSIZE; i++) {
    queue_sorted[i] = queue[i];
  }
  sort(queue_sorted, QSIZE);
  return queue_sorted[QSIZE/2];
}

void turn90Left() {
  moveForward(2);
  moveBackward(1);
  delay(700);
  stopBothMotors();
}

void performSpiralMotion(unsigned int width, unsigned int length) {
  digitalWrite(VACPin, HIGH); //turn on vacuum
  while(width > 0 && length > 0) {
    moveBothMotorsForward();
    delay(width * 10); // Adjust this multiplier to match the robot's speed
    stopBothMotors();
   
    turn90Right();
   
    // Move forward the length of the table
    moveBothMotorsForward();
    delay(length * 10); // Adjust this multiplier to match the robot's speed
    stopBothMotors();
   
    // Turn right
    turn90Right();
   
    // Decrease the dimensions for the next loop of the spiral
    width -= 2;
    length -= 2;
  }
  digitalWrite(VACPin, LOW); //turn off vacuum
 
}

unsigned int getDistance() {
  // generate 5-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration_us = pulseIn(echoPin, HIGH);
  float raw_dist= 0.017*duration_us;
  unsigned int dist = (unsigned int)raw_dist;
  Serial.print("distance: ");
  Serial.print(dist);
  Serial.println(" cm");
  return dist;
}



void setup() {
 Serial.begin(9600);
  pinMode(VACPin, OUTPUT);
  setupMotorPins();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(errorPin, OUTPUT);
  setupMotorPins();
  digitalWrite(VACPin, HIGH);
  edgeDetectorServo.attach(11);
 
}

void loop() {
  unsigned long startTime = millis();
  if(getDistance() < 30) {
   edgeDetectorServo.write(90);
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
  }
  else {
    unsigned long diffTime = millis() - startTime;
    if(!tableWidth) {
      tableWidth = speed * (diffTime/1000);
      stopBothMotors();
      delay(750);
      for(int i = 90; i <= 180; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      for(int i = 0; i < QSIZE; i++) {
        queue[i] = getDistance();
      }
      unsigned int rightDistance = getMedian();
      delay(750);
      for(int i = 180; i >=0; i--) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      for(int i = 0; i < QSIZE; i++) {
        queue[i] = getDistance();
      }
      unsigned int leftDistance = getMedian();
      delay(750);
      for(int i = 0; i <= 90; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
//      if((rightDistance < 30 && leftDistance < 30) || (rightDistance >= 30 && leftDistance >= 30)) {
//        while(1) {
//          stopBothMotors();
//          digitalWrite(VACPin, LOW);
//          edgeDetectorServo.detach();
//          digitalWrite(erro`rPin, HIGH);
//        }
//      }
      if (rightDistance < 30) {
        turn90Right();
        delay(1500);
      }
      else {
        turn90Left();
        delay(1500);
      }
    }
    else if(!tableLength) {
     
      tableLength = speed * (diffTime/1000);
      stopBothMotors();
      delay(750);
      for(int i = 90; i <= 180; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      for(int i = 0; i < QSIZE; i++) {
        queue[i] = getDistance();
      }
      unsigned int rightDistance = getMedian();
      delay(750);
      for(int i = 180; i >=0; i--) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      for(int i = 0; i < QSIZE; i++) {
        queue[i] = getDistance();
      }
      unsigned int leftDistance = getMedian();
      delay(750);
      for(int i = 0; i <= 90; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
//      if((rightDistance < 30 && leftDistance < 30) || (rightDistance >= 30 && leftDistance >= 30)) {
//        while(1) {
//          stopBothMotors();
//          digitalWrite(VACPin, LOW);
//          edgeDetectorServo.detach();
//          digitalWrite(erro`rPin, HIGH);
//        }
//      }
     if(!tableWidth) {
      tableWidth = speed * (diffTime/1000);
      stopBothMotors();
      delay(750);
      for(int i = 90; i <= 180; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      unsigned int rightDistance = getMedian();
      delay(750);
      for(int i = 180; i >=0; i--) {
        edgeDetectorServo.write(i);
        delay(10);
      }
      delay(100);
      unsigned int leftDistance = getMedian();
      delay(750);
      for(int i = 0; i <= 90; i++) {
        edgeDetectorServo.write(i);
        delay(10);
      }
//      if((rightDistance < 30 && leftDistance < 30) || (rightDistance >= 30 && leftDistance >= 30)) {
//        while(1) {
//          stopBothMotors();
//          digitalWrite(VACPin, LOW);
//          edgeDetectorServo.detach();
//          digitalWrite(erro`rPin, HIGH);
//        }
//      }
      if (leftDistance < 30) {
        turn90Left();
        delay(1500);
      }
      else {
        turn90Right();
        delay(1500);
      }
    }
    }
   
  }

 for (int v = 5000; v >= 0 ; v--){
  for (int y  = 3000; y >=0 ; y--){
    moveForward(2);
    moveForward(1);
    delay(y);
    stopBothMotors();
    y= y-999;
    }
     v = v-999;
     turn90Right; 
  }
 
  
 }
