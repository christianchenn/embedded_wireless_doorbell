from machine import Pin, UART, SPI, ADC, Timer
from modules.Button import Button
from modules.Camera import Camera
import camera

# import os, network, time
# import urequests as request
# import machine, camera
# Button IO0
button = Button(0)
camera = Camera(camera)

if __name__ == '__main__':
    
    while True:
        if button.is_pressed():
            camera.capture()
            print('captured')
        # Save Button State
        button.save_state()
    
    
