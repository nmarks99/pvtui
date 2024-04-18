#!/usr/bin/env python3

from textual.app import App
from textual.widgets import Footer, Header, Label
from textual.containers import ScrollableContainer, Horizontal, Vertical
from pvtui import PVLed, PVButton, PVTextMonitor

class ExampleApp(App):

    #  (key, action_name, description),
    BINDINGS = [
        ("d", "toggle_dark_mode", "Toggle dark mode"),
    ]
    
    CSS_PATH = "example.css"

    def compose(self):
        yield Header(show_clock=True)
        yield Footer()
        with ScrollableContainer(id="textmons"):
            with Vertical(id="vert"):
                with Horizontal():
                    yield Label("Robot Mode: ")
                    yield PVTextMonitor("urExample:Dashboard:RobotMode")
                with Horizontal():
                    yield Label("Uptime: ")
                    yield PVTextMonitor("urExample:Receive:ControllerTimestamp")
                with Horizontal():
                    yield Label("Connection: ")
                    yield PVLed("urExample:Dashboard:Connected")
                with Horizontal():
                    yield PVButton("urExample:Dashboard:Disconnect",label="Disconnect")
                    yield PVButton("urExample:Dashboard:Connect",label="Connect")

    # This is an ACTION method becasue it starts with action_
    def action_toggle_dark_mode(self):
        self.dark = not self.dark

if __name__ == "__main__":
    ExampleApp().run()
