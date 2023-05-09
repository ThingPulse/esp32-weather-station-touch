/*
 * - ThingPulse -
 * This version taken from https://github.com/DustinWatts/FT6236/pull/9
 * To be removed as soon as the new version at https://github.com/DustinWatts/FocalTech_Touch is released and available in the PIO registry.
 */


/*
This is a library for the FT6236 touchscreen controller by FocalTech.

The FT6236 and FT6236u work the same way.

A lot of this library is originally written by Limor Fried/Ladyada.

Because Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

@section author Author
Written by Limor Fried/Ladyada for Adafruit Industries.

@section license License
MIT license, all text above must be included in any redistribution
*/

#include "Arduino.h"
#include "FT6236.h"
#include <Wire.h>

/* New class. */
FT6236::FT6236(uint16_t width, uint16_t height)
{
    touches = 0;
    _touch_width = width;
    _touch_height = height;
}

/* Start I2C and check if the FT6236 is found. */
boolean FT6236::begin(uint8_t thresh, int8_t sda, int8_t scl)
{
    if (sda != -1 && scl != -1)
    {
        Wire.begin(sda, scl);
    }
    else
    {
        Wire.begin();
    }

    // Adjust threshold
    writeRegister8(FT6236_REG_THRESHHOLD, thresh);

    // Check if our chip has the correct Vendor ID
    uint8_t id = readRegister8(FT6236_REG_VENDID);
    if ((id != FT6236_VENDID) && (id != FT5436_VENDID))
    {
        return false;
    }
    // Check if our chip has the correct Chip ID.
    id = readRegister8(FT6236_REG_CHIPID);
    if ((id != FT6236_CHIPID) && (id != FT6236U_CHIPID) &&
        (id != FT6206_CHIPID) && (id != FT5436_CHIPID))
    {
        return false;
    }

    return true;
}

void FT6236::setRotation(uint8_t rotation)
{
    _rotation = rotation;
}

/* Returns the number of touches */
uint8_t FT6236::touched(void)
{
    uint8_t n = readRegister8(FT6236_REG_NUMTOUCHES);
    if (n > 2)
    {
        n = 0;
    }
    return n;
}

/* Get a touch point */
TS_Point FT6236::getPoint(uint8_t n)
{
    readData();
    if ((touches == 0) || (n > 1))
    {
        return TS_Point(0, 0, 0, _touch_width, _touch_height, _rotation);
    }
    else
    {
        return TS_Point(touchX[n], touchY[n], 1, _touch_width, _touch_height, _rotation);
    }
}

void FT6236::readData(void)
{

    uint8_t i2cdat[16];
    Wire.beginTransmission(FT6236_ADDR);
    Wire.write((byte)0);
    Wire.endTransmission();

    Wire.requestFrom((byte)FT6236_ADDR, (byte)16);
    for (uint8_t i = 0; i < 16; i++)
        i2cdat[i] = Wire.read();

    touches = i2cdat[0x02];
    if ((touches > 2) || (touches == 0))
    {
        touches = 0;
    }

    for (uint8_t i = 0; i < 2; i++)
    {
        touchX[i] = i2cdat[0x03 + i * 6] & 0x0F;
        touchX[i] <<= 8;
        touchX[i] |= i2cdat[0x04 + i * 6];
        touchY[i] = i2cdat[0x05 + i * 6] & 0x0F;
        touchY[i] <<= 8;
        touchY[i] |= i2cdat[0x06 + i * 6];
        touchID[i] = i2cdat[0x05 + i * 6] >> 4;
    }
}

/* Reading a byte from a register */
uint8_t FT6236::readRegister8(uint8_t reg)
{
    uint8_t x;

    Wire.beginTransmission(FT6236_ADDR);
    Wire.write((byte)reg);
    Wire.endTransmission();

    Wire.requestFrom((byte)FT6236_ADDR, (byte)1);
    x = Wire.read();

    return x;
}

/* Writing a byte to a register */
void FT6236::writeRegister8(uint8_t reg, uint8_t val)
{

    Wire.beginTransmission(FT6236_ADDR);
    Wire.write((byte)reg);
    Wire.write((byte)val);
    Wire.endTransmission();
}

/* Debug */
void FT6236::debug(void)
{
    Serial.print("Vend ID: 0x");
    Serial.println(readRegister8(FT6236_REG_VENDID), HEX);
    Serial.print("Chip ID: 0x");
    Serial.println(readRegister8(FT6236_REG_CHIPID), HEX);
    Serial.print("Firm V: ");
    Serial.println(readRegister8(FT6236_REG_FIRMVERS));
    Serial.print("Point Rate Hz: ");
    Serial.println(readRegister8(FT6236_REG_POINTRATE));
    Serial.print("Thresh: ");
    Serial.println(readRegister8(FT6236_REG_THRESHHOLD));
}

TS_Point::TS_Point(void) { x = y = z = 0; }

TS_Point::TS_Point(int16_t _x, int16_t _y, int16_t _z, uint16_t width, uint16_t height, uint8_t rotation)
{
    uint16_t t;
    switch (rotation)
    {
    case 0:
        _x = height - _x;
        _y = width - _y;
        break;
    case 1:
        t = _x;
        _x = _y;
        _y = t;
        _x = width - _x;
        break;
    case 2:
        break;
    case 3:
        t = _x;
        _x = _y;
        _y = t;
        _y = height - _y;
        break;
    }
    x = _x;
    y = _y;
    z = _z;
}

/* == comparator between two points */
bool TS_Point::operator==(TS_Point p1)
{
    return ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

/* != comparator netween two points */
bool TS_Point::operator!=(TS_Point p1)
{
    return ((p1.x != x) || (p1.y != y) || (p1.z != z));
}
