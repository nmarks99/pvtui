from epics import PV
import time

class Asdf:
    
    def __init__(self):
        self.pv = PV("min1:value")
        self.pv_value = self.pv.get()
        self.pv.add_callback(self.on_pv_change)

        self.run()

    def run(self):
        try: 
            while True:
                time.sleep(1e-4)
        except KeyboardInterrupt:
            print("done")
    
    def on_pv_change(self, **kwargs):
        if "value" in kwargs:
            print(kwargs["value"])


if __name__ == "__main__":
    Asdf()
