# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
#import webrepl
#webrepl.start()
import upip
import network
import time

#========Network========#
def connect_to_wifi(wait_connection=True):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect("Chen's AP", "Bayarchendulu:)")
    if wait_connection:
        while not wlan.isconnected():
            time.sleep(1)
    print(wlan.ifconfig())

#========Request========#
def import_request(wlan):
    try:
        import urequests
    except ImportError as e:
        upip.install('urequests')

def main():
    connect_to_wifi()
    import_request()

main()
print("bruh")