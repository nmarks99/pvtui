from epics import PV
import time

class Asdf:
    
    def __init__(self):
        self.pv = PV("min1:value")
        if self.pv is None:
            print("it is none")
        else:
            print(type(self.pv))
            print(f"pv.info = {self.pv.info}")
            print(f"pv.status = {self.pv.status}")
        self.pv_value = self.pv.get()
        self.pv.add_callback(self.on_pv_change)

        self.run()
    
    def run(self):
        count = 0 
        try: 
            while True:
                time.sleep(0.1)
                print(count)
                count += 1
        except KeyboardInterrupt:
            print("done")
    
    def on_pv_change(self, **kwargs):
        if "value" in kwargs:
            print(kwargs["value"])


if __name__ == "__main__":
    Asdf()
