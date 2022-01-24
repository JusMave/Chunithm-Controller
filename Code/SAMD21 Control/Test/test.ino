/*****************************
  适用于mpr121触摸按键的代码
  代码已经经过测试可以使用
  加了RGB
  更换了键盘库
  需要修改的地方是uint16_t KeyBoard[50]这一项中对应的按键与air的按键
  还有Adafruit_MPR121.h文件中的触摸阈值与release阈值
  不同的海拔，湿度，温度环境下，不同面积的金属箔和金属箔隔着触碰使用的阈值都不一样
  请逐一调低/高数值进行测试
  我使用的触摸阈值是3，release阈值是1
  Adafruit编写库时使用的触摸阈值是12，release是6
  库非本人编写，我只是对库的注释进行了翻译，请支持正版，购买adafruit销售的mpr121
  特殊按键注释
  NKROKeyboard
  KEY_LEFT_BRACE,         // [
  KEY_RIGHT_BRACE,        // ]
  KEY_PERIOD,            // .
  KEY_SLASH,            // /
  KEY_COMMA,            // ,
  KEY_QUOTE,            // '
  KEY_SEMICOLON,          // ;
**************************/
#include "Arduino.h"
#include <Wire.h> //I2C库
#include <FastLED.h>//led库
#include "HID-Project.h" //keyboard库
#include "Adafruit_MPR121.h" //MPR121库

#define NUM_LEDS 16//灯数量
#define led_PIN 10//灯的脚针
CRGB leds[NUM_LEDS];

#define MPRA 0x5A
#define MPRB 0x5B     //MPR121模块ADD脚默认地址/接地为 0x5A, 连接到 3.3V 为 0x5B       
#define MPRC 0x5C     // 连接到 SDA 为 0x5C ， 连接 SCL 为 0x5D
#define MPRD 0x5D
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define amount 3 //mpr121数量
const int air_key1 = 4;//定义air按键1,2,3,4,5,6
const int air_key2 = 5;
const int air_key3 = 6;
const int air_key4 = 7;
const int air_key5 = 8;
const int air_key6 = 9;
int airState = 0;


Adafruit_MPR121 capA = Adafruit_MPR121();  //创建对象
Adafruit_MPR121 capB = Adafruit_MPR121();
Adafruit_MPR121 capC = Adafruit_MPR121();
Adafruit_MPR121 capD = Adafruit_MPR121();


uint16_t lasttouchedA = 0; //创建相应的状态变量  lasttouchedA是上次的状态，currtouchedA是当前状态，两者用于检测引脚状态是否又变动。B，C，D与此相同。
uint16_t currtouchedA = 0;
uint16_t lasttouchedB = 0;
uint16_t currtouchedB = 0;
uint16_t lasttouchedC = 0;
uint16_t currtouchedC = 0;
uint16_t lasttouchedD = 0;
uint16_t currtouchedD = 0;

uint16_t Touched[50];  //变更为按下引脚的记录数组

uint16_t Released[50]; //变更为松开引脚的记录数组

uint16_t KeyBoard[50] = {'&', '1', 'q', '2', 'w', '3', 'e', '4', 'r', '5', 't', '6', 'y', '7', 'u', '8', 'i'};//第一个是无效的按键，不必修改，从第二个开始
//把脚针改为键盘按键,目前对应的是从按键的左边开始到右边分别为A0脚针到B3脚针

void setup() {
  
   while (!SerialUSB);//启动串口监视器时模块开始运行，完成测试工作后把这行删掉
   {
    SerialUSB.begin(9600);
    SerialUSB.println(" MPR121测试 ");
    MPR121Check();//检测I2C上的设备是否在线，切勿删掉此函数，否则Pro micro无法正常工作
   }
   Keyboard.begin(); //Keyboard库初始化
   Keyboard.releaseAll(); //所有键盘按键松开
   //NKROKeyboard.begin(); //NKROKeyboard库初始化
   //NKROKeyboard.releaseAll(); //所有键盘按键松开
   /*
  pinMode(air_key4, INPUT_PULLUP);//air按键脚针4,5,6,7,8,9
  pinMode(air_key1, INPUT_PULLUP);
  pinMode(air_key2, INPUT_PULLUP);
  pinMode(air_key3, INPUT_PULLUP);
  pinMode(air_key5, INPUT_PULLUP);
  pinMode(air_key6, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, led_PIN>(leds, NUM_LEDS);//设置RGB灯
  */
}

void MPR121Check() //MPR121初始化函数，检测是否在线
{
  if (amount >= 1)
  {
    if (!capA.begin(MPRA))
    {
      SerialUSB.println("MPRA not found, check wiring?");
      delay(1);
    }
    else
    {
      SerialUSB.println("MPRA found!");
    }
  }
  if (amount >= 2)
  {
    if (!capB.begin(MPRB))
    {
      SerialUSB.println("MPRB not found, check wiring?");
      delay(1);
    }
    else
    {
      SerialUSB.println("MPRB found!");
    }
  }
  if (amount >= 3)
  {
    if (!capC.begin(MPRC))
    {
      SerialUSB.println("MPRC not found, check wiring?");
      delay(1);
    }
    else
    {
      SerialUSB.println("MPRC found!");
    }
  }
  if (amount >= 4)
  {
    if (!capD.begin(MPRD))
    {
      SerialUSB.println("MPRD not found, check wiring?");
      delay(1);
    }
    else
    {
      SerialUSB.println("MPRD found!");
    }
  }
}

void TandRcheck()  //检测MPR121上的引脚变动情况，采用混合扫描
{
  uint8_t n = 0;
  uint8_t n2 = 0;
  uint8_t i = 0;
  if (amount >= 1)//地址为0x5A的mpr121
  {
    //SerialUSB.print("BBBB\n");
    currtouchedA = capA.touched();
    for ( i = 0; i < 12; i++) {

      if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) ) {
        SerialUSB.print(i + 1); SerialUSB.println(" touched");//串口打印哪一个按键被触摸
        
        Touched[n] = i + 1;
        Keyboard.press('1');
        //leds[i] = CRGB(0, 255, 255);//(红色的亮度，绿，蓝）//数值请从0-255中选择
        //FastLED.show();
        //SerialUSB.print("XXXXXXXXXX");
        n++;
        
      }

      if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) ) {
        SerialUSB.print(i + 1); SerialUSB.println(" released");//串口打印哪一个按键松开
        
        Released[n2] = i + 1;
        //SerialUSB.print("AAAAAA");
        Keyboard.release('1');
        //NKROKeyboard.release(KeyBoard[Released[n2]]);
        //leds[i] = CRGB(130, 255, 0);
        //FastLED.show();
        
        n2++;
        
      }
    }
    //SerialUSB.print("CCCC\n");
    lasttouchedA = currtouchedA;
    
  }
/*
  if (amount >= 2)//地址为0x5B的mpr121
  {
    currtouchedB = capB.touched();
    for ( i = 0; i < 12; i++) {

      if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) ) {
        SerialUSB.print(i + 12 + 1); SerialUSB.println(" touched");
        Touched[n] = i + 12 + 1;
        NKROKeyboard.press(KeyBoard[Touched[n]]);
        leds[i + 12] = CRGB(0, 255, 255);
        FastLED.show();
        
        n++;
      }
      if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) ) {
        SerialUSB.print(i + 12 + 1); SerialUSB.println(" released");
        Released[n2] = i + 12 + 1;
        NKROKeyboard.release(KeyBoard[Released[n2]]);
        leds[i + 12] = CRGB(130, 255, 0);
        FastLED.show();
        
        n2++;
      }
    }

    lasttouchedB = currtouchedB;
  }

  if (amount >= 3)//地址为0x5C的mpr121
  {
    currtouchedC = capC.touched();
    for ( i = 0; i < 12; i++) {

      if ((currtouchedC & _BV(i)) && !(lasttouchedC & _BV(i)) ) {
        SerialUSB.print(i + 12 + 12 + 1); SerialUSB.println(" touched");
        Touched[n] = i + 12 + 12 + 1;
        NKROKeyboard.press(KeyBoard[Touched[n]]);
        
        n++;
      }
      if (!(currtouchedC & _BV(i)) && (lasttouchedC & _BV(i)) ) {
        SerialUSB.print(i + 12 + 12 + 1); SerialUSB.println(" released");
        Released[n2] = i + 1;
        NKROKeyboard.release(KeyBoard[Released[n2]]);

        n2++;
      }
    }

    lasttouchedC = currtouchedC;
  }

  if (amount >= 4)//地址为0x5D的mpr121
  {
    currtouchedD = capD.touched();
    for (i = 0; i < 12; i++) {

      if ((currtouchedD & _BV(i)) && !(lasttouchedD & _BV(i)) ) {
        SerialUSB.print(i + 12 + 12 + 12 + 1); SerialUSB.println(" touched");
        Touched[n] = i + 12 + 12 + 12 + 1;
        NKROKeyboard.press(KeyBoard[Touched[n]]);

        n++;
      }
      if (!(currtouchedC & _BV(i)) && (lasttouchedC & _BV(i)) ) {
        SerialUSB.print(i + 12 + 12 + 12 + 1); SerialUSB.println(" released");
        Released[n2] = i + 12 + 12 + 12 + 1;
        NKROKeyboard.release(KeyBoard[Released[n2]]);
        
        n2++;
      }
    }

    lasttouchedD = currtouchedD;
  }
  */
}

void loop() 
{

   TandRcheck();

/*
  airState = digitalRead(air_key1); //按键1
  if (airState == LOW) {
    NKROKeyboard.press(KEY_RIGHT_BRACE);
  }
  else {
    NKROKeyboard.release(KEY_RIGHT_BRACE);
  }
  airState = digitalRead(air_key2);//按键2
  if (airState == LOW) {
    NKROKeyboard.press(KEY_PERIOD);
  }
  else {
    NKROKeyboard.release(KEY_PERIOD);
  }
  airState = digitalRead(air_key3); //按键3
  if (airState == LOW) {
    NKROKeyboard.press(KEY_SEMICOLON);
  }
  else {
    NKROKeyboard.release(KEY_SEMICOLON);
  }
  airState = digitalRead(air_key4);//按键4
  if (airState == LOW) {
    NKROKeyboard.press(0);
  }
  else {
    NKROKeyboard.release(0);
  }
  airState = digitalRead(air_key5); //按键5
  if (airState == LOW) {
    NKROKeyboard.press(KEY_QUOTE);
  }
  else {
    NKROKeyboard.release(KEY_QUOTE);
  }
  airState = digitalRead(air_key6);//按键6
  if (airState == LOW) {
    NKROKeyboard.press(KEY_SLASH);
  }
  else {
    NKROKeyboard.release(KEY_SLASH);
  }
  */
}
