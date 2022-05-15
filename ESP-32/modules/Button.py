from machine import Pin

class Button:
    def __init__(self, pin):
        self.state = False
        self.before_state = False
        self.button = Pin(pin, Pin.IN)
        
    def is_pressed(self):
        self.state = bool(self.button.value())
        if self.state is not True and self.before_state is True:
            return True
        return False
    
    def save_state(self):
        self.before_state = self.state
