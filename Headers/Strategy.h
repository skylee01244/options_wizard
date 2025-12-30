#pragma once
#include <vector>
#include <string>
#include "Option.h"

struct StrategyLeg {
    Option option;
    int quantity; // +1 (Long), -1 (Short)
};

class Strategy {
private:
    std::string name;
    std::vector<StrategyLeg> legs;

public:
    explicit Strategy(std::string name);

    void addLeg(const Option& option, int quantity);
    [[nodiscard]] const std::vector<StrategyLeg>& getLegs() const;
    [[nodiscard]] std::string getName() const;


    static Strategy longCall(double K, double T);
    static Strategy longPut(double K, double T);

    static Strategy bullCallSpread(double K_lower, double K_higher, double T);
    static Strategy straddle(double K, double T);
    static Strategy strangle(double K_lower, double K_higher, double T);
    static Strategy bearPutSpread(double K_higher, double K_lower, double T);
    static Strategy ironCondor(double K_put_long, double K_put_short, double K_call_short, double K_call_long, double T);
};