#!/usr/bin/env python3

import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), "../lib"))

import mymodule # type: ignore

price = mymodule.european_call_price(100, 100, 0.01, 0.2, 30)
print("European Call Option Price:", price)
