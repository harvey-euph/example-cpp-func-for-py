#include <pybind11/pybind11.h>

namespace py = pybind11;

// 一個簡單的 C++ 函數
int add(int a, int b) {
    return a + b;
}

// 模組定義
PYBIND11_MODULE(mymodule, m) {
    m.doc() = "Example pybind11 C++ module"; // 模組說明
    m.def("add", &add, "A function that adds two numbers");
}
