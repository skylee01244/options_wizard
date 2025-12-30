#include <iostream>
#include <optional>
#include <vector>
#include <limits>
#include <cstring>
#include <memory>
#include "Headers/Global.h"
#include "Headers/Option.h"
#include "Headers/BlackScholes.h"
#include "Headers/OptionWizard.h"
#include "Headers/Strategy.h"
#include "Headers/VolatilitySurface.h"
#include "Tests/ParityTest.h"
#include "Tests/FiniteDifferenceTest.h"
#include "Tests/MonteCarloConvergenceTest.h"
#include "UserInterface.h"


int main(int argc, char* argv[]) {

    // Tests
    if (argc > 1 && std::strcmp(argv[1], "--test") == 0) {
        // Finite Difference Test
        runFiniteDifferenceTest();
        runParityTest();
        runMonteCarloConvergenceTest();
        return 0;
    }

    double i_current_stock_price = UI::getDouble(">> Current Stock Price: ");
    double i_target_price = UI::getDouble(">> Target Stock Price: ");
    double i_target_date = UI::getDouble(">> Target date: ");
    double i_expiry_days = UI::getExpiry(">> Strategy Days to Expiration: ",i_target_date);
    double i_expected_return = UI::getDouble(">> Annual Expected Return % (default 8): ");
    i_expected_return = i_expected_return / 100;
    double r = 0.05;    // Risk-Free Rate


    Option atmOption(i_current_stock_price, (i_target_date / gbl::TRADING_DAYS), OptionType::Call);

    double assumed_vol = 0.30;
    FlatVolatility tempVol(assumed_vol);
    std::optional<Greeks> suggestion = BlackScholes::calculate(atmOption.getStrike(), atmOption.getTimeToExpiry(), atmOption.getType(), i_current_stock_price, r, tempVol);
    if(suggestion) {
        std::cout << ">> Suggested ATM Price (assuming 30% Vol): $" << suggestion->premium << std::endl;
    }

    double i_market_price = UI::getDouble(">> ATM Market Price: ");


    std::optional<double> IV = BlackScholes::calculateIV(atmOption, i_current_stock_price, i_market_price, r);

    double sigma = IV.value_or(0.30);
    std::cout << "IV is : " << sigma << std::endl;
    std::unique_ptr<ParametricVolatility> volModel = std::make_unique<ParametricVolatility>(sigma, -0.2, 1.0);
    // busy day: slope:-0.5, convexity:2.5
    // quiet day: slope:-0.05, convexity:0.3



    std::vector<Strategy> strategies;

    // Bull
    strategies.push_back(Strategy::longCall(i_current_stock_price, (i_expiry_days / gbl::TRADING_DAYS)));
    strategies.push_back(Strategy::bullCallSpread(i_current_stock_price, i_current_stock_price * 1.10, (i_expiry_days / gbl::TRADING_DAYS)));
    // Bear
    strategies.push_back(Strategy::longPut(i_current_stock_price, (i_expiry_days / gbl::TRADING_DAYS)));
    strategies.push_back(Strategy::bearPutSpread(i_current_stock_price, i_current_stock_price * 0.90, (i_expiry_days / gbl::TRADING_DAYS)));
    // Volatility
    strategies.push_back(Strategy::straddle(i_current_stock_price, (i_expiry_days / gbl::TRADING_DAYS)));
    // Structure: Long 90% Put / Short 95% Put / Short 105% Call / Long 110% Call
    strategies.push_back(Strategy::ironCondor(i_current_stock_price * 0.90, i_current_stock_price * 0.95, i_current_stock_price * 1.05, i_current_stock_price * 1.10, (i_expiry_days / gbl::TRADING_DAYS)));



    std::vector<result> results;

    for(const Strategy& strat : strategies) {
        try {
            result res = OptionWizard::simulateStrategy(strat, i_current_stock_price, i_target_price, i_target_date, r, *volModel, i_expected_return, sigma);
            results.push_back(res);
        } catch (const std::exception& e) {
            std::cerr << "Error simulating " << strat.getName() << ": " << e.what() << std::endl;
        }
    }

    UI::printTable(results);

    return 0;
}