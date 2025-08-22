#include <pybind11/pybind11.h>
#include <pybind11/stl.h>         // vector/map/tuple/optional 轉換
#include <pybind11/functional.h>  // std::function <-> Python callable
#include <pybind11/operators.h>   // py::self 運算子
#include <pybind11/chrono.h>      // chrono <-> datetime
#include <pybind11/numpy.h>       // numpy array_t
#include <cmath>
#include <sstream>
#include <numeric>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <chrono>

namespace py = pybind11;

//--------------------------------------
// 自訂例外
//--------------------------------------
struct MyError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

//--------------------------------------
// enum
//--------------------------------------
enum class Color : int { Red = 1, Green = 2, Blue = 4 };

//--------------------------------------
// 簡單的 struct/class：Point（屬性/方法/運算子/靜態工廠）
//--------------------------------------
struct Point {
    double x, y;
    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
    void translate(double dx, double dy) { x += dx; y += dy; }
    double distance_to(const Point& o) const {
        double dx = x - o.x, dy = y - o.y;
        return std::sqrt(dx*dx + dy*dy);
    }
    static Point origin() { return Point(0, 0); }
    std::string repr() const {
        std::ostringstream oss;
        oss << "Point(" << x << ", " << y << ")";
        return oss.str();
    }
};

inline Point operator+(const Point& a, const Point& b) {
    return Point(a.x + b.x, a.y + b.y);
}

//--------------------------------------
// 多型/繼承 + trampoline（Python 可 override）
//--------------------------------------
struct Shape {
    virtual ~Shape() = default;
    virtual double area() const = 0;                 // 純虛
    virtual std::string name() const { return "Shape"; }
};

// Trampoline：讓 Python 子類可以覆寫 C++ 虛擬函式
struct PyShape : Shape {
    using Shape::Shape;
    double area() const override {
        PYBIND11_OVERRIDE_PURE(double, Shape, area, );
    }
    std::string name() const override {
        PYBIND11_OVERRIDE(std::string, Shape, name, );
    }
};

struct Rectangle : Shape {
    double w, h;
    Rectangle(double w_, double h_) : w(w_), h(h_) {}
    double area() const override { return w * h; }
    std::string name() const override { return "Rectangle"; }
};

struct Circle : Shape {
    double r;
    Circle(double r_) : r(r_) {}
    double area() const override {
        constexpr double PI = 3.14159265358979323846;
        return PI * r * r;
    }
    std::string name() const override { return "Circle"; }
};

//--------------------------------------
// 可迭代的容器（用 make_iterator 暴露 __iter__）
//--------------------------------------
struct IntBox {
    std::vector<int> data;
    void add(int v) { data.push_back(v); }
    std::size_t size() const { return data.size(); }
};

//--------------------------------------
// Free functions（含 overload / kwargs / callable / STL / optional / chrono / numpy / GIL）
//--------------------------------------
int add(int a, int b) { return a + b; }

double area_circle(double radius) {
    if (radius < 0) throw MyError("radius must be non-negative");
    constexpr double PI = 3.14159265358979323846;
    return PI * radius * radius;
}
double area_rect(double w, double h) {
    if (w < 0 || h < 0) throw MyError("width/height must be non-negative");
    return w * h;
}

// *args/**kwargs
py::dict echo(py::args args, py::kwargs kwargs) {
    py::dict d;
    d["args"] = args;
    d["kwargs"] = kwargs;
    return d;
}

// 可傳 Python 函式進來
py::object apply_twice(const std::function<py::object(py::object)>& f, py::object x) {
    return f(f(x));
}

// STL 容器自動轉換
int sum_vec(const std::vector<int>& v) {
    return std::accumulate(v.begin(), v.end(), 0);
}

// optional
std::optional<int> maybe_add_one(std::optional<int> v) {
    if (v) return *v + 1;
    return std::nullopt;
}

// 例外
std::string might_fail(bool fail) {
    if (fail) throw MyError("Something went wrong");
    return "ok";
}

// 長任務：釋放 GIL
long long long_task(int n) {
    py::gil_scoped_release release;
    long long s = 0;
    for (int i = 0; i < n; ++i) s += i;
    return s;
}

// chrono
std::chrono::system_clock::time_point now() {
    return std::chrono::system_clock::now();
}

// Numpy：sum 1D double array（forcecast 會嘗試轉成連續 double 1D）
double sum_array(py::array_t<double, py::array::c_style | py::array::forcecast> arr) {
    auto r = arr.unchecked<1>();
    double s = 0.0;
    for (ssize_t i = 0; i < r.shape(0); ++i) s += r(i);
    return s;
}

// 多型輸入：計算總面積
double total_area(const std::vector<std::shared_ptr<Shape>>& shapes) {
    double s = 0.0;
    for (auto& sh : shapes) s += sh->area();
    return s;
}

// 靜態成員示例
struct Counter {
    static int instances;
    Counter() { ++instances; }
    ~Counter() { --instances; }
    static int get_instances() { return instances; }
};
int Counter::instances = 0;

//--------------------------------------
// 模組
//--------------------------------------
PYBIND11_MODULE(MYLIB_MODULE_NAME, m)
{
    m.doc() = "Everything-you-should-know pybind11 examples";

    // 自訂例外
    py::register_exception<MyError>(m, "MyError");

    // 模組層變數（常數/字串/可改成 object）
    m.attr("PI") = 3.14159265358979323846;
    m.attr("APP_NAME") = "mylib";

    // enum（可再加 .export_values() 與 .arithmetic() 若要位元運算）
    py::enum_<Color>(m, "Color")
        .value("Red", Color::Red)
        .value("Green", Color::Green)
        .value("Blue", Color::Blue)
        .export_values();

    // Point
    py::class_<Point>(m, "Point")
        .def(py::init<>(), "Default ctor")
        .def(py::init<double,double>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def("translate", &Point::translate, py::arg("dx"), py::arg("dy"))
        .def("distance_to", &Point::distance_to, py::arg("other"))
        .def_static("origin", &Point::origin)
        .def("__repr__", &Point::repr)
        .def(py::self + py::self);

    // 可迭代容器 IntBox
    py::class_<IntBox>(m, "IntBox")
        .def(py::init<>())
        .def("add", &IntBox::add)
        .def_property_readonly("length", &IntBox::size)
        .def("__iter__", [](IntBox &self) {
            return py::make_iterator(self.data.begin(), self.data.end());
        }, py::keep_alive<0, 1>()); // 迭代器存活期間綁 self

    // 繼承/多型 + trampoline
    py::class_<Shape, PyShape, std::shared_ptr<Shape>>(m, "Shape")
        .def(py::init<>())
        .def("area", &Shape::area)
        .def("name", &Shape::name);

    py::class_<Rectangle, Shape, std::shared_ptr<Rectangle>>(m, "Rectangle")
        .def(py::init<double,double>(), py::arg("w"), py::arg("h"))
        .def_readwrite("w", &Rectangle::w)
        .def_readwrite("h", &Rectangle::h);

    py::class_<Circle, Shape, std::shared_ptr<Circle>>(m, "Circle")
        .def(py::init<double>(), py::arg("r"))
        .def_readwrite("r", &Circle::r);

    // 靜態成員
    py::class_<Counter>(m, "Counter")
        .def(py::init<>())
        .def_static("get_instances", &Counter::get_instances)
        .def_readonly_static("instances", &Counter::instances);

    // free functions
    m.def("add", &add, py::arg("a"), py::arg("b"), "Add two integers");
    m.def("area", py::overload_cast<double>(&area_circle), py::arg("radius"),
          "Area of a circle");
    m.def("area", py::overload_cast<double,double>(&area_rect),
          py::arg("width"), py::arg("height"), "Area of a rectangle");

    m.def("echo", &echo, "Return the args/kwargs as a dict");
    m.def("apply_twice", &apply_twice, py::arg("func"), py::arg("x"));
    m.def("sum_vec", &sum_vec, py::arg("values"));
    m.def("maybe_add_one", &maybe_add_one, py::arg("v") = std::nullopt);
    m.def("might_fail", &might_fail, py::arg("fail") = false);
    m.def("long_task", &long_task, py::arg("n"));
    m.def("now", &now);
    m.def("sum_array", &sum_array, py::arg("arr"));
    m.def("total_area", &total_area, py::arg("shapes"));

    // submodule
    auto util = m.def_submodule("util", "Utility helpers");
    util.def("mean", [](const std::vector<double>& v) {
        if (v.empty()) throw MyError("mean() of empty list");
        double s = std::accumulate(v.begin(), v.end(), 0.0);
        return s / v.size();
    }, py::arg("values"));
}
