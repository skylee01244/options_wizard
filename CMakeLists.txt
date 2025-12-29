cmake_minimum_required(VERSION 3.28)

project(options_pricing_model)

set(CMAKE_CXX_STANDARD 20)

add_executable(options_pricing_model
        main.cpp
        BlackScholes.cpp
        Option.cpp
        OptionWizard.cpp
        Strategy.cpp
        VolatilitySurface.cpp
        Tests/FiniteDifferenceTest.h
        Tests/ParityTest.h
)

target_include_directories(options_pricing_model PRIVATE ${CMAKE_SOURCE_DIR}/Headers)