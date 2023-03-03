#include <Wire.h>
#include <PixyI2C.h>
PixyI2C pixy;
#include <Adafruit_SH1106_STM32.h>
Adafruit_SH1106 display(-1);

void inti(){
  // -------------------- Motor Settings
  pinMode(PB15,OUTPUT);
  pinMode(PB14,OUTPUT);
  pinMode(PB13,OUTPUT);
  pinMode(PB12,OUTPUT);
  pinMode(PA8,PWM);
  pinMode(PB8,PWM);
  pinMode(PB7,PWM);
  pinMode(PB6,PWM);
  motor(0,0,0,0);
  // -------------------- OLED Display Settings
  display.begin(0x2 , 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // -------------------- Loading
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Loading");
  display.display();
  delay(500);
  display.print(".");
  display.display();
  delay(500);
  display.print(".");
  display.display();
  delay(500);
  display.print(".");
  display.display();
  delay(500);
  display.setTextSize(1);
  // -------------------- Pixy Settings 
  pixy.init(); 
  // -------------------- Serial Settings for GY-25
  Serial1.begin(115200);
  Serial1.write(0xA5);
  Serial1.write(0x54);
  delay(500);
  Serial1.write(0xA5);
  Serial1.write(0x51);
}
void sensors(){
  // -------------------- Set Button
  if(digitalRead(PB5))
  {
    digitalWrite(PC13,0);
    Serial1.write(0XA5);
    Serial1.write(0X55);
    while(digitalRead(PB5));
  }
  // -------------------- GY-25 Read Data
  Serial1.write(0xA5);
  Serial1.write(0x51);
  while(true){
    buff[counter] = Serial1.read();
    if(counter == 0 && buff[0] != 0xAA) break;
    counter ++;
    if(counter ==8){
      counter=0;
      if(buff[0]== 0xAA && buff[7] == 0x55){
          GY=(int16_t) (buff[1]<<8|buff[2])/100;
      }
    }
  }
  // -------------------- Pixy Read Data
  uint16_t blocks;
  blocks = pixy.getBlocks();
  is_ball = false;
  if(blocks){
    for(int i=0;i<blocks;i++ ){
      if(pixy.blocks[i].signature == 1){
        x_ball = pixy.blocks[i].y;
        y_ball = pixy.blocks[i].x;
        angle_ball = get_angle(x_ball, y_ball);
        direction_ball = get_direction(angle_ball);
        distance_ball = sqrt(pow(x_ball - x_robot, 2) + pow(y_ball - y_robot, 2));
        is_ball = true;
      }
    }
  }
  // -------------------- Sharp Read Data
  shb = analogRead(PA0);
  shr = analogRead(PA1);
  shl = analogRead(PA2);
  dif = (shl - shr)/18;
}
int get_angle(int x, int y){
  int angle = atan2(y - y_robot, x - x_robot)*180/PI;
  angle += 90;
  if(angle > 360) angle -= 360;
  if(angle < 0)   angle += 360;
  return angle;
}
int get_direction(int angle){
  int direction;
  for(int i=0; i<16; i++){
    if((angle - 11.25 >= i*22.5) && (angle - 11.25 < (i+1) * 22.5)) direction = i + 1; 
  }
  if(angle <= 11.25 || angle >= 348.5)   direction = 0;
  return direction;
}
void print_all(){
  display.clearDisplay();
  // --------------------- Pixy Print
  display.setCursor(0, 0);
  display.print("X:");
  display.println(x_ball);
  display.print("Y:");
  display.println(y_ball);
  display.print("dir:");
  display.println(direction_ball);
  display.print("dis:");
  display.println(distance_ball);
  // --------------------- Sharp Data Print
  display.print("shb:");
  display.println(shb);
  display.print("shl:");
  display.println(shl);
  display.print("shr:");
  display.println(shr);
  display.print("dif:");
  display.println(dif);
  
  // --------------------- GY-25 Print Circle
  display.drawCircle(80, 32, 12, WHITE);
  display.drawLine(80 + sin(GY * PI/180) * 9, 32 - cos(GY * PI/180) * 9, 80 - sin(GY * PI/180) * 9, 32 + cos(GY * PI/180) * 9, WHITE);
  display.fillCircle(80 - sin(GY * PI/180) * 9, 32 + cos(GY * PI/180) * 9, 2, WHITE);
  // --------------------- Ball Print Circle
  if(is_ball)
    display.fillCircle(80 + sin(angle_ball * PI/180) * 20, 32 - cos(angle_ball * PI/180) * 20, 3, WHITE);
  // --------------------- Out Shape
  if(shr > Wall_Distance) display.fillRect(105, 27, 3, 10, WHITE);
  if(shl > Wall_Distance) display.fillRect(53, 27, 3, 10, WHITE);
  if(shb > Wall_Distance) display.fillRect(75, 57, 10, 3, WHITE);
  

  display.display();
}
void motor(int ML1, int ML2 , int MR2 , int MR1) {
  ML1 += GY;
  ML2 += GY;
  MR2 += GY;
  MR1 += GY;
  ML1 *= 235;
  ML2 *= 255;
  MR2 *= 255;
  MR1 *= 235;
  if(ML1 > 65535) ML1 = 65535;
  if(ML2 > 65535) ML2 = 65535;
  if(MR2 > 65535) MR2 = 65535;
  if(MR1 > 65535) MR1 = 65535;
  if(ML1 < -65535) ML1 = -65535;
  if(ML2 < -65535) ML2 = -65535;
  if(MR2 < -65535) MR2 = -65535;
  if(MR1 < -65535) MR1 = -65535;


  
  if(ML1>0){
    digitalWrite(PB15,0);
    pwmWrite(PA8,ML1);
  }
  else{
    digitalWrite(PB15,1);
    pwmWrite(PA8,ML1+65535);
  }
  if(ML2>0){
    digitalWrite(PB14,0);
    pwmWrite(PB8,ML2);
  }
  else{
    digitalWrite(PB14,1);
    pwmWrite(PB8,ML2+65535);
  }
  if(MR2>0){
    digitalWrite(PB13,0);
    pwmWrite(PB7,MR2);
  }
  else{
    digitalWrite(PB13,1);
    pwmWrite(PB7,MR2+65535);
  }
  if(MR1>0){
    digitalWrite(PB12,0);
    pwmWrite(PB6,MR1);
  }
  else{
    digitalWrite(PB12,1);
    pwmWrite(PB6,MR1+65535);
  }
}
void move(int direction){
  if(direction ==  0) motor(v,v,-v,-v);
  if(direction ==  1) motor(v,v/2,-v,-v/2);
  if(direction ==  2) motor(v,0,-v,0);
  if(direction ==  3) motor(v,-v/2,-v,v/2);
  if(direction ==  4) motor(v,-v,-v,v);
  if(direction ==  5) motor(v/2,-v,-v/2,v);
  if(direction ==  6) motor(0,-v,0,v);
  if(direction ==  7) motor(-v/2,-v,v/2,v);
  if(direction ==  8) motor(-v,-v,v,v);
  if(direction ==  9) motor(-v,-v/2,v,v/2);
  if(direction == 10) motor(-v,0,v,0);
  if(direction == 11) motor(-v,v/2,v,-v/2);
  if(direction == 12) motor(-v,v,v,-v);
  if(direction == 13) motor(-v/2,v,v/2,-v);
  if(direction == 14) motor(0,v,0,-v);
  if(direction == 15) motor(v/2,v,-v/2,-v);
}
void out_sharp(){
  if(shr > Wall_Distance){
    while(direction_ball < 8 && direction_ball > 0 && is_ball){
      sensors();
      print_all();
      v = 130;
      if(shr > Wall_Distance + 300) move(12);
      else                          stop();
    }
  }
  if(shl > Wall_Distance){
    while(direction_ball > 8 && is_ball){
      sensors();
      print_all();
      v = 130;
      if(shl > Wall_Distance + 300) move(4);
      else                          stop();
    }
  }
}
void stop(){
  motor(0, 0, 0, 0);
}
void movexy(int x, int y){
  motor((x+y)/2, (y-x)/2, (-x-y)/2, (x-y)/2);
}
void moveangle(int angle){
  movexy(v * sin(angle * PI/180), v * cos(angle * PI/180));
}