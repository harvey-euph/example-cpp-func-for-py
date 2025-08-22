#include <pybind11/pybind11.h>
#include <ql/quantlib.hpp>

namespace py = pybind11;

double european_call_price(double spot, double strike, double rate, double vol, int days)
{
    using namespace QuantLib;

    Date today = Date::todaysDate();
    Settings::instance().evaluationDate() = today;

    Date maturity = today + days;

    Handle<Quote> spotHandle(boost::make_shared<SimpleQuote>(spot));
    Handle<YieldTermStructure> riskFreeHandle(
        boost::make_shared<FlatForward>(today, rate, Actual365Fixed())
    );
    Handle<YieldTermStructure> dividendYield(
        boost::make_shared<FlatForward>(today, 0.0, Actual365Fixed())
    );
    Handle<BlackVolTermStructure> volHandle(
        boost::make_shared<BlackConstantVol>(today, TARGET(), vol, Actual365Fixed())
    );

    auto payoff = boost::make_shared<PlainVanillaPayoff>(Option::Call, strike);
    auto exercise = boost::make_shared<EuropeanExercise>(maturity);

    VanillaOption option(payoff, exercise);

    auto bsm = boost::make_shared<BlackScholesMertonProcess>(
        spotHandle, dividendYield, riskFreeHandle, volHandle
    );

    option.setPricingEngine(boost::make_shared<AnalyticEuropeanEngine>(bsm));

    return option.NPV();

}

PYBIND11_MODULE(mymodule, m)
{
    m.doc() = "QuantLib Python module via pybind11";
    m.def(
        "european_call_price", &european_call_price, "Calculate European call option price",
        py::arg("spot"), py::arg("strike"), py::arg("rate"), py::arg("vol"), py::arg("days")
    );
}
