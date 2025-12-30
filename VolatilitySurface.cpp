#include "Headers/VolatilitySurface.h"
#include <cmath>
#include <algorithm>

ParametricVolatility::ParametricVolatility(double atmVol, double slope, double convexity)
        : atmVol_(atmVol), slope_(slope), convexity_(convexity) {}

double ParametricVolatility::getVol(double strike, double timeToExpiry, double spot) const {
    if (spot <= 0.0 || strike <= 0.0 || timeToExpiry <= 0.0)
        return atmVol_;

    const double moneyness = std::log(strike / spot);
    const double smile = slope_ * moneyness + convexity_ * moneyness * moneyness;

    static constexpr double MinTime = 0.10;
    const double timeFactor = (timeToExpiry >= MinTime) ? 1.0 / std::sqrt(timeToExpiry) : 1.0;

    const double vol = atmVol_ + smile * timeFactor;

    return std::max(0.01, vol); // no negative vol
}
