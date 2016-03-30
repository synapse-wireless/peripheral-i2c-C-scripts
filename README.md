# peripheral-i2c-C-scripts
An i2c driver, using the AVR's peripheral i2c driver.  

Note that unlike SNAPpy i2c, the AVR i2c pins are not relocatable, so this will only be applicable on boards where pins 25 (PD0) and 26 (PD1) of the ATmega128 have been pinned out, such as the SN172 or SN173 protoboards, or your own custom device.  

Included in the I2C examples are drivers for interacting with two sensors - the TCS34725 Color Sensor and the Sparkfun ZX Distance and Gesture Sensor.  Neither go into the full range of what the devices can do; they're intended to provide a top level view of how you can pass data from a read/write in the driver back and forth from SNAPpy.
