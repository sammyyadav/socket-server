#include <Encoder.h>
#define USE_USBCON
#include <ros.h>
#include <std_msgs/Float64.h>
ros::NodeHandle nh;

Encoder knobLeft(5, 6);
Encoder knobRight(7, 8);

//Motor1
double kp_l =2.7;
double ki_l = 00;
double kd_l = 0.01;
//Motor 27
double kp_r =2.7;
double ki_r = 0;
double kd_r = 0;

unsigned long Time=0;
double error_l, error_r;
double lastError_l, lastError_r;
double out_l, out_r, pid_l, pid_r;
double setpoint_l = 0, setpoint_r = 0;
double sumError_l=0, sumError_r=0, diffError_l=0, diffError_r=0;

 double speedLeft, speedRight;
double newLeft, newRight, oldLeft, oldRight;
double rpmL=0,rpmR=0,ldif,rdif;
int start;


void rpml_Cb( const std_msgs::Float64& rpml_){
  setpoint_l =rpml_.data;
}

ros::Subscriber<std_msgs::Float64> sub_l("rpml_", &rpml_Cb );

void rpmr_Cb( const std_msgs::Float64& rpmr_){
 setpoint_r = rpmr_.data;
}

ros::Subscriber<std_msgs::Float64> sub_r("rpmr_", &rpmr_Cb );

void setup()
{
  nh.initNode();
  nh.subscribe(sub_l);
  nh.subscribe(sub_r);

  Serial.begin(9600);
  Serial1.begin(9600);
  Time = millis();
  
 // _control(100,100);
start = millis();
}

void _control(double l, double r)
{     double command;
      if (l >= 0) {
        l=(l*63)/100;
        command = 0 + l;
      }
      else if (l < 0) {
        l=(-l*63)/100;
        command = 0x40 + l;
      }
      Serial1.write(command);
      //Serial.print(command);
      //Serial.print("\t ");

      if (r >= 0) {
        r=(r*63)/100;
        command = 0xC0 + r;
      }
      else if (r < 0) {
        r=(-r*63)/100;
        command = 0x80 + r;
      }
      Serial1.write(command);
      //Serial.println(command);
}

void pid(double rpmL, double rpmR)
{
  error_l = setpoint_l-rpmL;
  sumError_l += error_l;
  diffError_l = error_l - lastError_l;
  pid_l =  kp_l*error_l + ki_l*sumError_l + kd_l*diffError_l;
  lastError_l = error_l;

  error_r = setpoint_r-rpmR;
  sumError_r += error_r;
  diffError_r = error_r - lastError_r;
  pid_r =  kp_r*error_r + ki_r*sumError_r + kd_r*diffError_r;
  lastError_r = error_r;

  speedLeft = speedLeft + pid_l;
  speedRight = speedRight + pid_r;
  if (speedLeft >= 100){
    speedLeft = 100;
  }
    if (speedLeft <= -100){
    speedLeft = -100;
  }
  if (speedRight >=100){
    speedRight = 100;
  }
  if (speedRight <=-100 ){
    speedRight = -100;
  }
  _control(speedLeft, speedRight);

  //Serial.print(speedLeft);
  //Serial.print("\t");
  //Serial.println(speedRight);
}

double positionLeft  = -999;
double positionRight = -999;

void loop()
{
  nh.spinOnce();
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) 
  {
    positionLeft = newLeft;
    positionRight = newRight;
  }
  
  if(millis()-Time >1000)
  { 
    ldif = newLeft - oldLeft;
    rdif = newRight - oldRight;
    oldLeft  = newLeft;
    oldRight = newRight;
    
    rpmL = (-ldif *60/154000);
    rpmR = (-rdif *60/154000);
  
    if (millis()-start >3000){
    Serial.print(rpmL);
      Serial.print("\t");
      //Serial.print(rpmR);
      Serial.println("");}
    pid(rpmL,rpmR);
    Time = millis();   
    
  }
}
