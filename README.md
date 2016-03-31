[![](https://cloud.githubusercontent.com/assets/1317406/12406044/32cd9916-be0f-11e5-9b18-1547f284f878.png)](http://www.synapse-wireless.com/)

# SNAPpy Example - AVR I2C Scripts

An i2c driver, using the AVR's peripheral i2c driver.  

Note that unlike SNAPpy i2c, the AVR i2c pins are not relocatable, so this will only be applicable on boards where pins 25 (PD0) and 26 (PD1) of the ATmega128 have been pinned out, such as the SN172 or SN173 protoboards, or your own custom device.  

Included in the I2C examples are drivers for interacting with two sensors - the TCS34725 Color Sensor and the Sparkfun ZX Distance and Gesture Sensor.  Neither go into the full range of what the devices can do; they're intended to provide a top level view of how you can pass data from a read/write in the driver back and forth from SNAPpy.

## License

Copyright © 2016 [Synapse Wireless](http://www.synapse-wireless.com/), licensed under the [Apache License v2.0](LICENSE.md).
