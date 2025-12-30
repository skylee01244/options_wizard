#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include "OptionWizard.h"

namespace UI {

    double getDouble(const std::string& prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value < 0) {
                    std::cout << "Must be positive";
                    continue;
                }
                return value;
            } else {
                std::cout << "Invalid type";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    inline double getExpiry(const std::string& prompt, double min_days) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value <= 0) {
                    std::cout << "Must be positive";
                    continue;
                }
                if (value < min_days) {
                    std::cout << "[ERROR] Option cannot expire before your target date:"<< min_days << std::endl;
                    continue;
                }
                return value;

            } else {
                std::cout << "Invalid type";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    void printTable(const std::vector<result>& results) {
        std::cout << "\n" << std::string(110, '-') << std::endl;
        printf("%-20s", "");
        printf("%-10s", "Cost ($)");
        printf("%-15s", "Projected ($)");
        printf("%-13s", "Exp Val ($)");
        printf("%-13s", "Return (%)");
        printf("%-10s", "PoP (%)");
        printf("%-8s", "Delta");
        printf("%-8s", "Gamma");
        printf("%-8s", "Theta");
        printf("%-8s", "Vega");

        printf("\n");


        for (const auto& res : results) {
            printf("%-20s", res.strategyName.c_str());
            printf("%-10.3f", res.entryCost);
            printf("%-15.3f", res.projectedValue);
            printf("%-13.3f", res.expectedValue);
            printf("%-13.2f", res.profitPercent);
            printf("%-10.2f", (res.pop * 100.0));
            printf("%-8.2f", res.netGreeks.delta);
            printf("%-8.2f", res.netGreeks.gamma);
            printf("%-8.2f", res.netGreeks.theta);
            printf("%-8.2f", res.netGreeks.vega * 0.01);

            printf("\n");
        }
        std::cout << std::string(110, '-') << "\n";
    }
}