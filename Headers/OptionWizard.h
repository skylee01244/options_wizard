#pragma once
#include <optional>
#include <string>
#include "Option.h"

struct result {
    std::string strategyName;
    double entryCost;
    double projectedValue;
    double profitPercent;
    double pop;
};


class OptionWizard {
private:
    static double getEstimatedPrice(const Option& i_option, double futureSpot, double futureTimeRemaining, double r, double sigma);

public:
    static result simulateStrategy(const Option& leg1, int quantity1, std::optional<Option> leg2, int quantity2, double currentPrice, double targetPrice, double daysToTarget, double r, double sigma);
};