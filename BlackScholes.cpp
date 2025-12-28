#include "Headers/BlackScholes.h"
#include "Headers/Global.h"
#include <cmath>
#include <algorithm>
#include <numbers>
#include <optional>

double BlackScholes::normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * std::numbers::pi)) * std::exp(-0.5 * x * x);
}

double BlackScholes::normalCDF(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

std::optional<Greeks> BlackScholes::calculate(double K, double T, OptionType type, double S, double r, double sigma) {
    if (T <= 0 || S <= 0 || K <= 0 || sigma < 0) {
        return std::nullopt;
    }

    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    // pre-calculations
    double sqrtT = std::sqrt(T);
    double exp_rT = std::exp(-r * T);

    double pdf_d1 = normalPDF(d1);
    double cdf_d1 = normalCDF(d1);
    double cdf_d2 = normalCDF(d2);
    double cdf_neg_d1 = normalCDF(-d1);
    double cdf_neg_d2 = normalCDF(-d2);

    Greeks g{};

    g.gamma = pdf_d1 / (S * sigma * sqrtT);
    g.vega = S * pdf_d1 * sqrtT * 0.01; // change per 1% vol

    if (type == OptionType::Call) {
        g.premium = S * cdf_d1 - K * exp_rT * cdf_d2;
        g.delta = cdf_d1;
        g.rho = K * T * exp_rT * cdf_d2 * 0.01; // change per 1% vol
        g.theta = (double)(-(S * normalPDF(d1) * sigma) / (2 * sqrtT) - r * K * exp_rT * cdf_d2) / gbl::TRADING_DAYS;   // daily theta

    } else { // Put
        g.premium = K * exp_rT * cdf_neg_d2 - S * cdf_neg_d1;
        g.delta = cdf_d1 - 1.0;
        g.rho = -K * T * exp_rT * cdf_neg_d2 * 0.01; // change per 1% vol
        g.theta = (double)(-(S * normalPDF(d1) * sigma) / (2 * sqrtT) + r * K * exp_rT * cdf_neg_d2) / gbl::TRADING_DAYS;
    }

    return g;
}

std::optional<double> BlackScholes::calculateIV(const Option& option, double S, double marketPrice, double r) {
    constexpr int MAX_ITERATIONS = 100;
    constexpr double EPSILON = 1e-6;
    double sigma = 0.2;

    for (int i{} ; i < MAX_ITERATIONS; i++) {
        std::optional<Greeks> result = calculate(option.getStrike(), option.getTimeToExpiry(), option.getType(), S, r, sigma);
        if (!result) return std::nullopt;

        double price = result->premium;
        double vega = result->vega * 100.0;

        double diff = price - marketPrice;
        if (std::abs(diff) < EPSILON) {
            return sigma;
        }

        if (std::abs(vega) < 1e-8) {
            return std::nullopt;
        }

        sigma = sigma - (diff / vega);

        if (sigma <= 0.0) sigma = 1e-5;
    }

    // Bisection Method
    double low_v = 0.001;
    double high_v = 5.0;
    for (int i = 0; i < 30; ++i) {
        double mid = low_v + (high_v - low_v) / 2.0;
        std::optional<Greeks> greeks = calculate(option, S, r, mid);
        double price = greeks ? greeks->premium : 0.0;

        if (std::abs(price - marketPrice) < EPSILON) return mid;

        if (price < marketPrice) low_v = mid;
        else high_v = mid;
    }

    return (low_v + high_v) / 2.0;
}