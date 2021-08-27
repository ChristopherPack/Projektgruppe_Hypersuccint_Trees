import time

import pht_timer

tmr = pht_timer.PyTimer()
print(tmr)
tmr.start()
time.sleep(2.4)
tmr.stop()
print(tmr.toString())