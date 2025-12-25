#include "Headers/Option.h"

Option::Option(double strike, double timeToMaturity, OptionType type)
        : K(strike), T(timeToMaturity), type(type) {}

double Option::getStrike() const { return K; }
double Option::getTimeToExpiry() const { return T; }
OptionType Option::getType() const { return type; }