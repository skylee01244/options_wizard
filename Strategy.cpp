#include "Headers/Strategy.h"
#include <stdexcept>

Strategy::Strategy(std::string name)
    : name(std::move(name)) {}

void Strategy::addLeg(const Option& option, int quantity) {
    legs.push_back({option, quantity});
}

const std::vector<StrategyLeg>& Strategy::getLegs() const {
    return legs;
}

std::string Strategy::getName() const {
    return name;
}




Strategy Strategy::longCall(double K, double T) {
    Strategy s("Long Call");
    s.addLeg(Option(K, T, OptionType::Call), 1);
    return s;
}

Strategy Strategy::longPut(double K, double T) {
    Strategy s("Long Put");
    s.addLeg(Option(K, T, OptionType::Put), 1);
    return s;
}

Strategy Strategy::bullCallSpread(double K_low, double K_high, double T) {
    if (K_low >= K_high) throw std::invalid_argument("ERROR: bullCallSpread");

    Strategy s("Bull Call Spread");
    s.addLeg(Option(K_low, T, OptionType::Call), 1);
    s.addLeg(Option(K_high, T, OptionType::Call), -1);
    return s;
}

Strategy Strategy::straddle(double K, double T) {
    Strategy s("Long Straddle");
    s.addLeg(Option(K, T, OptionType::Call), 1);
    s.addLeg(Option(K, T, OptionType::Put), 1);
    return s;
}

Strategy Strategy::strangle(double K_low, double K_high, double T) {
    if (K_low >= K_high) throw std::invalid_argument("ERROR: strangle");

    Strategy s("Long Strangle");
    s.addLeg(Option(K_low, T, OptionType::Put), 1);
    s.addLeg(Option(K_high, T, OptionType::Call), 1);
    return s;
}

Strategy Strategy::bearPutSpread(double K_high, double K_low, double T) {
    if (K_low >= K_high) throw std::invalid_argument("ERROR: bearPutSpread");

    Strategy s("Bear Put Spread");
    s.addLeg(Option(K_high, T, OptionType::Put), 1);
    s.addLeg(Option(K_low, T, OptionType::Put), -1);
    return s;
}

Strategy Strategy::ironCondor(double K_put_long, double K_put_short, double K_call_short, double K_call_long, double T) {
    if (K_put_long >= K_put_short || K_put_short >= K_call_short || K_call_short >= K_call_long) {
        throw std::invalid_argument("ERROR: ironCondor");
    }

    Strategy s("Iron Condor");
    // Bull Put Spread Leg
    s.addLeg(Option(K_put_short, T, OptionType::Put), -1);
    s.addLeg(Option(K_put_long, T, OptionType::Put), 1);

    // Bear Call Spread Leg
    s.addLeg(Option(K_call_short, T, OptionType::Call), -1);
    s.addLeg(Option(K_call_long, T, OptionType::Call), 1);

    return s;
}