/*
* Sparkfun ZX Distance and Gesture Interface
*
* This application provides an example of interfacing with a Sparkfun ZX Distance 
* and Gesture sensor via I2C.  This is only an example, to help get you started 
* with I2C in C called via SNAPpy.  Some of the details of what registers 
* specifically control what are intentionally not explored in detail and some
* device functions are left to the reader to implement if so desired.
* 
* Refer to the datasheet for a more thorough explanation of register functionality.
*/

#include "stdlib.h"
#include "..\i2c\peripheral_i2c.h"
#include "snappy.h"
#include "snap_utils.h"
#include <intrinsics.h>

const int16_t zx_addr = 0x20;
char buffer[4];

/*
*  Read from the color sensor, into our input buffer.
*  Do not end with restart.
*/
int16_t read(uint8_t bytes)
{
    return i2c_read(zx_addr, buffer, bytes, false);
}


/*
*  Write the input buffer 'value' to the color sensor.
*  Do not end with restart.
*/
int16_t write(char *value, int16_t length)
{
   return i2c_write(zx_addr, value, length, false);
}

/*
*  Init our i2c with the clock settings we want.
*
*  Disable the interrupt pin and interrupts, we will just poll when we
*  want info in this simple example.
*/
int16_t zx_init()
{
    uint8_t twbr = 16;
    uint8_t twps = 0;
    uint16_t intercharacter_timeout = 128;
    uint8_t retries = 10;
    i2c_init(twbr, twps, intercharacter_timeout, retries);
    
    int16_t rva;
    rva = write("\x01\x00", 2);
    
    if (rva != 0)
    {
        //pyerr(rva);
        return rva;
    }
    
    rva = write("\x02\x00", 2);
    
    if (rva != 0)
    {
        //pyerr(rva);
        return rva;
    }
    
    return 0;
}
    
    
/*
*  Connectivity test - read the Sensor Model register (0xFF)
*/
int16_t getModelVersion()
{
    int16_t rva;
    rva = write("\xFF", 1);
    
    if (rva != 0)
    {
        //pyerr(rva);
        return rva;
    }
    
    // the ZX is a bit slow, needs a bit to respond
    delay(1024);
    
    rva = read(1);

    if (rva != 0)
    {
        //pyerr(rva);
        return rva;
    }

    return buffer[0];
}

/*
*  Read the last gesture register (0x04)
*/
int16_t getLastGesture()
{
    int16_t rva;
    rva = write("\x04", 1);
    
    if (rva != 0)
    {
        //pyerr(rva);
        return -1;
    }
    
    // the ZX is a bit slow, needs a bit to respond
    delay(1024);
    
    rva = read(1);
    
    if (rva != 0)
    {
        //pyerr(rva);
        return -1;
    }
        
    return buffer[0];
}

/*
* Read the Z distance register (0x0A).
*/
int16_t getZ()
{
    int16_t rva;
    rva = write("\x0A", 1);

    if (rva != 0)
    {
        //pyerr(rva);
        return -1;
    } 

    // the ZX is a bit slow, needs a bit to respond
    delay(1024);
    
    rva = read(1);

    if (rva != 0)
        return rva;
    
    return buffer[0];

}



