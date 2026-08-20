#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

enum class Decision
{
    Buy,
    Sell,
    Hold
};

inline const char *toString(Decision decision)
{
    switch (decision)
    {
    case Decision::Buy:
        return "BUY";
    case Decision::Sell:
        return "SELL";
    case Decision::Hold:
        return "HOLD";
    }
    return "UNKNOWN";
}

class TradingStrategy
{
public:
    virtual ~TradingStrategy() = default;
    virtual Decision analyze(double price) = 0;
};

class ConservativeStrategy : public TradingStrategy
{
private:
    std::vector<double> history;
    std::size_t windowSize;
    double buyDiscount;

public:
    explicit ConservativeStrategy(std::size_t windowSize_ = 3, double buyDiscount_ = 0.02)
        : windowSize(windowSize_), buyDiscount(buyDiscount_)
    {
        if (windowSize == 0 || buyDiscount < 0.0)
            throw std::invalid_argument("Invalid conservative strategy configuration");
    }

    Decision analyze(double price) override
    {
        if (price <= 0.0)
            throw std::invalid_argument("Price must be positive");
        history.push_back(price);
        if (history.size() > windowSize)
            history.erase(history.begin());

        double average = 0.0;
        for (double value : history)
            average += value;
        average /= static_cast<double>(history.size());
        return price < average * (1.0 - buyDiscount) ? Decision::Buy : Decision::Hold;
    }
};

class MomentumStrategy : public TradingStrategy
{
private:
    std::vector<double> history;
    std::size_t requiredMoves;

public:
    explicit MomentumStrategy(std::size_t requiredMoves_ = 2) : requiredMoves(requiredMoves_)
    {
        if (requiredMoves == 0)
            throw std::invalid_argument("Required momentum moves must be positive");
    }

    Decision analyze(double price) override
    {
        if (price <= 0.0)
            throw std::invalid_argument("Price must be positive");
        history.push_back(price);
        if (history.size() <= requiredMoves)
            return Decision::Hold;

        const std::size_t start = history.size() - requiredMoves - 1;
        bool rising = true;
        bool falling = true;
        for (std::size_t i = start + 1; i < history.size(); ++i)
        {
            rising = rising && history[i] > history[i - 1];
            falling = falling && history[i] < history[i - 1];
        }
        if (rising)
            return Decision::Buy;
        if (falling)
            return Decision::Sell;
        return Decision::Hold;
    }
};

class ThresholdStrategy : public TradingStrategy
{
private:
    double buyBelow;
    double sellAbove;

public:
    ThresholdStrategy(double buyBelow_, double sellAbove_)
        : buyBelow(buyBelow_), sellAbove(sellAbove_)
    {
        if (buyBelow >= sellAbove)
            throw std::invalid_argument("Buy threshold must be below sell threshold");
    }

    Decision analyze(double price) override
    {
        if (price <= 0.0)
            throw std::invalid_argument("Price must be positive");
        if (price <= buyBelow)
            return Decision::Buy;
        if (price >= sellAbove)
            return Decision::Sell;
        return Decision::Hold;
    }
};

class GoldTradingBot
{
private:
    std::unique_ptr<TradingStrategy> strategy;

public:
    GoldTradingBot() = default;

    explicit GoldTradingBot(std::unique_ptr<TradingStrategy> strategy_)
        : strategy(std::move(strategy_))
    {
    }

    void setStrategy(std::unique_ptr<TradingStrategy> strategy_)
    {
        strategy = std::move(strategy_);
    }

    Decision analyze(double price)
    {
        if (!strategy)
            throw std::logic_error("GoldTradingBot has no strategy");
        return strategy->analyze(price);
    }
};

class StrategyRegistry
{
private:
    using Creator = std::function<std::unique_ptr<TradingStrategy>()>;
    std::unordered_map<std::string, Creator> registry;

public:
    StrategyRegistry()
    {
        registerStrategy("Conservative", []
                         { return std::make_unique<ConservativeStrategy>(); });
        registerStrategy("Momentum", []
                         { return std::make_unique<MomentumStrategy>(); });
        registerStrategy("Threshold", []
                         { return std::make_unique<ThresholdStrategy>(3300.0, 3360.0); });
    }

    void registerStrategy(const std::string &name, Creator creator)
    {
        if (contains(name))
            throw std::invalid_argument("Strategy name already taken: " + name);
        registry.emplace(name, std::move(creator));
    }

    std::unique_ptr<TradingStrategy> create(const std::string &name) const
    {
        const auto it = registry.find(name);
        if (it == registry.end())
            throw std::invalid_argument("Unknown strategy: " + name);
        return it->second();
    }

    bool contains(const std::string &name) const
    {
        return registry.find(name) != registry.end();
    }

    void remove(const std::string &name)
    {
        if (registry.erase(name) == 0)
            throw std::invalid_argument("Unknown strategy: " + name);
    }
};
