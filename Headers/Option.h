#pragma once
#ifndef OPTIONS_PRICING_MODEL_OPTION_H
#define OPTIONS_PRICING_MODEL_OPTION_H

enum class OptionType {
    Call,
    Put
};

class Option {
private:
    double K;
    double T;
    OptionType type;

public:
    Option(double strike, double timeToMaturity, OptionType type);

    [[nodiscard]] double getStrike() const;
    [[nodiscard]] double getTimeToExpiry() const;
    [[nodiscard]] OptionType getType() const;
};


#endif //OPTIONS_PRICING_MODEL_OPTION_H
