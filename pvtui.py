from textual import on
from textual.reactive import reactive
from textual.widgets import Static, Button
from epics import PV
from rich.emoji import Emoji

class PVTextMonitor(Static):
    
    '''
    Displays a the value of a PV as text
    '''

    pv_value =  reactive("?")

    def __init__(self, pv_name, **kwargs):
        super().__init__()
        self.pv_name = pv_name
        if "connection_timeout" in kwargs:
            self.pv = PV(self.pv_name, connection_timeout=kwargs["connection_timeout"])
        else:
            self.pv = PV(self.pv_name, connection_timeout=1.0)
        self.pv.add_callback(self.pv_callback)

    def pv_callback(self,**kwargs):
        '''Called whenever a change occurs with the PV through channel access'''
        if "value" in kwargs:
            val = kwargs["value"]
            self.pv_value = val

    def watch_pv_value(self):
        '''Called whenever self.pv_value changes'''
        self.update(str(self.pv_value))

class PVLed(Static):

    '''
    Displays a binary PV as a green circle when 1, and red circle when 0.
    The label for the high/low values is customizable
    '''
    
    pv_value = reactive("?")

    def __init__(self, pv_name,
                 high_label=Emoji("green_circle"),
                 low_label=Emoji("red_circle"),
                 other_label=Emoji("white_circle"),
                 **kwargs):
        super().__init__()
        self.high_label=high_label
        self.low_label=low_label
        self.other_label = other_label
        self.pv_name = pv_name
        if "connection_timeout" in kwargs:
            self.pv = PV(self.pv_name, connection_timeout=kwargs["connection_timeout"])
        else:
            self.pv = PV(self.pv_name, connection_timeout=1.0)
        self.pv.add_callback(self.pv_callback)

    def pv_callback(self,**kwargs):
        '''Called whenever a change occurs with the PV through channel access'''
        if "value" in kwargs:
            val = kwargs["value"]
            self.pv_value = val

    def watch_pv_value(self):
        '''Called whenever self.pv_value changes'''
        if self.pv_value == 1:
            self.update(str(self.high_label))
        elif self.pv_value == 0:
            self.update(str(self.low_label))
        else:
            self.update(str(self.other_label))

    
class PVButton(Button):

    '''
    Button that writes a value to a PV when pressed
    '''
    
    def __init__(self, pv_name, label=None, press_val=1, **kwargs):
        super().__init__()
        self.pv_name = pv_name
        self.press_val = press_val
        self.button_label = label if label is not None else self.pv_name
        if "connection_timeout" in kwargs:
            self.pv = PV(self.pv_name, connection_timeout=kwargs["connection_timeout"])
        else:
            self.pv = PV(self.pv_name)
        
    @on(Button.Pressed, "#trigger")
    def write_pv(self):
        self.pv.put(self.press_val)
        
    def compose(self):
        yield Button(label=self.button_label, variant="primary", id="trigger")
