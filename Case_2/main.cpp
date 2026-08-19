#include "AurumTech.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class AITrendStrategy : public TradingStrategy
{
private:
    double previousPrice = 0.0;

public:
    Decision analyze(double price) override
    {
        if (price <= 0.0)
            throw std::invalid_argument("Price must be positive");

        if (previousPrice == 0.0)
        {
            previousPrice = price;
            return Decision::Hold;
        }

        const Decision decision = price > previousPrice ? Decision::Buy : price < previousPrice ? Decision::Sell
                                                                                                : Decision::Hold;
        previousPrice = price;
        return decision;
    }
};

void printDecisions(const std::string &botName, GoldTradingBot &bot, const std::vector<double> &prices)
{
    std::cout << botName << ": ";
    for (double price : prices)
        std::cout << toString(bot.analyze(price)) << ' ';
    std::cout << '\n';
}

int main()
{
    const std::vector<double> prices{3350.0, 3353.0, 3348.0, 3360.0};

    // Runtime strategy replacement.
    GoldTradingBot replaceableBot(std::make_unique<MomentumStrategy>());
    printDecisions("Momentum", replaceableBot, prices);
    replaceableBot.setStrategy(std::make_unique<ConservativeStrategy>());
    printDecisions("After replacement", replaceableBot, prices);

    // Stateful strategies should have independent objects and independent history.
    GoldTradingBot firstBot(std::make_unique<MomentumStrategy>());
    GoldTradingBot secondBot(std::make_unique<MomentumStrategy>());
    std::cout << "Independent bots: "
              << toString(firstBot.analyze(3300.0)) << ' '
              << toString(firstBot.analyze(3310.0)) << " | "
              << toString(secondBot.analyze(3400.0)) << ' '
              << toString(secondBot.analyze(3390.0)) << '\n';

    // Registry creation and third-party runtime registration.
    StrategyRegistry registry;
    registry.registerStrategy("AI Trend", []
                              { return std::make_unique<AITrendStrategy>(); });

    GoldTradingBot registeredBot(registry.create("AI Trend"));
    printDecisions("AI Trend", registeredBot, prices);

    try
    {
        registry.create("Unknown");
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << "Unknown-key handling: " << error.what() << '\n';
    }

    try
    {
        GoldTradingBot emptyBot;
        emptyBot.analyze(3350.0);
    }
    catch (const std::logic_error &error)
    {
        std::cout << "Missing-strategy handling: " << error.what() << '\n';
    }
}
