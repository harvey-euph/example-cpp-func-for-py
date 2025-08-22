#!/usr/bin/env python3

import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), "../lib"))

import mylib # type: ignore
import numpy as np # type: ignore

print("=== Module attributes / Enum ===")
print("APP_NAME:", mylib.APP_NAME, "(expected: 'mylib')")
print("PI:", mylib.PI, "(expected: 3.141592653589793)")
print("Enum values:", mylib.Color.Red, mylib.Color.Green, mylib.Color.Blue,
      "\n(expected: Color.Red Color.Green Color.Blue)")
print("Enum as int:", int(mylib.Color.Red), "(expected: 1)")


print("\n=== Free functions / Overload / Args & Kwargs ===")
print("add(2,5):", mylib.add(2, 5), "(expected: 7)")
print("area(3.0):", mylib.area(3.0), "(expected: ~28.2743)")
print("area(4.0,2.0):", mylib.area(4.0, 2.0), "(expected: 8.0)")
print("echo(1,2,a=10,b='x'):", mylib.echo(1,2,a=10,b="x"),
      "\n(expected: {'args': (1,2), 'kwargs': {'a': 10, 'b': 'x'}})")


print("\n=== Exception ===")
try:
    mylib.might_fail(True)
except mylib.MyError as e:
    print("Caught MyError:", str(e), "(expected: 'Something went wrong')")


print("\n=== Callable from Python to C++ ===")
print("apply_twice(lambda x: x*2, 5):", mylib.apply_twice(lambda x: x*2, 5),
      "(expected: 20)")


print("\n=== STL Containers & Optional ===")
print("sum_vec([1,2,3,4]):", mylib.sum_vec([1,2,3,4]), "(expected: 10)")
print("maybe_add_one(None):", mylib.maybe_add_one(None), "(expected: None)")
print("maybe_add_one(41):", mylib.maybe_add_one(41), "(expected: 42)")


print("\n=== Numpy Array ===")
arr = np.arange(5, dtype=np.float64)
print("sum_array(np.arange(5)):",
      mylib.sum_array(arr), "(expected: 10.0)")


print("\n=== chrono <-> datetime ===")
print("now():", mylib.now(), "(expected: datetime around 'now')")


print("\n=== GIL Release Demo ===")
print("long_task(1_000_000):",
      mylib.long_task(1_000_000), "(expected: 499999500000)")


print("\n=== Class Point (properties, methods, operator) ===")
p = mylib.Point(3, 4)
q = mylib.Point.origin()
print("p:", p, "(expected: Point(3, 4))")
print("q:", q, "(expected: Point(0, 0))")
print("p.distance_to(q):", p.distance_to(q), "(expected: 5.0)")
p.translate(1, -2)
print("p after translate:", p, "(expected: Point(4, 2))")
print("p + Point(1,1):", p + mylib.Point(1,1), "(expected: Point(5, 3))")


print("\n=== Custom Iterable IntBox ===")
box = mylib.IntBox()
for i in range(5):
    box.add(i)
print("box.length:", box.length, "(expected: 5)")
print("list(box):", list(box), "(expected: [0,1,2,3,4])")


print("\n=== Polymorphism / Inheritance ===")
r = mylib.Rectangle(3, 2)
c = mylib.Circle(1.5)
print("Rectangle area/name:", r.area(), r.name(),
      "(expected: 6.0, 'Rectangle')")
print("Circle area/name:", c.area(), c.name(),
      "(expected: ~7.0686, 'Circle')")

class Triangle(mylib.Shape):
    def __init__(self, base, height):
        super().__init__()
        self.base = base
        self.height = height
    def area(self): return 0.5 * self.base * self.height
    def name(self): return "Triangle"

t = Triangle(3, 4)
print("Triangle area/name:", t.area(), t.name(),
      "(expected: 6.0, 'Triangle')")
print("total_area([r,c,t]):",
      mylib.total_area([r,c,t]),
      "(expected: ~19.0686)")


print("\n=== Static members ===")
before = mylib.Counter.get_instances()
a = mylib.Counter()
b = mylib.Counter()
after = mylib.Counter.get_instances()
print("instances before:", before, "(expected: 0)")
print("instances after:", after, "(expected: 2)")
print("Counter.instances:", mylib.Counter.instances, "(expected: 2)")


print("\n=== Submodule util ===")
print("mylib.util.mean([1,2,3]):",
      mylib.util.mean([1,2,3]), "(expected: 2.0)")
