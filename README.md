
[![Arduino CI](https://github.com/RobTillaart/AD568X/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/AD568X/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/AD568X/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/AD568X/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/AD568X/actions/workflows/jsoncheck.yml)
[![GitHub issues](https://img.shields.io/github/issues/RobTillaart/AD568X.svg)](https://github.com/RobTillaart/AD568X/issues)

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/AD568X/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/AD568X.svg?maxAge=3600)](https://github.com/RobTillaart/AD568X/releases)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/robtillaart/library/AD568X.svg)](https://registry.platformio.org/libraries/robtillaart/AD568X)


# AD568X

**Experimental**

Arduino library for the AD568X series digital analog convertor.

- SPI interface  (AD569X series is I2C)

Note: not yet tested, 
TODO buy hardware.
TODO documentation.


## Description


|  Type      |  bits   |  supported  |  Notes  |
|:----------:|:-------:|:-----------:|:--------|
|  AD5680    |    18   |     N       | need separate class ?
|  AD5681R   |    12   |     Y       |
|  AD5682R   |    14   |     Y       |
|  AD5683    |    16   |     Y       |
|  AD5683R   |    16   |     Y       |


Feedback, issues, improvements are welcome. 
Please file an issue on GitHub.


## Links

- https://github.com/RobTillaart/AD56x8 (multi channel)
- https://github.com/RobTillaart/MCP_DAC (SPI interface)


## Interface

```cpp
#include "AD568X.h"
```

### Base class

Should not be used to instantiate a device as the derived types have 
set the correct number of bits. 

- **AD568X(uint8_t slaveSelect)** constructor base class, sets HW SPI.
Sets internal values to zero.
- **AD568X(uint8_t spiData, uint8_t spiClock, uint8_t slaveSelect)** constructor, 
sets SW SPI.
Sets internal values to zero.
- **begin()** initializes the SPI and sets internal state.


#### Derived classes (preferred use)

The parameters for the specific constructors are identical to the base class.
One should use these!

- **AD5680(..)** constructor,  18 bit.  NOT IMPLEMENTED.
- **AD5681R(..)** constructor, 12 bit.
- **AD5682R(..)** constructor, 14 bit.
- **AD5683R(..)** constructor, 16 bit.
- **AD5683(..)** constructor,  16 bit.


### Set DAC

- **bool setValue(uint16_t value)** set value to the output immediately, 
effectively a prepare + update in one call.
Returns false if value out of range.
- **uint16_t getValue()** returns set OR prepared value.
At power up the DAC's will be reset to 0 Volt.
- **bool setPercentage(float percentage)** idem.
- **float getPercentage()** idem.
- **bool prepareValue(uint16_t value)** prepares the value.
Returns false if value out of range.
- **bool updateValue()** writes the prepared value to ADC.
Returns false if value out of range.


### Control Register

TODO

|  mode  |  define            |
|:------:|:-------------------|
|  0x00  | AD568X_PWR_NORMAL  |
|  0x01  | AD568X_PWR_1K      |
|  0x02  | AD568X_PWR_100K    |
|  0x03  | AD568X_PWR_TRI     |




#### SPI 

- **void setSPIspeed(uint32_t speed)** sets SPI clock in **Hz**,
please read datasheet of the ADC first to get optimal speed.
- **uint32_t getSPIspeed()** gets current speed in **Hz**.
- **bool usesHWSPI()** returns true if HW SPI is used.


#### SPI ESP32 specific

("inherited" from MPC_DAC library)

- **void selectHSPI()** in case hardware SPI, the ESP32 has two options HSPI and VSPI.
- **void selectVSPI()** see above.
- **bool usesHSPI()** returns true if HSPI is used.
- **bool usesVSPI()** returns true if VSPI is used.

The **selectVSPI()** or the **selectHSPI()** needs to be called 
BEFORE the **begin()** function.

(experimental)
- **void setGPIOpins(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t select)** 
overrule GPIO pins of ESP32 for hardware SPI. Needs to be called AFTER the **begin()** function.

Note: earlier experiments shows that on a ESP32 SW-SPI is equally fast as 
HW-SPI and in fact a bit more stable. 
The SW pulses are a bit slower than the HW pulses and therefore more square. 
The HW-SPI has some overhead SW-SPI hasn't. 




## Future

#### Must

- improve documentation
- get test hardware
- test the library
- support for LDAC pin
  - void **setLDACPin(uint8_t pin)**
  - void **triggerLDAC()**
- implement better control register code
  - RESET
  - Power Down modes
  - GAIN bit
  - Daisy Chain support


#### Should

- write examples
  - performance
- investigate derived class for AD5680
- check TODO in code


#### Could

- write unit test


#### Wont


## Support

If you appreciate my libraries, you can support the development and maintenance.
Improve the quality of the libraries by providing issues and Pull Requests, or
donate through PayPal or GitHub sponsors.

Thank you,



