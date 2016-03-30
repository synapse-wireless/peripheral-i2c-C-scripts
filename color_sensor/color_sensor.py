import_c("..\i2c\peripheral_i2c.c")
import_c("color_sensor.c")

@c_function(api=["int"])
def color_sensor_init():
    pass

@c_function(api=["int"])
def test_connection():
    pass

@c_function(api=["int"])
def getRed():
    pass
    
@c_function(api=["int"])
def getGreen():
    pass
    
@c_function(api=["int"])
def getBlue():
    pass
    
