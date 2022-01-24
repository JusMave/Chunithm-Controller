/*!
 * @file Adafruit_MPR121.cpp
 *
 *  @mainpage Adafruit MPR121 arduino driver
 *
 *  @section intro_sec简介
 *
 *  这是用于MPR121 I2C 12通道电容传感器的库
 *
 *  专为与Adafruit的MPR121传感器配合使用而设计
 *  ----> https://www.adafruit.com/products/1982
 *
 *  这些传感器使用I2C进行通信，需要2个以上的引脚进行接口
 *  
 *
 *  Adafruit投入时间和资源来提供此开源代码，
 *  请通过购买支持Adafruit和开源硬件
 *  Adafruit的产品！
 *
 *  @section作者作者
 *
 *  由Limor Fried / Ladyada为Adafruit Industries撰写。
 *
 *  @section许可证许可证
 *
 *  BSD许可证，此处的所有文本都必须包含在任何重新分发中。
 */
#ifndef ADAFRUIT_MPR121_H
#define ADAFRUIT_MPR121_H

#include "Arduino.h"
#include <Wire.h>

// 默认的I2C地址
#define MPR121_I2CADDR_DEFAULT 0x5A ///< 默认的I2C地址
#define MPR121_TOUCH_THRESHOLD_DEFAULT 20 ///< 默认触摸阈值*******需要根据实际情况修改
#define MPR121_RELEASE_THRESHOLD_DEFAULT 10 ///< 默认relese阈值******需要根据实际情况修改
/*!
 *  设备寄存器图
 */
enum {
  MPR121_TOUCHSTATUS_L = 0x00,
  MPR121_TOUCHSTATUS_H = 0x01,
  MPR121_FILTDATA_0L = 0x04,
  MPR121_FILTDATA_0H = 0x05,
  MPR121_BASELINE_0 = 0x1E,
  MPR121_MHDR = 0x2B,
  MPR121_NHDR = 0x2C,
  MPR121_NCLR = 0x2D,
  MPR121_FDLR = 0x2E,
  MPR121_MHDF = 0x2F,
  MPR121_NHDF = 0x30,
  MPR121_NCLF = 0x31,
  MPR121_FDLF = 0x32,
  MPR121_NHDT = 0x33,
  MPR121_NCLT = 0x34,
  MPR121_FDLT = 0x35,

  MPR121_TOUCHTH_0 = 0x41,
  MPR121_RELEASETH_0 = 0x42,
  MPR121_DEBOUNCE = 0x5B,
  MPR121_CONFIG1 = 0x5C,
  MPR121_CONFIG2 = 0x5D,
  MPR121_CHARGECURR_0 = 0x5F,
  MPR121_CHARGETIME_1 = 0x6C,
  MPR121_ECR = 0x5E,
  MPR121_AUTOCONFIG0 = 0x7B,
  MPR121_AUTOCONFIG1 = 0x7C,
  MPR121_UPLIMIT = 0x7D,
  MPR121_LOWLIMIT = 0x7E,
  MPR121_TARGETLIMIT = 0x7F,

  MPR121_GPIODIR = 0x76,
  MPR121_GPIOEN = 0x77,
  MPR121_GPIOSET = 0x78,
  MPR121_GPIOCLR = 0x79,
  MPR121_GPIOTOGGLE = 0x7A,

  MPR121_SOFTRESET = 0x80,
};

//.. thru to 0x1C/0x1D

/*!
 *  @brief类，用于存储与MPR121交互的状态和函数
 *  接近电容触摸传感器控制器。
 */
class Adafruit_MPR121 {
public:
  // 硬件I2C
  Adafruit_MPR121();

  boolean begin(uint8_t i2caddr = MPR121_I2CADDR_DEFAULT,
                TwoWire *theWire = &Wire,
                uint8_t touchThreshold = MPR121_TOUCH_THRESHOLD_DEFAULT,
                uint8_t releaseThreshold = MPR121_RELEASE_THRESHOLD_DEFAULT);

  uint16_t filteredData(uint8_t t);
  uint16_t baselineData(uint8_t t);

  uint8_t readRegister8(uint8_t reg);
  uint16_t readRegister16(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  uint16_t touched(void);
  // 添加不推荐使用的属性，以便编译器显示警告
  void setThreshholds(uint8_t touch, uint8_t release)
      __attribute__((deprecated));
  void setThresholds(uint8_t touch, uint8_t release);

private:
  int8_t _i2caddr;
  TwoWire *_wire;
};

#endif
