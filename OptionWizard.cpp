#include "Headers/OptionWizard.h"
#include "Headers/Greeks.h"
#include "Headers/BlackScholes.h"
#include "Headers/Strategy.h"
#include "Headers/Global.h"
#include "Headers/VolatilitySurface.h"
#include <optional>
#include <stdexcept>
#include <random>
#include <cmath>
#include <future>
#include <thread>

double OptionWizard::getEstimatedPrice(const Option& i_option, double futureSpot, double futureTimeRemaining, double r, const IVolatilitySurface& volSurface, double currentSpot) {
    double K = i_option.getStrike();
    double T = futureTimeRemaining;
    OptionType type = i_option.getType();
    double sigma = volSurface.getVol(K,T, currentSpot);

    std::optional<double> premium = BlackScholes::calculatePremium(K, T, type, futureSpot, r, sigma);
    return premium.value_or(0.0);
}

result OptionWizard::simulateStrategy(const Strategy& strategy, double current, double target, double daysToTarget, double r, const IVolatilitySurface& volSurface, double mu) {

    double totalCost = 0.0;
    Greeks strategyGreeks = {};
    const std::vector<StrategyLeg>& legs = strategy.getLegs();

    for(const StrategyLeg& leg : legs) {
        double K = leg.option.getStrike();
        double T = leg.option.getTimeToExpiry();
        OptionType type = leg.option.getType();
        std::optional<Greeks> g = BlackScholes::calculate(K, T, type, current, r, volSurface);
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
    std::vector<std::future<std::pair<int, double>>> futures; // profitableCount, totalValueSum

    double marketVol = 0.20;
    double drift = (mu - 0.5 * marketVol * marketVol) * timeToTarget;
    double vol = marketVol * std::sqrt(timeToTarget);

    auto worker = [&](int iterations) -> std::pair<int, double> {
        static thread_local std::mt19937 gen = [](){
            std::random_device rd;
            auto now = std::chrono::high_resolution_clock::now();
            std::seed_seq ss{
                    static_cast<unsigned long long>(rd()),
                    static_cast<unsigned long long>(now.time_since_epoch().count()),
                    static_cast<unsigned long long>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
            };
            return std::mt19937(ss);
        }();

        std::normal_distribution<> d(0, 1);
        int ProfitablePaths = 0;
        double threadSum = 0.0;

        auto simulatePath = [&](double simulatedPrice) {
            double pathValue = 0.0;

            for(const StrategyLeg& leg : legs) {
                double legValue = 0.0;

                if(timeRemaining <= 0) {
                    if(leg.option.getType() == OptionType::Call)
                        legValue = std::max(0.0, simulatedPrice - leg.option.getStrike());
                    if(leg.option.getType() == OptionType::Put)
                        legValue = std::max(0.0, leg.option.getStrike() - simulatedPrice);
                } else {
                    legValue = getEstimatedPrice(leg.option, simulatedPrice, timeRemaining, r, volSurface, simulatedPrice);
                }
                pathValue += legValue * leg.quantity;
            }
            return pathValue;
        };

        for (int i = 0; i < iterations / 2; ++i) {
            double Z = d(gen);
            double pnl{};

            double price1 = current * std::exp(drift + vol * Z);
            double val1 = simulatePath(price1);
            threadSum += val1;
            pnl = val1 - totalCost;
            if (pnl > 0) ProfitablePaths++;

            double price2 = current * std::exp(drift + vol * (-Z));
            double val2 = simulatePath(price2);
            threadSum += val2;
            pnl = val2 - totalCost;
            if (pnl > 0) ProfitablePaths++;
        }

        return {ProfitablePaths, threadSum};
    };

    for(int i = 0 ; i < threadCount ; i++) {
        int count = (i == threadCount - 1) ? (simulations - (i * simsPerThread)) : simsPerThread;
        futures.push_back(std::async(std::launch::async, worker, count));
    }

    int totalProfitablePaths = 0;
    double grandTotalValue = 0.0;

    for(std::future<std::pair<int,double>>& f : futures) {
        auto result = f.get();
        totalProfitablePaths += result.first;
        grandTotalValue += result.second;
    }

    double totalProjectedValue = 0.0;

    for(const StrategyLeg& leg : legs) {
        double val = getEstimatedPrice(leg.option, target, timeRemaining, r, volSurface, target);
        totalProjectedValue += val * leg.quantity;
    }

    double profitPercent = (totalCost != 0.0) ? ((totalProjectedValue - totalCost) / std::abs(totalCost)) * 100.0 : 0.0;
    double pop = static_cast<double>(totalProfitablePaths) / simulations;
    double expectedValue = grandTotalValue / simulations;

    return {
            strategy.getName(),
            totalCost,
            totalProjectedValue,
            profitPercent,
            pop,
            strategyGreeks,
            expectedValue
    };
}