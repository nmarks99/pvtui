#!/usr/bin/env python3

from textual.app import App
from textual.widgets import Footer, Header, Label, Static, Input
from textual.containers import ScrollableContainer, Horizontal, Vertical, Container, VerticalScroll
from pvtui import PVLed, PVButton, PVTextMonitor


class ExampleApp(App):

    CSS_PATH = "example.css"

    def compose(self):
        yield Header()
        with Container(id="app-grid"):
            
            # left panel
            with Vertical(id="top-left"):
                with Horizontal(classes="hrow"):
                    yield PVButton("urExample:Dashboard:Connect", label="Connect")
                    yield PVButton("urExample:Dashboard:Disconnect", label="Disconnect")
                for _ in range(10):
                    with Horizontal(classes="hrow"):
                        yield Label("Some more text here ")

            # top right
            with Vertical(id="top-right"):
                with Horizontal(classes="hrow"):
                    yield Label("Uptime: ")
                    yield PVTextMonitor("urExample:Receive:ControllerTimestamp")
                with Horizontal(classes="hrow"):
                    yield Label("Connected: ")
                    yield PVLed("urExample:Dashboard:Connected")
                with Horizontal(classes="hrow"):
                    yield Label("Remote Control: ")
                    yield PVLed("urExample:Dashboard:IsInRemoteControl")
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("urExample:Dashboard:RobotMode")
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("urExample:Dashboard:SafetyStatus")

    
            # Bottom right
            with Vertical(id="bottom-right"):
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("urExample:Dashboard:LoadedProgram")
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("urExample:Dashboard:ProgramState")
                with Horizontal(classes="hrow"):
                    yield Input(placeholder="Enter URP program to load...")

if __name__ == "__main__":
    ExampleApp().run()
