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

Strategy Strategy::bullCallSpread(double K_lower, double K_higher, double T) {
    if (K_lower >= K_higher) throw std::invalid_argument("ERROR: bullCallSpread");

    Strategy s("Bull Call Spread");
    s.addLeg(Option(K_lower, T, OptionType::Call), 1);
    s.addLeg(Option(K_higher, T, OptionType::Call), -1);
    return s;
}

Strategy Strategy::straddle(double K, double T) {
    Strategy s("Long Straddle");
    s.addLeg(Option(K, T, OptionType::Call), 1);
    s.addLeg(Option(K, T, OptionType::Put), 1);
    return s;
}

Strategy Strategy::strangle(double K_lower, double K_higher, double T) {
    if (K_lower >= K_higher) throw std::invalid_argument("ERROR: strangle");

    Strategy s("Long Strangle");
    s.addLeg(Option(K_lower, T, OptionType::Put), 1);
    s.addLeg(Option(K_higher, T, OptionType::Call), 1);
    return s;
}