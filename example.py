#!/usr/bin/env python3

from textual.app import App
from textual.widgets import Footer, Header, Label, Static, Input
from textual.containers import ScrollableContainer, Horizontal, Vertical, Container, VerticalScroll
from pvtui import PVLed, PVButton, PVTextMonitor
import argparse
from rich.emoji import Emoji


class ExampleApp(App):

    CSS_PATH = "example.css"

    def __init__(self, macros=None):
        super().__init__()
        self.macros = macros

    def compose(self):
        yield Header()
        with Container(id="app-grid"):
            
            # left panel
            with Vertical(id="top-left") as vert:
                vert.border_title="Control"
                with Horizontal(classes="hrow"):
                    yield PVButton("$(P)Dashboard:Connect", self.macros, label="Connect")
                    yield PVButton("$(P)Dashboard:Disconnect", self.macros, label="Disconnnect")
                for _ in range(10):
                    with Horizontal(classes="hrow"):
                        yield Label("Some more text here ")

            # top right
            with Vertical(id="top-right") as vert:
                vert.border_title = "Status"
                with Horizontal(classes="hrow"):
                    yield Label("Uptime: ")
                    yield PVTextMonitor("$(P)Receive:ControllerTimestamp", self.macros)
                with Horizontal(classes="hrow"):
                    yield Label("Connected: ")
                    yield PVLed("$(P)Dashboard:Connected", self.macros)
                with Horizontal(classes="hrow"):
                    yield Label("Remote Control: ")
                    yield PVLed("$(P)Dashboard:IsInRemoteControl", self.macros)
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("$(P)Dashboard:RobotMode", self.macros)
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("$(P)Dashboard:SafetyStatus", self.macros)

            # Bottom right
            with Vertical(id="bottom-right") as vert:
                vert.border_title = "Program"
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("$(P)Dashboard:LoadedProgram", self.macros)
                with Horizontal(classes="hrow"):
                    yield PVTextMonitor("$(P)Dashboard:ProgramState", self.macros)
                with Horizontal(classes="hrow"):
                    yield Input(placeholder="Enter URP program to load...")
                with Horizontal(classes="hrow"):
                    yield PVButton("$(P)Dashboard:Play", self.macros, label="Play")
                    yield PVButton("$(P)Dashboard:Pause", self.macros, label="Pause")
                    yield PVButton("$(P)Dashboard:Stop", self.macros, label="Stop")

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument('-macro')
    args = parser.parse_args()
    
    # macros are given in the same form as
    # caQtDM and MEDM, e.g.
    # -macro P=xxx:,R=a_macro:,M=another:
    macros_dict = dict()
    if args.macro is not None:
        for m in args.macro.split(","):
            kv = m.replace(" ", "").split("=")
            macros_dict.update({kv[0]:kv[1]})

    macros_dict = {"P" : "urExample:"}
    
    ExampleApp(macros=macros_dict).run()



