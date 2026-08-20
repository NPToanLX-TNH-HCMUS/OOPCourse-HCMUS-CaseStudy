
```mermaid
classDiagram
    class TradingStrategy {
        <<interface>>
        +analyze(price)
    }

    class ConservativeStrategy {
        -history: PriceHistory
        -windowSize: size_t
        -buyDiscount: double
        +analyze(price)
    }

    class MomentumStrategy {
        -history: PriceHistory
        -requiredMoves: size_t
        +analyze(price)
    }

    class ThresholdStrategy {
        -buyBelow: double
        -sellAbove: double
        +analyze(price)
    }

    class GoldTradingBot {
        -strategy: StrategyPtr
        +GoldTradingBot(strategy)
        +setStrategy(strategy)
        +analyze(price)
    }

    class StrategyRegistry {
        -registry: CreatorMap
        +registerStrategy(name, creator)
        +create(name)
        +contains(name)
        +remove(name)
    }

    TradingStrategy <|.. ConservativeStrategy
    TradingStrategy <|.. MomentumStrategy
    TradingStrategy <|.. ThresholdStrategy
    GoldTradingBot o--> TradingStrategy : owns one
    StrategyRegistry ..> TradingStrategy : creates
```
