#!/usr/bin/env python3

from time import monotonic
from textual.app import App
from textual import on
from textual.reactive import reactive
from textual.widgets import Static, Button, Footer, Header
from textual.containers import ScrollableContainer

class TimeDisplay(Static):
    '''Custom time display widget'''
    
    accumulated_time = 0.0
    start_time = monotonic()
    time_elapsed = reactive(0)

    def on_mount(self):
        
        # Whenever the widget is mounted, start a textual timer
        # that calls the self.update_time_elapsed function every 1/60th
        # of a second
        self.update_timer = self.set_interval(
            1.0 / 60.0,
            self.update_time_elapsed,
            pause=True
        )

    def update_time_elapsed(self):
        self.time_elapsed = (monotonic() - self.start_time) + self.accumulated_time

    def watch_time_elapsed(self):
        time = self.time_elapsed
        time, seconds = divmod(time, 60)
        hours, minutes = divmod(time, 60)
        time_string = f"{hours:02.0f}:{minutes:02.0f}:{seconds:05.2f}"
        self.update(time_string)

    def start(self):
        '''start keeping track of the time elapsed'''
        self.start_time = monotonic()
        self.update_timer.resume()

    def stop(self):
        '''stop keeping track of the time elapsed'''
        self.accumulated_time = self.time_elapsed
        self.update_timer.pause()

    def reset(self):
        '''reset the time elapsed'''
        self.accumulated_time = 0.0
        self.time_elapsed = 0.0


class Stopwatch(Static):
    '''Custom stopwatch widget'''
    
    @on(Button.Pressed, "#start")
    def start_stopwatch(self):
        self.add_class("started")
        self.query_one(TimeDisplay).start()

    @on(Button.Pressed, "#stop")
    def stop_stopwatch(self):
        self.remove_class("started")
        self.query_one("TimeDisplay").stop()

    @on(Button.Pressed, "#reset")
    def reset_stopwatch(self):
        self.query_one("TimeDisplay").reset()

    def compose(Self):
        yield Button(
            "Start",
            variant="success",
            id="start"
        ) 
        yield Button(
            "Stop",
            variant="error",
            id="stop",
        )
        yield Button(
            "Reset",
            variant="default",
            id="reset",
        )
        yield TimeDisplay("00:00:00.00")

class StopwatchApp(App):

    #  (key, action_name, description),
    BINDINGS = [
        ("d", "toggle_dark_mode", "Toggle dark mode"),
        ("a", "add_stopwatch", "Adds a new stopwatch"),
        ("r", "remove_stopwatch", "Removes the last stopwatch")
    ]
    
    CSS_PATH = "stopwatch.css"

    def compose(self):
        # yield each widget you want
        yield Header(show_clock=True)
        yield Footer()
        with ScrollableContainer(id="stopwatches"):
            yield Stopwatch()
            yield Stopwatch()
            yield Stopwatch()

    # This is an ACTION method becasue it starts with action_
    def action_toggle_dark_mode(self):
        self.dark = not self.dark

    def action_add_stopwatch(self):
        stopwatch = Stopwatch()
        container = self.query_one("#stopwatches")
        container.mount(stopwatch)
        stopwatch.scroll_visible()

    def action_remove_stopwatch(self):
        all_stopwatches = self.query(Stopwatch)
        if all_stopwatches:
            all_stopwatches.last().remove()


if __name__ == "__main__":
    StopwatchApp().run()
