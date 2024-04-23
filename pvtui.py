from textual import on, events
from textual.reactive import reactive
from textual.widgets import Static, Button, Input
from epics import PV
from rich.emoji import Emoji

def get_pv_type(pv: PV) -> type:
    '''Gets the python type of a PV'''
    type_str = pv.type.replace("time_","")
    if type_str in ("string", "char"):
        return str
    elif type_str in ("double", "float"):
        return float
    elif type_str in ("int", "long", "short"):
        return int
    else:
        # TODO: error logging for unhandled type?
        return None

def has_method(o, name):
    return callable(getattr(o, name, None))

def _epics_handler(obj):
    '''
    Handles connection to a PV given a PV Widget object
    such as PVTextMonitor, etc.

    The object optionally implements PV callback
    and connection callbacks which must be named "pv_callback"
    and "pv_conn_callback" respectively. 

    This function returns the PV object and the string PV name
    with macro substitutions
    '''

    # Replace macros in PV name
    pv_name = obj.pv_name
    for k,v in obj.macros.items():
        pv_name = pv_name.replace(f"$({k})", v)
    
    pv_callback = obj.pv_callback if has_method(obj, "pv_callback") else None
    pv_conn_callback = obj.pv_conn_callback if has_method(obj, "pv_conn_callback") else None

    pv = PV(
        pv_name,
        connection_timeout=obj.connection_timeout,
        callback=pv_callback,
        connection_callback=pv_conn_callback
    )
    
    return pv, pv_name

class PVTextMonitor(Static):
    
    '''
    Displays a the value of a PV as text
    '''

    pv_value = reactive("?")

    def __init__(self, pv_name, macros, connection_timeout=1.0, **kwargs):
        super().__init__(**kwargs)

        self.pv_type = None
        self.pv_connected = False
        self.pv_name = pv_name
        self.macros = macros
        self.connection_timeout = connection_timeout

        # PV connection
        self.add_class("pv_down")
        self.pv, self.pv_name = _epics_handler(self)

    def pv_callback(self,**pv_kwargs):
        '''Called whenever a change occurs with the PV through channel access'''
        if "value" in pv_kwargs:
            self.pv_value = pv_kwargs["value"]

    def pv_conn_callback(self, **pv_kwargs):
        if "conn" in pv_kwargs:
            self.pv_connected = pv_kwargs["conn"]
        if self.pv_connected:
            self.pv_type = get_pv_type(self.pv)
            self.remove_class("pv_down")
        else:
            self.add_class("pv_down")

    def watch_pv_value(self):
        '''Called whenever self.pv_value changes'''
        self.update(str(self.pv_value))

    @on(events.Click)
    def on_click(self, event: events.Click):
        # right mouse click
        if event.button == 3:
            print(self.pv_name)


class PVLed(Static):

    '''
    Displays a binary PV as a green circle when 1, and red circle when 0.
    The label for the high/low values is customizable
    '''
    
    pv_value = reactive(None)

    def __init__(self, pv_name, macros,
                 connection_timeout=1.0,
                 high_label=Emoji("green_circle"),
                 low_label=Emoji("red_circle"),
                 other_label=Emoji("white_circle"),
                 **kwargs):
        super().__init__(**kwargs)

        self.pv_name = pv_name
        self.macros = macros
        self.connection_timeout = connection_timeout
        self.high_label=high_label
        self.low_label=low_label
        self.other_label = other_label
        self.pv_connected = False
    
        # PV connection
        self.add_class("pv_down")
        self.pv, self.pv_name = _epics_handler(self)

    def pv_callback(self,**kwargs):
        '''Called whenever a change occurs with the PV through channel access'''
        if "value" in kwargs:
            self.pv_value = kwargs["value"]

    def pv_conn_callback(self, **pv_kwargs):
        if "conn" in pv_kwargs:
            self.pv_connected = pv_kwargs["conn"]
        if self.pv_connected:
            self.remove_class("pv_down")
        else:
            self.add_class("pv_down")
            self.pv_value = None

    def watch_pv_value(self):
        '''Called whenever self.pv_value changes'''
        if self.pv_value == 1:
            self.update(str(self.high_label))
        elif self.pv_value == 0:
            self.update(str(self.low_label))
        else:
            self.update(str(self.other_label))


class PVInput(Input):

    def __init__(self, pv_name, macros, connection_timeout=1.0, **kwargs):
        super().__init__(**kwargs)
        
        self.pv_name = pv_name
        self.macros = macros
        self.connection_timeout = connection_timeout
        self.pv_connected = False

        # PV connection
        self.pv, self.pv_name = _epics_handler(self)

    def pv_conn_callback(self, **pv_kwargs):
        if "conn" in pv_kwargs:
            self.pv_connected = pv_kwargs["conn"]
        #  if self.pv_connected: #FIX: this is broken
            #  self.remove_class("pv_down")
        #  else:
            #  self.add_class("pv_down")

    @on(Input.Submitted)
    def write_pv(self, event: Input.Submitted):
        self.pv.put(event.value)


class PVButton(Button):

    '''
    Button that writes a value to a PV when pressed
    '''
    
    def __init__(self, pv_name, macros, connection_timeout=1.0, press_val=1, **kwargs):
        super().__init__(**kwargs)
        
        self.pv_name = pv_name
        self.macros = macros
        self.connection_timeout = connection_timeout
        self.press_val = press_val
        self.pv_connected = False
        
        # PV connection
        self.add_class("pv_down")
        self.pv, self.pv_name = _epics_handler(self)

    def pv_conn_callback(self, **pv_kwargs):
        if "conn" in pv_kwargs:
            self.pv_connected = pv_kwargs["conn"]
        if self.pv_connected:
            self.remove_class("pv_down")
        else:
            self.add_class("pv_down")


    #TODO: Check that this works without the id?
    @on(Button.Pressed)
    def write_pv(self):
        self.pv.put(self.press_val)
