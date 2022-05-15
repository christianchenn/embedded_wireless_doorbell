from machine import Pin
import camera
import time
import os
import ntptime as ntp

class Camera:
    def __init__(self):
        camera.init(0, format=camera.JPEG)
        # camera.flip(1)
        camera.brightness(2)
        camera.contrast(1)
    
    def capture(self):
        try:
            dt = time.localtime(ntp.time())
            print("Captured At: {0}/{1}/{2} {3}:{4}:{5}"
                  .format(dt[2],dt[1],dt[0],dt[3],dt[4],dt[5]))
            img = camera.capture()
            if len(img) < 0:
                filename = "{0}.jpeg".format(str(ntp))
                f = open(filename, 'w')
                f.write(img)
                f.close()
                print(os.listdir('/'))
        except:
            print('Camera Capture Error')
            return false

    def deinitCamera(self):
        camera.deinit()
        print("Deinit camera")
        
