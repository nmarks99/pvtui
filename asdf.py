from epics import PV
import time
import random

pv = PV("min1:value")

for i in range(50):
    pv.put(round(random.random(), 2))
    time.sleep(0.2)
