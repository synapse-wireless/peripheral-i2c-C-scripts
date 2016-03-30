/*
* TCS34725 Color Sensor I2C Interface
*
* This application provides an example of interfacing with a TCS34725 color 
* sensor via I2C.  This is only an example, to help get you started with I2C
* in C called via SNAPpy.  Some of the details of what registers specifically
* control what are intentionally not explored in detail and several device
* functions are left to the reader to implement if so desired.
*
* Refer to the datasheet for a more thorough explanation of register functionality.
*/

#include <stdlib.h>
#include <intrinsics.h>

#include "..\i2c\peripheral_i2c.h"
#include "snappy.h"
#include "snap_utils.h"

const int16_t color_addr = 0x52;

char buffer[4];

/*
*  Read from the color sensor, into our input buffer.
*  Do not end with restart.
*/
int16_t read(uint8_t bytes)
{
    return i2c_read(color_addr, buffer, bytes, false);
}


/*
*  Write the input buffer 'value' to the color sensor.
*  Do not end with restart.
*/
int16_t write(char *value, int16_t length)
{
   return i2c_write(color_addr, value, length, false);
}

/*
* Init our i2c with the clockiing settings we want.
*
* Set the integration time (0x81)
* Set the gain (0x8F)
* Enable (0x80)
*/
int16_t color_sensor_init()
{
    uint8_t twbr = 8;
    uint8_t twps = 0;
    uint8_t intercharacter_timeout = 128;
    uint8_t retries = 5;

    i2c_init(twbr, twps, intercharacter_timeout, retries);

    write("\x81\xFF", 2);
    write("\x8F\x01", 2);
    write("\x80\x01", 2);
    delay(18750);
    write("\x80\x03", 2);
    return 0;
}

/*
* Test the connection by reading the ID register (0x92)
*
* I2C read will store the value in 'buffer', and return 0 on success,
* and an error code on any other failure.
*/
int16_t test_connection()
{
    write("\x92", 1);
    int16_t rva = read(1);
    if (rva != 0)
        return rva;
    else
        return buffer[0];
}

/*
* Helper function to perform a I2C read of two bytes and generate
* an int16_t out of the result.
* 
* I2C read will store the value in 'buffer', and return 0 on success,
* and an error code on any other failure.
*/
int16_t colorSensorRead16(char reg)
{
    char command_register = 0x80 + reg;
    
    write(&command_register, 1);
    int16_t rva = read(2);
    if (rva != 0)
        return rva;
    
    int16_t data = buffer[0] | (buffer[1]<<8);
    return data;
}
    
/*
*  Get the red register (0x16) value
*/
int16_t getRed()
{
    int8_t red = colorSensorRead16('\x16');
    return red;
}

/*
*  Get the green register (0x18) value
*/
int16_t getGreen()
{
    int8_t green = colorSensorRead16('\x18');
    return green;
}

/*
*  Get the blue register (0x1A) value
*/
int16_t getBlue()
{
    int8_t blue = colorSensorRead16('\x1A');
    return blue;
}
