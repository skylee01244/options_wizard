#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include "../Headers/BlackScholes.h"
#include "../Headers/Option.h"

inline void runFiniteDifferenceTest() {

    double S = 100.0;
    double K = 100.0;
    double T = 1.0;
    double r = 0.05;
    double sigma = 0.2;
    double epsilon = 0.01;
    FlatVolatility tempVol(sigma);

    std::optional<Greeks> result = BlackScholes::calculate(K, T, OptionType::Call, S, r, tempVol);
    if (!result) {
        std::cerr << "[FAIL] BlackScholes returned nullopt" << std::endl;
        return;
    }

    std::optional<Greeks> up   = BlackScholes::calculate(K, T, OptionType::Call, S + epsilon, r, tempVol);
    std::optional<Greeks> down = BlackScholes::calculate(K, T, OptionType::Call, S - epsilon, r, tempVol);
    // (Price(S+h) - Price(S-h)) / 2h
    double numericalDelta = (up->premium - down->premium) / (2.0 * epsilon);
    double error = std::abs(result->delta - numericalDelta);

    if (error < 1e-4) {
        std::cout << "[PASS] Delta is correct." << "\n";
    } else {
        std::cout << "[FAIL] Delta is incorrect!" << "\n";
    }
}