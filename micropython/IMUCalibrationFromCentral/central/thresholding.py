import time
import utime
from gpio_setup import _send_zone_via_gpio
#State constants
WAITING = 0
WAITING_FOR_HIGHEST_VALUE = 1
WAITING_FOR_RETURN_TO_CENTERED_VALUE = 2

class ThresholdCrossing:
    def __init__(self):
        """
        This class has functionality to determine the highest acceleration point reached when
        the initial green zone is crossed. It does this by using non-blocking timers. Based
        on the highest point reached it then matches that to a corresponding zone

        Args:
            None
        """ 
        self.green_zone = 3
        self.yellow_zone = 6
        self.amber_zone = 9
        self.red_zone = 12
        
        self.start_time = None
        self.zone_time = None
        self.zone_time_started = 0
        self.zone_time_ended = 0
        self.highest_value = 0
        self.state = WAITING

    def _determine_threshold_crossing(self, average_accel):
        """
        Determine the state based on the threshold crossing logic.

        Args:
            average_accel (float): The average acceleration value.

        Returns:
            int: The current state.
        """
        #print("average_accel", average_accel)
        
        if self.start_time is None:
            self.start_time = time.time()  # Initialize start time on first function call

        if time.time() - self.start_time < 3:
            return WAITING  # If less than 2 seconds have passed, return WAITING

        #### Main logic for determining the higest value reached
        
        #when average accel crosses green zone start the timer
        if average_accel > self.green_zone and self.zone_time is None:
            self.highest_value = average_accel  # Record the highest value
            print("zone_timer has started")
            self.zone_time = utime.ticks_us()  # Start zone timer
            self.zone_time_started = self.zone_time
            self.state = WAITING_FOR_HIGHEST_VALUE

        #while the zone timer is running find the highest value reached 
        elif self.zone_time is not None:
            #print("difference", time.time() - self.zone_time)
            if utime.ticks_us() - self.zone_time < 100000:  # 100 milliseconds in microseconds
                #print("difference", time.time() - self.zone_time)
                if average_accel > self.highest_value:
                    self.highest_value = average_accel  # Update highest value                    
            
            elif average_accel > self.green_zone:
                print("waiting for value to return to 0")
                self.state = WAITING_FOR_RETURN_TO_CENTERED_VALUE
            else:
                print("zone_timer is reset")
                self.zone_time = None
                self.zone_time_ended = utime.ticks_us()
                print("time elapsed:", self.zone_time_ended - self.zone_time_started)
                return self.highest_value
                
        return self.state
    
    def _determine_zone(self, value):
        print("highest_value is reset")
        self.highest_value = 0
        print("value", value)
        
        if value > self.amber_zone:
            print("RED ZONE")
            return self.red_zone
        if value > self.yellow_zone:
            print("AMBER ZONE")
            return self.amber_zone
        elif value > self.green_zone:
            print("YELLOW ZONE")
            return self.yellow_zone
        elif value <= self.green_zone:
            pass
            #print("GREEN ZONE")
    def _transmit_zone(self, zone):
        if zone == self.red_zone:
            _send_zone_via_gpio(3)
        elif zone == self.amber_zone:
            _send_zone_via_gpio(2)
        elif zone == self.yellow_zone:
            _send_zone_via_gpio(1)
        elif zone == self.green_zone:
            _send_zone_via_gpio(0)
            
            
 