#pragma once
#include <optional>
#include <string>
#include "Option.h"
#include "Strategy.h"
#include "Greeks.h"
#include "VolatilitySurface.h"

struct result {
    std::string strategyName;
    double entryCost;
    double projectedValue;
    double profitPercent;
    double pop;
    Greeks netGreeks;
    double expectedValue;
};


class OptionWizard {
private:
    static double getEstimatedPrice(const Option& i_option, double futureSpot, double futureTimeRemaining, double r, const IVolatilitySurface& volSurface, double CurrentSpot);

public:
    static result simulateStrategy(const Strategy& strategy, double current, double target, double daysToTarget, double r, const IVolatilitySurface& volSurface, double mu, double sigma);
};