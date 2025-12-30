#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include "../Headers/BlackScholes.h"
#include "../Headers/OptionWizard.h"
#include "../Headers/Strategy.h"
#include "../Headers/VolatilitySurface.h"

inline void runMonteCarloConvergenceTest() {

    double S = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    double expected_return = 0.05;
    double targetDateDays = T * gbl::TRADING_DAYS;

    FlatVolatility flatVol(sigma);
    std::optional<Greeks> bsResult = BlackScholes::calculate(K, T, OptionType::Call, S, r, flatVol);

    if (!bsResult) {
        std::cerr << "[FAIL] BlackScholes calculation failed." << std::endl;
        return;
    }
    double bsPrice = bsResult->premium;

    Strategy strat = Strategy::longCall(K, T);
    ParametricVolatility volModel(sigma, 0.0, 0.0);

    result mcResult = OptionWizard::simulateStrategy(strat, S, S, targetDateDays, r, volModel, expected_return, sigma);

    double mcFutureValue = mcResult.expectedValue;
    double mcPresentValue = mcFutureValue * std::exp(-r * T);

    double diff = std::abs(bsPrice - mcPresentValue);
    double errorPct = (diff / bsPrice) * 100.0;


    if (errorPct < 1.0) {
        std::cout << "[PASS] Monte Carlo converges" << std::endl;
    } else {
        std::cout << "[FAIL] Divergence detected" << std::endl;
    }
}