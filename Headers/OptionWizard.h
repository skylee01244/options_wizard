#pragma once
#include <optional>
#include <string>
#include "Option.h"
#include "Strategy.h"
#include "Greeks.h"

struct result {
    std::string strategyName;
    double entryCost;
    double projectedValue;
    double profitPercent;
    double pop;
    Greeks netGreeks;
};


class OptionWizard {
private:
    static double getEstimatedPrice(const Option& i_option, double futureSpot, double futureTimeRemaining, double r, double sigma);

public:
    static result simulateStrategy(const Strategy& strategy, double current, double target, double daysToTarget, double r, double sigma, double mu);
};