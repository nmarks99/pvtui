#!/usr/bin/env python3

from textual.app import App
from textual.reactive import reactive
from textual.widgets import Static, Footer, Header
from textual.containers import ScrollableContainer
from epics import PV

class TextMonitor(Static):

    pv = PV("min1:value")
    pv_value = reactive(pv.get())

    def on_mount(self):
        self.pv.add_callback(self.on_pv_change)
        #  self.update_timer = self.set_interval(
            #  1.0 / 60.0,
            #  self.debug_func,
        #  )

    def on_pv_change(self,**kwargs):
        if "value" in kwargs:
            val = kwargs["value"]
            self.pv_value = val

    def watch_pv_value(self):
        self.update(str(self.pv_value))


class Box(Static):
    def compose(Self):
        yield TextMonitor("0.00")

class pvtuiApp(App):

    #  (key, action_name, description),
    BINDINGS = [
        ("d", "toggle_dark_mode", "Toggle dark mode"),
    ]
    
    CSS_PATH = "pvtui.css"

    def compose(self):
        # yield each widget you want
        yield Header(show_clock=True)
        yield Footer()
        with ScrollableContainer(id="textmons"):
            yield Box()

    # This is an ACTION method becasue it starts with action_
    def action_toggle_dark_mode(self):
        self.dark = not self.dark

if __name__ == "__main__":
    pvtuiApp().run()
