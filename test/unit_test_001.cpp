//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2023-09-18
// PURPOSE: unit tests for the AD568X DA convertor.
//          https://github.com/RobTillaart/AD568X
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)


#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "AD568X.h"



unittest_setup()
{
  fprintf(stderr, "AD568X_LIB_VERSION: %s\n", (char *) AD568X_LIB_VERSION);
}

unittest_teardown()
{
}


unittest(constant)
{
  assertEqual(0x00, AD568X_PWR_NORMAL    );
  assertEqual(0x01, AD568X_PWR_1K        );
  assertEqual(0x02, AD568X_PWR_100K      );
  assertEqual(0x03, AD568X_PWR_TRI_STATE );

  assertEqual(0x00, AD568X_CC_0000 );
  assertEqual(0x01, AD568X_CC_8000 );
  assertEqual(0x02, AD568X_CC_FFFF );
  assertEqual(0x03, AD568X_CC_NOP  );
}


unittest(constructors)
{
  AD5680 AD0(8);  //  12 bit  0..4096

  assertTrue(AD0.usesHWSPI());
}


unittest(get_type)
{
  AD568X AD0(8);

  assertEqual(0, AD0.getType());
}


unittest(get_setValue)
{
  AD5680 AD0(8);

  AD0.begin();
}


unittest(get_setPercentage)
{
  AD5680 AD0(8);

  AD0.begin();
}


unittest_main()


//  -- END OF FILE --
