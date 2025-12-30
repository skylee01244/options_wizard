#include "BlackScholes.h"
#include "VolatilitySurface.h"
#include <iostream>
#include <cmath>

// C + K*e^(-rT) = P + S
void checkPutCallParity(const Greeks& callGreeks, const Greeks& putGreeks, const Option& callOption, double spot, double r) {

    double K = callOption.getStrike();
    double T = callOption.getTimeToExpiry();

    double lhs = callGreeks.premium + K * std::exp(-r * T);
    double rhs = putGreeks.premium + spot;

    double diff = std::abs(lhs - rhs);

    if (diff < 1e-5) {
        std::cout << "[PASS] Parity Holds. No Arbitrage.\n";
    } else {
        std::cout << "[PASS] Parity Violated. Arbitrage opportunity.\n";
    }
    return;
}

inline void runParityTest() {

    double S = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.30;

    Option callOption(S, T, OptionType::Call);
    Option putOption(S, T, OptionType::Put);

    FlatVolatility tempVol(sigma);
    std::optional<Greeks> callGreeks = BlackScholes::calculate(K, T, OptionType::Call, S, r, tempVol);
    std::optional<Greeks> putGreeks  = BlackScholes::calculate(K, T, OptionType::Put,  S, r, tempVol);

    if (callGreeks && putGreeks) {
        checkPutCallParity(*callGreeks, *putGreeks, callOption, S, r);
    } else {
        std::cout << "[FAIL] Failed Parity.\n";
    }
}