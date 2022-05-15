import os
from machine import Pin, SoftSPI
from sdcard import SDCard

class SDCard:
    # SD Card Pin Assignment
    # MISO -> GPIO 13
    # MOSI -> GPIO 12
    # SCK  -> GPIO 14
    # CS   -> GPIO 27
    def __init__(self, miso, mosi, sck, cs):
        self.soft_spi = SoftSPI(-1, miso=Pin(miso), mosi=Pin(mosi), sck=Pin(sck))
        self.sd_card = SDCard(self.soft_spi, Pin(cs))
    def mount(self):
        vfs = os.VfsFat(self.sd_card)
        os.mount(vfs, "/sd")
    def list_dir(self):
        listdir = []
        if os.getcwd() != '/sd':
            os.chdir('sd')
        listdir = os.listdir()
        os.chdir('/')
        return 'SD Card contains: {}'.format(listdir)
    

