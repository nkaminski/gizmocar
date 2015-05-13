#include <Servo.h> 
 
Servo esc, steer;
volatile unsigned long ticks;

void r_ISR(){
  ticks++;
}

void setup() 
{ 
  Serial.begin(9600);
  steer.attach(5);
  esc.attach(6);
  steer.write(90);
  esc.writeMicroseconds(1500);
  delay(1000);
} 
 
void loop() 
{ 
  delay(100);
  Serial.println();
  Serial.println("Please power on the ESC now, then enter the % sign to begin. RAPID MOVEMENT WILL OCCUR, KEEP WHEELS OFF GROUND!");
  while(1){
    if(Serial.available()){
      if(Serial.read() == '%')
        break;
    }
  }
  Serial.println("left");
  steer.write(125);
  delay(1000);
  Serial.println("right");
  steer.write(55);
  delay(1000);
  Serial.println("center steering, preparing ESC test/calibration");
  steer.write(90);
  delay(1000);
  Serial.println("full forward");
  esc.writeMicroseconds(1000);
  delay(4000);
  esc.writeMicroseconds(1500);
  delay(1000);
  Serial.println("Brake/reverse");
  esc.writeMicroseconds(2000);
  delay(4000);
  esc.writeMicroseconds(1500);
  Serial.println("Preparing RPM sensor test");
  delay(4000);
  ticks=0;
  attachInterrupt(0,r_ISR,FALLING);
  Serial.println("Slowly rotate rear wheels by hand");
  while(ticks<3);
  Serial.println("RPM readout OK!, test complete");
  detachInterrupt(0);
} 

