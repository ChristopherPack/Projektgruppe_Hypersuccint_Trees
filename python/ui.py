import ctypes as ct
import os.path

pht_lib = ct.CDLL(os.path.join(os.path.curdir,"pht_lib.dll"))

pht_lib.hello()