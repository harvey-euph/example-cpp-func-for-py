import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), "../lib"))

import mymodule # type: ignore

print("2 + 3 =", mymodule.add(2, 3))
