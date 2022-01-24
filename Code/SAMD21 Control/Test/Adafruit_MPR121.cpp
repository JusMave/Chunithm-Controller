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

#include "Adafruit_MPR121.h"

// 取消使用自动配置的注释！
//#define AUTOCONFIG // use autoconfig (Yes it works pretty well!)

/*!
 *  @brief      默认构造函数
 */
Adafruit_MPR121::Adafruit_MPR121() {}

/*!
 *  @brief    在给定的I2C总线上启动MPR121对象。此功能将重置设备并写入默认设置。
 *            
 *  @param    i2caddr
 *            可以找到该设备的i2c地址。默认为0x5A。
 *  @param    *theWire
 *            电线对象
 *  @param    touchThreshold
 *            触摸检测阈值
 *  @param    releaseThreshold
 *            释放检测阈值
 *  @returns  成功则为true，否则为false
 */
boolean Adafruit_MPR121::begin(uint8_t i2caddr, TwoWire *theWire,
                               uint8_t touchThreshold,
                               uint8_t releaseThreshold) {

  _i2caddr = i2caddr;
  _wire = theWire;

  _wire->begin();

  // 软复位
  writeRegister(MPR121_SOFTRESET, 0x63);
  delay(1);
  for (uint8_t i = 0; i < 0x7F; i++) {
    //  Serial.print("$"); Serial.print(i, HEX);
    //  Serial.print(": 0x"); Serial.println(readRegister8(i));
  }

  writeRegister(MPR121_ECR, 0x0C);

  uint8_t c = readRegister8(MPR121_CONFIG2);

  if (c != 0x24)
    return false;

  setThresholds(touchThreshold, releaseThreshold);
  writeRegister(MPR121_MHDR, 0x01);
  writeRegister(MPR121_NHDR, 0x01);
  writeRegister(MPR121_NCLR, 0x00);
  writeRegister(MPR121_FDLR, 0x00);

  writeRegister(MPR121_MHDF, 0x01);
  writeRegister(MPR121_NHDF, 0x01);
  writeRegister(MPR121_NCLF, 0xFF);
  writeRegister(MPR121_FDLF, 0x02);

  writeRegister(MPR121_NHDT, 0x00);
  writeRegister(MPR121_NCLT, 0x00);
  writeRegister(MPR121_FDLT, 0x00);

  writeRegister(MPR121_DEBOUNCE, 0);
  writeRegister(MPR121_CONFIG1, 0x10); // 默认情况下，16uA充电电流
  writeRegister(MPR121_CONFIG2, 0x02); // 0.5uS编码，周期为1ms

#ifdef AUTOCONFIG
  writeRegister(MPR121_AUTOCONFIG0, 0x0B);

  // Vdd = 3.3V的正确值
  writeRegister(MPR121_UPLIMIT, 0x9C);     // ((Vdd - 0.7)/Vdd) * 256
  writeRegister(MPR121_TARGETLIMIT, 0x8C); // UPLIMIT * 0.9
  writeRegister(MPR121_LOWLIMIT, 0x65);    // UPLIMIT * 0.65
#endif

  // 启用X电极并启动MPR121
  byte ECR_SETTING =
      B10000000 + 12; // 5位用于基线跟踪和接近禁用+ X
                      // 运行的电极数量（12）
  writeRegister(MPR121_ECR, ECR_SETTING); // 从以上ECR设置开始

  return true;
}

/*!
 *  @brief      已淘汰使用 Adafruit_MPR121::setThresholds(uint8_t touch,
 *              uint8_t release) 代替。
 *  @param      touch
 *              see Adafruit_MPR121::setThresholds(uint8_t touch, uint8_t
 * *release)
 *  @param      release
 *              see Adafruit_MPR121::setThresholds(uint8_t touch, *uint8_t
 * release)
 */
void Adafruit_MPR121::setThreshholds(uint8_t touch, uint8_t release) {
  setThresholds(touch, release);
}

/*!
 *  @brief      将设备上所有13个通道的
 *              触摸和释放阈值设置为传递的值。阈值定义为与基
 * 线值的偏差值，因此即使基线值发生变化，阈值也保持恒定。通常，触摸阈
 * 值比释放阈值稍大一些，以消除触摸的反跳和滞后现象。对于典型的触摸应用，该值可以在例如0x05
 * 〜0x30的范围内。阈值的设置取决于实际应用。有关操作的详细信息以及如何设置阈值，请参考应用
 * 笔记AN3892和MPR121设计指南。
 *  @param      touch
 *              触摸阈值，范围从0到255。
 *  @param      release
 *              释放阈值从0到255。
 */
void Adafruit_MPR121::setThresholds(uint8_t touch, uint8_t release) {
  // 首先停止传感器进行更改
  writeRegister(MPR121_ECR, 0x00);
  // set all thresholds (the same)
  for (uint8_t i = 0; i < 12; i++) {
    writeRegister(MPR121_TOUCHTH_0 + 2 * i, touch);
    writeRegister(MPR121_RELEASETH_0 + 2 * i, release);
  }
  // 再次打开传感器
  writeRegister(MPR121_ECR, 0x8F);
}

/*!
 *  @brief      从通道t读取过滤的数据。 
 *              ADC原始数据输出经过3级
 * 数字滤波，以滤除遇到的
 * 高频和低频噪声。有关此过滤的详细信息，请参见器件数据手册第6页。
 *  @param      t
 *              阅读渠道
 *  @returns    过滤后的读数为10位无符号值
 */
uint16_t Adafruit_MPR121::filteredData(uint8_t t) {
  if (t > 12)
    return 0;
  return readRegister16(MPR121_FILTDATA_0L + t * 2);
}

/*!
 *  @brief      读取通道的基线值。第三级过滤
 *              结果内部为10位，但仅可读高8位
 * 但只能从寄存器0x1E〜0x2A读取高8位作为每个通道的基准值输出。
 *  @param      t
 *              要阅读的频道。
 *  @returns    读取的基准数据
 */
uint16_t Adafruit_MPR121::baselineData(uint8_t t) {
  if (t > 12)
    return 0;
  uint16_t bl = readRegister8(MPR121_BASELINE_0 + t);
  return (bl << 2);
}

/**
 *  @brief      读取所有13个通道的触摸状态作为12中的位值位整数。
 *  @returns    一个12位整数，每个位对应于传感器的触摸状态。例如，如果设置了位0，则当前认为设备的通道0被触摸。
 */
uint16_t Adafruit_MPR121::touched(void) {
  uint16_t t = readRegister16(MPR121_TOUCHSTATUS_L);
  return t & 0x0FFF;
}

/*!
 *  @brief      读取8位设备寄存器的内容。
 *  @param      reg读取的寄存器地址
 *  @returns    读取的8位值。
 */
uint8_t Adafruit_MPR121::readRegister8(uint8_t reg) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(_i2caddr, 1);
  if (_wire->available() < 1)
    return 0;
  return (_wire->read());
}

/*!
 *  @brief      读取16位设备寄存器的内容。
 *  @param      reg读取的寄存器地址
 *  @returns    读取的16位值。
 */
uint16_t Adafruit_MPR121::readRegister16(uint8_t reg) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(_i2caddr, 2);
  if (_wire->available() < 2)
    return 0;
  uint16_t v = _wire->read();
  v |= ((uint16_t)_wire->read()) << 8;
  return v;
}

/*!
    @brief  将8位写入指定的目标寄存器
    @param  reg写入的寄存器地址
    @param  重视要写的价值
*/
void Adafruit_MPR121::writeRegister(uint8_t reg, uint8_t value) {
  // 必须将MPR121置于停止模式才能写入大多数寄存器
  bool stop_required = true;
  uint8_t ECR = readRegister8(
      MPR121_ECR); // 首先获取MPR121_ECR寄存器的当前设置值
  if (reg == MPR121_ECR || (0x73 <= reg && reg <= 0x7A)) {
    stop_required = false;
  }
  if (stop_required) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(MPR121_ECR);
    _wire->write(0x00); // 清除该寄存器以设置停止方式
    _wire->endTransmission();
  }

  _wire->beginTransmission(_i2caddr);
  _wire->write((uint8_t)reg);
  _wire->write((uint8_t)(value));
  _wire->endTransmission();

  if (stop_required) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(MPR121_ECR);
    _wire->write(ECR); // 写回先前设置的ECR设置
    _wire->endTransmission();
  }
}
