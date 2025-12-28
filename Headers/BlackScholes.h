#pragma once
#include <optional>
#include "Greeks.h"
#include "Option.h"

class BlackScholes {
private:
    static double normalPDF(double x);
    static double normalCDF(double x);

public:
    [[nodiscard]] static std::optional<Greeks> calculate(double K, double T, OptionType type, double spotPrice, double riskFreeRate, double volatility);
    [[nodiscard]] static std::optional<double> calculatePremium(double K, double T, OptionType type, double S, double r, double sigma);
    [[nodiscard]] static std::optional<double> calculateIV( const Option& option, double spotPrice, double marketPrice, double riskFreeRate);
};