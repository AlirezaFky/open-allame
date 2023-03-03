int buff[8];
int counter, GY;
int blocks;
int angle_ball, direction_ball, distance_ball , x_ball , y_ball;
int shr, shl, shb, dif;
int v=130;
bool is_ball;
int a = 0;
#define Wall_Distance 2600
#define x_robot 110 //117
#define y_robot 175 //145

void setup() {
  inti();
}

void loop() {
  sensors();
  print_all();
  out_sharp();
  // moveangle(a);
  // delay(10);
  // a++;
  // a %= 360;
  if(is_ball){
    if(distance_ball > 70) {
      v = 250;
      move(direction_ball);
    }  
    else{
      v = 130;
      if(direction_ball==0)        move(direction_ball);
      else if(direction_ball==1)   move(direction_ball + 1);
      else if(direction_ball==15)  move(direction_ball - 1);
      else if(direction_ball <=3)  move(direction_ball + 2);
      else if(direction_ball <=8)  move(direction_ball + 3);
      else if(direction_ball <=13) move(direction_ball - 3);
      else                         move(direction_ball - 2);
    }
  }

  else {
    v = 150;
    if(shb < 1000)    motor(-v + dif, -v - dif, v - dif, v + dif);
    else              motor(0, 0, 0, 0);
  }
}
