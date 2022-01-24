
/***************************
  适用于mpr121触摸按键的代码
  代码已经经过测试可以使用
  如果遇到air开关是通光高电平，遮光低电平，仅需把HIGH改成LOW即可
  加了RGB
  更换了键盘库
  需要修改的地方是uint16_t KeyBoard[50]这一项中对应的按键与air的按键
  还有Adafruit_MPR121.h文件中的触摸阈值与release阈值
  不同的海拔，湿度，温度环境下，不同面积的金属箔和金属箔隔着触碰使用的阈值都不一样
  请逐一调低/高数值进行测试
  我使用的触摸阈值是3，release阈值是1
  Adafruit编写库时使用的触摸阈值是12，release是6yn
  库非本人编写，我只是对库的注释进行了翻译，请支持正版，购买adafruit销售的mpr121
  特殊按键注释
  NKROKeyboard
  KEY_LEFT_BRACE,         // [  0xDB
  KEY_RIGHT_BRACE,        // ]  0xDD
  KEY_PERIOD,            // .   0xBE
  KEY_SLASH,            // /    0xBF
  KEY_COMMA,            // ,    0xBC
  KEY_QUOTE,            // '    0xDE
 KEY_SEMICOLON,          // ;   0xBA
**************************/
#include <Wire.h> //I2C库
#include <FastLED.h>//led库
#define NUM_LEDS 16//灯数量
#define led_PIN 10//灯的脚针
CRGB leds[NUM_LEDS];
#include "HID-Project.h" //keyboard库
#include "Adafruit_MPR121.h" //MPR121库
#define MPRA 0x5A
#define MPRB 0x5B     //MPR121模块ADD脚默认地址/接地为 0x5A, 连接到 3.3V 为 0x5B       
#define MPRC 0x5C     // 连接到 SDA 为 0x5C ， 连接 SCL 为 0x5D
#define MPRD 0x5D

#define amount 4 //mpr121数量
const int air_key1 = 4;//定义air按键1,2,3,4,5,6
const int air_key2 = 5;
const int air_key3 = 6;
const int air_key4 = 7;
const int air_key5 = 8;
const int air_key6 = 9;
int airState = 0;

static int serial_count;
static int serial_delay, serial_prevdelay;
static unsigned long serial_timer;

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
/*
uint16_t KeyBoard[50] = {'&', 'i', '8', 'u', '7', 'y', '6', 't', '5', 'r', '4', 'e', '3', 'w', '2', 'q', '1'};//第一个是无效的按键，不必修改，从第二个开始
uint16_t KeyBoarddown[50] = {'&', ',', 'k', 'm', 'j', 'n', 'h', 'b', 'g', 'v', 'f', 'c', 'd', 'x', 's', 'z', 'a'};
*/
uint16_t KeyBoard[50] = {'&', '1', 'q', '2', 'w', '3', 'e', '4', 'r', '5', 't', '6', 'y', '7', 'u', '8', 'i'};//第一个是无效的按键，不必修改，从第二个开始
uint16_t KeyBoarddown[50] = {'&', 'a', 'z', 's', 'x', 'd', 'c', 'f', 'v', 'g', 'b', 'h', 'n', 'j', 'm', 'k', ','};
//把脚针改为键盘按键,目前对应的是从按键的左边开始到右边分别为A0脚针到B3脚针

void led_com_tasklet() {
  
  if (Serial.available() >= 3) {
    leds[15 - serial_count].r = Serial.read();
    leds[15 - serial_count].g = Serial.read();
    leds[15 - serial_count].b = Serial.read();
    
    serial_count++;
    if (serial_count == 16) {
      FastLED.show();
      while(Serial.read() != -1);
      serial_delay = serial_prevdelay;
      Serial.write("A"); // ACK
      serial_count = 0;
    }
  }
  if (--serial_delay == 0) {
    while(Serial.read() != -1);
    Serial.write('A');
    int interval = millis() - serial_timer;
    if (interval > 500) {
      serial_prevdelay = serial_prevdelay * 0.9;
    } else {
      serial_prevdelay = serial_prevdelay * 1.1;
    }
    serial_delay = serial_prevdelay;
    serial_timer += interval;
  }
}

void gsled_commit() {
  FastLED.show();
}

void gsled_init() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = 0x000000;
  }
  FastLED.addLeds<NEOPIXEL, led_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
  Serial.setTimeout(0);
  serial_prevdelay = serial_delay = 1000;
  serial_timer = millis();
  serial_count = 0;
}


void setup() {
  gsled_init();
  
  MPR121Check();//检测I2C上的设备是否在线，切勿删掉此函数，否则Pro micro无法正常工作
  NKROKeyboard.begin(); //NKROKeyboard库初始化
  NKROKeyboard.releaseAll(); //所有键盘按键松开
  /*
  pinMode(air_key4, INPUT_PULLUP);//air按键脚针4,5,6,7,8,9
  pinMode(air_key1, INPUT_PULLUP);
  pinMode(air_key2, INPUT_PULLUP);
  pinMode(air_key3, INPUT_PULLUP);
  pinMode(air_key5, INPUT_PULLUP);
  pinMode(air_key6, INPUT_PULLUP);
  */
  //FastLED.addLeds<WS2812B, led_PIN>(leds, NUM_LEDS);//设置RGB灯
  
}

void MPR121Check() //MPR121初始化函数，检测是否在线
{
  if (amount >= 1)
  {
    if (!capA.begin(MPRA))
    {
      Serial.println("MPRA not found, check wiring?");
      delay(1);
    }
    else
    {
      Serial.println("MPRA found!");
    }
  }
  if (amount >= 2)
  {
    if (!capB.begin(MPRB))
    {
      Serial.println("MPRB not found, check wiring?");
      delay(1);
    }
    else
    {
      Serial.println("MPRB found!");
    }
  }
  if (amount >= 3)
  {
    if (!capC.begin(MPRC))
    {
      Serial.println("MPRC not found, check wiring?");
      delay(1);
    }
    else
    {
      Serial.println("MPRC found!");
    }
  }
  if (amount >= 4)
  {
    if (!capD.begin(MPRD))
    {
      Serial.println("MPRD not found, check wiring?");
      delay(1);
    }
    else
    {
      Serial.println("MPRD found!");
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
    currtouchedA = capA.touched();
    for ( i = 0; i < 8; i++) {

      if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) ) {
        if(i < 4)
        {
          Touched[n] = i + 1;
          NKROKeyboard.press(KeyBoarddown[Touched[n]]);
        }
        else
        {
          Touched[n] = i + 1 - 4;
          NKROKeyboard.press(KeyBoard[Touched[n]]);
        }
        //leds[i] = CRGB(0, 255, 255);//(红色的亮度，绿，蓝）//数值请从0-255中选择
        FastLED.show();
        n++;
      }

      if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) ) {
        
        
         if(i < 4)
         {
          Released[n2] = i + 1;
          NKROKeyboard.release(KeyBoarddown[Released[n2]]);
         }
        else
        {
          Released[n2] = i + 1 - 4;
          NKROKeyboard.release(KeyBoard[Released[n2]]);
        }
        //leds[i] = CRGB(130, 255, 0);
        FastLED.show();
        n2++;
      }
    }

    lasttouchedA = currtouchedA;
  }

  if (amount >= 2)//地址为0x5B的mpr121
  {
    currtouchedB = capB.touched();
    for ( i = 0; i < 8; i++) {

      if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) ) {
        
        if(i < 4)
        {
          Touched[n] = i + 4 + 1;
          NKROKeyboard.press(KeyBoarddown[Touched[n]]);
        }
        else
        {
          Touched[n] = i + 4 + 1 - 4;
          NKROKeyboard.press(KeyBoard[Touched[n]]);
        }
        //leds[i + 12] = CRGB(0, 255, 255);
        FastLED.show();
        n++;
      }
      if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) ) {
        
        if(i < 4)
        {
          Released[n2] = i + 4 + 1;
          NKROKeyboard.release(KeyBoarddown[Released[n2]]);
        }
        else
        {
          Released[n2] = i + 1 + 4 - 4;
          NKROKeyboard.release(KeyBoard[Released[n2]]);
        }
        //leds[i + 12] = CRGB(130, 255, 0);
        FastLED.show();
        n2++;
      }
    }

    lasttouchedB = currtouchedB;
  }

  if (amount >= 3)//地址为0x5C的mpr121
  {
    currtouchedC = capC.touched();
    for ( i = 0; i < 8; i++) {

      if ((currtouchedC & _BV(i)) && !(lasttouchedC & _BV(i)) ) {
        
        if(i < 4)
        {
          Touched[n] = i + 4 + 4 + 1;
          NKROKeyboard.press(KeyBoarddown[Touched[n]]);
        }
        else
        {
          Touched[n] = i + 1 + 4 + 4 - 4;
          NKROKeyboard.press(KeyBoard[Touched[n]]);
        }
        FastLED.show();
        n++;
      }
      if (!(currtouchedC & _BV(i)) && (lasttouchedC & _BV(i)) ) {
       
        if(i < 4)
        {
          Released[n2] = i + 4 + 4 + 1;
          NKROKeyboard.release(KeyBoarddown[Released[n2]]);
        }
        else
        {
          Released[n2] = i + 1 + 4 + 4 - 4;
          NKROKeyboard.release(KeyBoard[Released[n2]]);
        }
        FastLED.show();
        n2++;
      }
    }

    lasttouchedC = currtouchedC;
  }

  if (amount >= 4)//地址为0x5D的mpr121
  {
    currtouchedD = capD.touched();
    for ( i = 0; i < 8; i++) {

      if ((currtouchedD & _BV(i)) && !(lasttouchedD & _BV(i)) ) {
        
        if(i < 4)
        {
          Touched[n] = i + 4 + 4 + 4 + 1;
          NKROKeyboard.press(KeyBoarddown[Touched[n]]);
        }
        else
        {
          Touched[n] = i + 4 + 4 + 4 + 1 - 4;
          NKROKeyboard.press(KeyBoard[Touched[n]]);
        }
        FastLED.show();
        n++;
      }
      if (!(currtouchedD & _BV(i)) && (lasttouchedD & _BV(i)) ) {
       
        if(i < 4)
        {
          Released[n2] = i + 4 + 4 + 4 + 1;
          NKROKeyboard.release(KeyBoarddown[Released[n2]]);
        }
        else
        {
          Released[n2] = i + 4 + 4 + 4 + 1 - 4;
          NKROKeyboard.release(KeyBoard[Released[n2]]);
        }
        FastLED.show();
        n2++;
      }
    }

    lasttouchedD = currtouchedD;
  }
}




void loop() {

  led_com_tasklet();
  gsled_commit();

  TandRcheck();

  //char LED_com = "0";
  
  /*
  if(Serial.available() > 0)
  {
    LED_com = Serial.read();
    Serial.print(LED_com);
  }*/
  
  airState = digitalRead(air_key1); //按键1
  if (airState == LOW)
    NKROKeyboard.press(KEY_SLASH);
  else 
    NKROKeyboard.release(KEY_SLASH);
 
  airState = digitalRead(air_key2);//按键2
  if (airState == HIGH)
    NKROKeyboard.press(KEY_PERIOD);
  else
    NKROKeyboard.release(KEY_PERIOD);
  
  airState = digitalRead(air_key3); //按键3
  if (airState == HIGH)
    NKROKeyboard.press(KEY_QUOTE);
  else
    NKROKeyboard.release(KEY_QUOTE);
    
  airState = digitalRead(air_key4);//按键4
  if (airState == LOW)
    NKROKeyboard.press(KEY_SEMICOLON);
  else
    NKROKeyboard.release(KEY_SEMICOLON);
    
  airState = digitalRead(air_key5); //按键5
  if (airState == HIGH)
    NKROKeyboard.press(KEY_RIGHT_BRACE);
  else
    NKROKeyboard.release(KEY_RIGHT_BRACE);

  airState = digitalRead(air_key6);//按键6
  if (airState == HIGH)
    NKROKeyboard.press(KEY_LEFT_BRACE);
  else
    NKROKeyboard.release(KEY_LEFT_BRACE);
    
  gsled_commit();
}
