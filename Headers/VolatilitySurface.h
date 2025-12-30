#pragma once
#include <cmath>
#include <algorithm>

class IVolatilitySurface {
public:
    virtual ~IVolatilitySurface() = default;

    [[nodiscard]] virtual double getVol(double strike, double timeToExpiry, double spot) const = 0;
};

// For unit tests and basic Black-Scholes assumptions
class FlatVolatility : public IVolatilitySurface {
    double sigma_;

public:
    explicit FlatVolatility(double sigma) : sigma_(sigma) {}

    [[nodiscard]] double getVol(double /*strike*/, double /*timeToExpiry*/, double /*spot*/) const override {
        return sigma_;
    }
};

class ParametricVolatility : public IVolatilitySurface {
    double atmVol_;
    double slope_;
    double convexity_;

public:
    ParametricVolatility(double atmVol, double slope, double convexity);
    [[nodiscard]] double getVol(double strike, double timeToExpiry, double spot) const override;
};