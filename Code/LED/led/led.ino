#include <FastLED.h>       // 此示例程序需要使用FastLED库
#define NUM_LEDS 16             // LED灯珠数量
#define DATA_PIN 10              // Arduino输出控制信号引脚
#define LED_TYPE WS2812B         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
 
uint8_t max_bright = 128;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
String info;
char led_com;
char val;
CRGB leds[NUM_LEDS];            // 建立光带leds
static int serial_count = 0;
static int serial_delay, serial_prevdelay;
static unsigned long serial_timer;

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
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
  Serial.setTimeout(0);
  serial_prevdelay = serial_delay = 1000;
  serial_timer = millis();
  serial_count = 0;
}

void setup() { 
  
  gsled_init();
  //LEDS.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);  // 初始化光带 
  //FastLED.setBrightness(max_bright);// 设置光带亮度 
}

void loop() {

  led_com_tasklet();
  gsled_commit();
  /*
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Green; FastLED.show(); delay(30);
    leds[i] = CRGB::Black; FastLED.show(); delay(30);
  }*/
   /*
  if(Serial.available() > 0)
  {
    if(Serial.read() == 0xAA)
    {
      if(Serial.read() == 0xAA)
      {
        for(int i = 0; i < 32; i++)
        {
          char B = Serial.read();
          char R = Serial.read();
          char G = Serial.read();
          leds[i] = CRGB(G,R,B);
        }
        FastLED.show();
      }
    }
  }*/
}
