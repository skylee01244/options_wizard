#include "Headers/OptionWizard.h"
#include "Headers/Greeks.h"
#include "Headers/BlackScholes.h"
#include "Headers/Global.h"
#include <optional>
#include <stdexcept>
#include <random>
#include <cmath>

double getEstimatedPrice(const Option& originalOpt, double futureSpot, double futureTimeRemaining, double r, double sigma) {
    Option option(originalOpt.getStrike(), futureTimeRemaining, originalOpt.getType());

    std::optional<Greeks> greeks = BlackScholes::calculate(option, futureSpot, r, sigma);
    return greeks ? greeks->premium : 0.0;
}

result OptionWizard::simulateStrategy(const Option &leg1, int quantity1, std::optional<Option> leg2, int quantity2, double current, double target, double daysToTarget, double r, double sigma) {

    std::optional<Greeks> greeks1 = BlackScholes::calculate(leg1, current, r, sigma);
    if(!greeks1) throw std::runtime_error("Error calculating greeks1");

    double totalCost = greeks1->premium * quantity1;

    if(leg2) {
        std::optional<Greeks> greeks2 = BlackScholes::calculate(*leg2, current, r, sigma);
        if (!greeks2) throw std::runtime_error("Error calculating greeks2");
        totalCost += greeks2->premium * quantity2;
    }

    // 2. Setup Simulation Constraints
    double timeToTarget = daysToTarget / static_cast<double>(gbl::TRADING_DAYS);
    double timeRemaining = leg1.getTimeToExpiry() - timeToTarget;

    if (timeRemaining < 0) timeRemaining = 0;


    int simulations = 100000;
    int profitablePaths = 0;
    static thread_local std::mt19937 gen(std::random_device{}());
    std::normal_distribution<> d(0, 1);

    double drift = (r - 0.5 * sigma * sigma) * timeToTarget;
    double vol = sigma * std::sqrt(timeToTarget);

    for(int i = {}; i < simulations; i++) {
        double Z = d(gen);
        double simulatedPrice = current * std::exp(drift + vol * Z);


        double val1 = 0.0;
        if (timeRemaining <= 0) {
            double intrinsic = 0;
            if(leg1.getType() == OptionType::Call)
                intrinsic = std::max(0.0, simulatedPrice - leg1.getStrike());
            if(leg1.getType() == OptionType::Put)
                intrinsic = std::max(0.0, leg1.getStrike() - simulatedPrice);
            val1 = intrinsic;
        } else {
            val1 = getEstimatedPrice(leg1, simulatedPrice, timeRemaining, r, sigma);
        }

        double val2 = 0.0;
        if (leg2) {
            if (timeRemaining <= 0) {
                double intrinsic = 0;
                if(leg2->getType() == OptionType:: Call)
                    intrinsic = std::max(0.0, simulatedPrice - leg2->getStrike());
                if(leg2->getType() == OptionType::Put)
                    intrinsic = std::max(0.0, leg2->getStrike() - simulatedPrice);
                val2 = intrinsic;
            } else {
                val2 = getEstimatedPrice(*leg2, simulatedPrice, timeRemaining, r, sigma);
            }
        }


        double pnl = (val1 * quantity1) + (val2 * quantity2) - totalCost;
        if (pnl > 0) {
            profitablePaths++;
        }
    }


    std::string name = leg2 ? "Two-Leg" : "Single Leg";

    double projectedVal1 = getEstimatedPrice(leg1, target, timeRemaining, r, sigma);
    double projectedVal2 = leg2 ? getEstimatedPrice(*leg2, target, timeRemaining, r, sigma) : 0.0;
    double totalProjectedValue = (projectedVal1 * quantity1) + (projectedVal2 * quantity2);

    double profitPercent = (totalCost != 0.0) ? ((totalProjectedValue - totalCost) / std::abs(totalCost)) * 100.0 : 0.0;
    double pop = static_cast<double>(profitablePaths) / simulations;

    return {
            name,
            totalCost,
            totalProjectedValue,
            profitPercent,
            pop // This is now "Probability of Profit at Target Date"
    };
}
