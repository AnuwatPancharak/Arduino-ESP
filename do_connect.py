import network
import time
import urequests as requests
from secrets import *
 
def do_connect(ssid=secrets['ssid'],psk=secrets['password']):
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, psk)
 
    # Wait for connect or fail
    wait = 10
    while wait > 0:
        if wlan.status() < 0 or wlan.status() >= 3:
            break
        wait -= 1
        print('waiting for connection...')
        time.sleep(1)
 
    # Handle connection error
    if wlan.status() != 3:
        raise RuntimeError('wifi connection failed')
    else:
        response = requests.post("https://sgp1.blynk.cloud/external/api/update?token=nuhXyfffcxCPXuPuIKklSqAvQ5ytYxPg&V1=1")
        response.close()
        print('connected')
        ip=wlan.ifconfig()[0]
        print('network config: ', ip)
        return ip