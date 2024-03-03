import machine

POTHOLE_EVENT_PIN = 19 #GP19
BIT_0_PIN = 18 #GP18
BIT_1_PIN = 17 #GP17
BIT_2_PIN = 16 #GP16

pothole_event_pin = machine.Pin(POTHOLE_EVENT_PIN, machine.Pin.OUT)
bit_0 = machine.Pin(BIT_0_PIN, machine.Pin.OUT)
bit_1 = machine.Pin(BIT_1_PIN, machine.Pin.OUT)
bit_2 = machine.Pin(BIT_2_PIN, machine.Pin.OUT)

#set default values for the pin
pothole_event_pin.value(1)
bit_0.value(1)
bit_1.value(1)
bit_2.value(1)

def _send_zone_via_gpio(zone):
    if(zone == 0):
        
        print("sending value over GPIO")
        #b'0
        bit_0.value(0)
        bit_1.value(0)
        bit_2.value(0)
        
        #green_zone
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
            
        
        
    elif(zone == 1):
        
        print("sending value over GPIO")
        #b'1
        bit_0.value(1)
        bit_1.value(0)
        bit_2.value(0)
        
        #yellow_zone
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
            
    elif(zone == 2):
        print("sending value over GPIO")
        #b'2
        bit_0.value(0)
        bit_1.value(1)
        bit_2.value(0)
        
        #amber_zone
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
        
    elif(zone == 3):
        print("sending value over GPIO")
        #b'3
        bit_0.value(1)
        bit_1.value(1)
        bit_2.value(0)
        
        #red_zone
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
   
    elif(zone == 4):
        print("sending value over GPIO")
        #b'4
        bit_0.value(0)
        bit_1.value(0)
        bit_2.value(1)
        
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
            
    elif(zone == 5):
        print("sending value over GPIO")
        #b'5
        bit_0.value(1)
        bit_1.value(0)
        bit_2.value(1)
        
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
            
    elif(zone == 6):
        print("sending value over GPIO")
        #b'6
        bit_0.value(0)
        bit_1.value(1)
        bit_2.value(1)
        
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
        
    elif(zone == 7):
        print("sending value over GPIO")
        #b'7
        bit_0.value(1)
        bit_1.value(1)
        bit_2.value(1)
        
        print("transition")
        pothole_event_pin.value(0)
        pothole_event_pin.value(1)
        
        
    