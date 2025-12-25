#pragma once
#include "Greeks.h"
#include "Option.h"

// C + K*e^(-rT) = P + S
void checkPutCallParity(const Greeks& callGreeks, const Greeks& putGreeks, const Option& callOption, double spot, double r);