import_c("zx_gesture_sensor.c")
import_c("..\i2c\peripheral_i2c.c")

@c_function(api=["int"])
def zx_init():
    pass

@c_function(api=["int"])
def getZ():
    pass

@c_function(api=["int"])
def getModelVersion():
    pass
    
@c_function(api=["int"])
def getLastGesture():
    pass