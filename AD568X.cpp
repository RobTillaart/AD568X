//
//    FILE: AD568X.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2023-09-18
// PURPOSE: Arduino library for AD568X series Digital Analog Convertor.


#include "AD568X.h"

//  not all "commands" implemented

#define AD568X_REG_NOP            0x00
#define AD568X_REG_WRITE          0x10
#define AD568X_REG_UPDATE         0x20
#define AD568X_REG_WRITE_UPDATE   0x30
#define AD568X_REG_CONTROL        0x40


AD568X::AD568X(uint8_t slaveSelect)
{
  _hwSPI  = true;
  _select = slaveSelect;
  _value  = 0;
}


AD568X::AD568X(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
{
  _hwSPI   = false;
  _dataOut = spiData;
  _clock   = spiClock;
  _select  = slaveSelect;
  _value   = 0;
}


//  initializes the SPI
//  and sets internal state
void AD568X::begin()
{
  pinMode(_select, OUTPUT);
  digitalWrite(_select, HIGH);

  _spi_settings = SPISettings(_SPIspeed, MSBFIRST, SPI_MODE1);

  if(_hwSPI)
  {
    #if defined(ESP32)
    if (_useHSPI)      //  HSPI
    {
      mySPI = new SPIClass(HSPI);
      mySPI->end();
      mySPI->begin(14, 12, 13, _select);   //  CLK=14  MISO=12  MOSI=13
    }
    else               //  VSPI
    {
      mySPI = new SPIClass(VSPI);
      mySPI->end();
      mySPI->begin(18, 19, 23, _select);   //  CLK=18  MISO=19  MOSI=23
    }
    #else              //  generic hardware SPI
    mySPI = &SPI;
    mySPI->end();
    mySPI->begin();
    #endif
    delay(1);
  }
  else                 //  software SPI
  {
    pinMode(_dataOut, OUTPUT);
    pinMode(_clock, OUTPUT);
    digitalWrite(_dataOut, LOW);
    digitalWrite(_clock, LOW);
  }
}


uint8_t AD568X::getType()
{
  return _type;
}


//  value = 0..65535 (16 bit), 16383 (14 bit), 4095 (12 bit) depending on type)
bool AD568X::setValue(uint16_t value)
{
  if ((_type == 12) && (value > 4095))  return false;
  if ((_type == 14) && (value > 16383)) return false;
  _value = value;
  updateDevice(AD568X_REG_WRITE_UPDATE, value);
  return true;
}


//  returns 0..65535 (16 bit), 16383 (14 bit), 4095 (12 bit) depending on type)
uint16_t AD568X::getValue()
{
  return _value;
}


bool AD568X::setPercentage(float percentage)
{
  uint16_t value = 0;
  if ((percentage < 0) || (percentage > 100)) return false;
  if      (_type == 16) value = round(655.35 * percentage);
  else if (_type == 14) value = round(163.83 * percentage);
  else /* type = 12 */  value = round( 40.95 * percentage);
  return setValue(value);
}


float AD568X::getPercentage()
{
  float value = getValue();
  if (value > 0)
  {
    if (_type == 16) return value * ( 1.0 / 655.35);
    if (_type == 14) return value * ( 1.0 / 163.83);
    if (_type == 12) return value * ( 1.0 /  40.95);
  }
  return 0;
}


bool AD568X::prepareValue(uint16_t value)
{
  if ((_type == 12) && (value > 4095))  return false;
  if ((_type == 14) && (value > 16383)) return false;
  _value = value;
  updateDevice(AD568X_REG_WRITE, value);
  return true;
}


bool AD568X::updateValue()
{
  updateDevice(AD568X_REG_UPDATE, 0);
  return true;
}


bool AD568X::setControlRegister(uint16_t value)
{
  updateDevice(AD568X_REG_CONTROL, value);
  return true;
}


//
//  SPI
//
void AD568X::setSPIspeed(uint32_t speed)
{
  _SPIspeed = speed;
  _spi_settings = SPISettings(_SPIspeed, MSBFIRST, SPI_MODE1);
};


uint32_t AD568X::getSPIspeed()
{
  return _SPIspeed;
};


bool AD568X::usesHWSPI()
{
  return _hwSPI;
}


//  ESP32 specific
#if defined(ESP32)

void AD568X::selectHSPI()
{
  _useHSPI = true;
}


void AD568X::selectVSPI()
{
  _useHSPI = false;
}


bool AD568X::usesHSPI()
{
  return _useHSPI;
}


bool AD568X::usesVSPI()
{
  return !_useHSPI;
}


void AD568X::setGPIOpins(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t select)
{
  _clock   = clk;
  _dataOut = mosi;
  _select  = select;
  pinMode(_select, OUTPUT);
  digitalWrite(_select, HIGH);

  mySPI->end();                            //  disable SPI
  mySPI->begin(clk, miso, mosi, select);   //  enable SPI
}

#endif


//////////////////////////////////////////////////////////////////
//
//  PRIVATE
//

void AD568X::updateDevice(uint8_t cmd, uint16_t value)
{
  //  to be optimized - switch cmd etc
  uint8_t a = cmd | (value >> 12);
  uint8_t b = (value >> 4) & 0xFF;
  uint8_t c = (value << 4) & 0xF0;

  updateDevice(a, b, c);
}


void AD568X::updateDevice(uint8_t a, uint8_t b, uint8_t c)
{
  digitalWrite(_select, LOW);
  if (_hwSPI)
  {
    mySPI->beginTransaction(_spi_settings);
    mySPI->transfer(a);
    mySPI->transfer(b);
    mySPI->transfer(c);
    mySPI->endTransaction();
  }
  else //  Software SPI
  {
    swSPI_transfer(a);
    swSPI_transfer(b);
    swSPI_transfer(c);
  }
  digitalWrite(_select, HIGH);
}


//  simple one mode version
void AD568X::swSPI_transfer(uint8_t value)
{
  uint8_t clk = _clock;
  uint8_t dao = _dataOut;
  for (uint8_t mask = 0x80; mask; mask >>= 1)
  {
    digitalWrite(dao,(value & mask));
    digitalWrite(clk, HIGH);
    digitalWrite(clk, LOW);
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  DERIVED  AD5680
//
// AD5680::AD5680(uint8_t slaveSelect) : AD568X(slaveSelect)
// {
  // _type  = 18;
  // _value = 0;
// }

// AD5680::AD5680(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
                // : AD568X(spiData, spiClock, slaveSelect)
// {
  // _type  = 18;
  // _value = 0;
// }


/////////////////////////////////////////////////////////////////////////////
//
//  DERIVED  AD5681
//
AD5681R::AD5681R(uint8_t slaveSelect) : AD568X(slaveSelect)
{
  _type  = 12;
  _value = 0;
}

AD5681R::AD5681R(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
                : AD568X(spiData, spiClock, slaveSelect)
{
  _type = 12;
  _value = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  DERIVED  AD5682
//
AD5682R::AD5682R(uint8_t slaveSelect) : AD568X(slaveSelect)
{
  _type  = 14;
  _value = 0;
}

AD5682R::AD5682R(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
                : AD568X(spiData, spiClock, slaveSelect)
{
  _type  = 14;
  _value = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  DERIVED  AD5683
//
AD5683R::AD5683R(uint8_t slaveSelect) : AD568X(slaveSelect)
{
  _type  = 16;
  _value = 0;
}

AD5683R::AD5683R(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
                : AD568X(spiData, spiClock, slaveSelect)
{
  _type  = 16;
  _value = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  DERIVED  AD5683
//
AD5683::AD5683(uint8_t slaveSelect) : AD568X(slaveSelect)
{
  _type  = 16;
  _value = 0;
}

AD5683::AD5683(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)
                : AD568X(spiData, spiClock, slaveSelect)
{
  _type  = 16;
  _value = 0;
}


//  -- END OF FILE --

