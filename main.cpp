#include <iostream>
#include <optional>
#include "Headers/Option.h"
#include "Headers/BlackScholes.h"
#include "Tests/ParityTest.h"

int main() {

    double S = 100.0;   // Initial stock price
    double r = 0.05;    // Risk-free Rate
    double sigma = 0.2; // Volatility

    // Contract Inputs
    Option callOption(100.0, 1.0, OptionType::Call);
    Option putOption(100.0, 1.0, OptionType::Put);

    // Calculation
    std::optional<Greeks> callGreeks = BlackScholes::calculate(callOption, S, r, sigma);
    std::optional<Greeks> putGreeks = BlackScholes::calculate(putOption, S, r, sigma);

    if(callGreeks) {
        std::cout << "European Call Option Price: " << callGreeks->premium
                  << ", Delta: " << callGreeks->delta
                  << ", Gamma: " << callGreeks->gamma
                  << ", Vega: " << callGreeks->vega
                  << ", Theta: " << callGreeks->theta
                  << ", Rho: " << callGreeks->rho << std::endl;
    }

    if(putGreeks) {
        std::cout << "European Put Option Price: " << putGreeks->premium
                  << ", Delta: " << putGreeks->delta
                  << ", Gamma: " << putGreeks->gamma
                  << ", Vega: " << putGreeks->vega
                  << ", Theta: " << putGreeks->theta
                  << ", Rho: " << putGreeks->rho << std::endl;
    }

    if (callGreeks && putGreeks) {
        checkPutCallParity(*callGreeks, *putGreeks, callOption, S, r);
    }

    return 0;
}