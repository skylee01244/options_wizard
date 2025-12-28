#pragma once

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
