#include "Headers/OptionWizard.h"
#include "Headers/Greeks.h"
#include "Headers/BlackScholes.h"
#include "Headers/Strategy.h"
#include "Headers/Global.h"
#include <optional>
#include <stdexcept>
#include <random>
#include <cmath>
#include <future>
#include <thread>

double OptionWizard::getEstimatedPrice(const Option& i_option, double futureSpot, double futureTimeRemaining, double r, double sigma) {
    double K = i_option.getStrike();
    double T = futureTimeRemaining;
    OptionType type = i_option.getType();
    double skewedSigma = BlackScholes::getImpliedVol(K, futureSpot, sigma, T);

    std::optional<double> premium = BlackScholes::calculatePremium(K, T, type, futureSpot, r, skewedSigma);
    return premium.value_or(0.0);
}

result OptionWizard::simulateStrategy(const Strategy& strategy, double current, double target, double daysToTarget, double r, double sigma, double mu) {

    double totalCost = 0.0;
    Greeks strategyGreeks = {};
    const std::vector<StrategyLeg>& legs = strategy.getLegs();

    for(const StrategyLeg& leg : legs) {
        double K = leg.option.getStrike();
        double T = leg.option.getTimeToExpiry();
        OptionType type = leg.option.getType();
        std::optional<Greeks> g = BlackScholes::calculate(K, T, type, current, r, sigma);
        if(!g) throw std::runtime_error("Error pricing leg");
        totalCost += g->premium * leg.quantity;

        strategyGreeks.delta += g->delta * leg.quantity;
        strategyGreeks.gamma += g->gamma * leg.quantity;
        strategyGreeks.theta += g->theta * leg.quantity;
        strategyGreeks.vega  += g->vega  * leg.quantity;
    }

    double timeToTarget = daysToTarget / gbl::TRADING_DAYS;
    if(legs.empty()) throw std::runtime_error("Strategy has no legs");
    double timeRemaining = legs[0].option.getTimeToExpiry() - timeToTarget;
    if(timeRemaining < 0) timeRemaining = 0;

    int simulations = 100000;
    int threadCount = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;
    int simsPerThread = simulations / threadCount;
    std::vector<std::future<int>> futures;

    double drift = (mu - 0.5 * sigma * sigma) * timeToTarget;
    double vol = sigma * std::sqrt(timeToTarget);

    auto worker = [&](int iterations) -> int {
        static thread_local std::mt19937 gen(std::random_device{}());
        std::normal_distribution<> d(0, 1);
        int threadProfitablePaths = 0;

        for(int i{}; i < iterations; i++) {
            double Z = d(gen);
            double simulatedPrice = current * std::exp(drift + vol * Z);

            double pathValue = 0.0;

            for(const StrategyLeg& leg : legs) {
                double legValue = 0.0;

                if(timeRemaining <= 0) {
                    if(leg.option.getType() == OptionType::Call)
                        legValue = std::max(0.0, simulatedPrice - leg.option.getStrike());
                    if(leg.option.getType() == OptionType::Put)
                        legValue = std::max(0.0, leg.option.getStrike() - simulatedPrice);
                } else {
                    legValue = getEstimatedPrice(leg.option, simulatedPrice, timeRemaining, r, sigma);
                }

                pathValue += legValue * leg.quantity;
            }

            double pnl = pathValue - totalCost;
            if(pnl > 0) threadProfitablePaths++;
        }
        return threadProfitablePaths;
    };

    for(int i = 0 ; i < threadCount ; i++) {
        int count = (i == threadCount - 1) ? (simulations - (i * simsPerThread)) : simsPerThread;
        futures.push_back(std::async(std::launch::async, worker, count));
    }

    int totalProfitablePaths = 0;
    for(std::future<int>& f : futures) totalProfitablePaths += f.get();

    double totalProjectedValue = 0.0;
    for(const StrategyLeg& leg : legs) {
        double val = getEstimatedPrice(leg.option, target, timeRemaining, r, sigma);
        totalProjectedValue += val * leg.quantity;
    }

    double profitPercent = (totalCost != 0.0) ? ((totalProjectedValue - totalCost) / std::abs(totalCost)) * 100.0 : 0.0;
    double pop = static_cast<double>(totalProfitablePaths) / simulations;

    return {
            strategy.getName(),
            totalCost,
            totalProjectedValue,
            profitPercent,
            pop,
            strategyGreeks
    };
}