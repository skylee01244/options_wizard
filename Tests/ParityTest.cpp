#include "ParityTest.h"
#include <iostream>
#include <cmath>
#include <iomanip>

// C + K*e^(-rT) = P + S
void checkPutCallParity(const Greeks& callGreeks, const Greeks& putGreeks, const Option& callOption, double spot, double r) {

    double K = callOption.getStrike();
    double T = callOption.getTimeToExpiry();

    double lhs = callGreeks.premium + K * std::exp(-r * T);
    double rhs = putGreeks.premium + spot;

    double diff = std::abs(lhs - rhs);

    if (diff < 1e-5) {
        std::cout << "Parity Holds. No Arbitrage.\n";
    } else {
        std::cout << "Parity Violated. Arbitrage opportunity.\n";
    }
    return;
}