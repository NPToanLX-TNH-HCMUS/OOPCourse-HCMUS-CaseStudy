```mermaid
classDiagram
    %% EcoMode and SportMode are built-in library modes.
    %% ComfortMode, SnowMode, TrackMode, AutonomousMode, and
    %% RefrigeratedDeliveryVan are application/plugin types from main.cpp.
    class DrivingMode {
        <<interface>>
        +name()
        +drive()
        +clone()
    }

    class EcoMode
    class ComfortMode
    class SportMode
    class SnowMode
    class TrackMode
    class AutonomousMode

    class Vehicle {
        <<abstract>>
        -model_: string
        -battery_kwh_: int
        -motor_: string
        -driving_mode_: DrivingModePtr
        +setDrivingMode(mode)
        +clone()
        +setPaint(paint)
    }

    class ElectricCar
    class SportEV
    class RefrigeratedDeliveryVan

    class Builder {
        <<interface>>
    }

    class ElectricCarBuilder {
        -optional configuration fields
        +model(value) ElectricCarBuilder
        +battery(kwh) ElectricCarBuilder
        +motor(value) ElectricCarBuilder
        +drive(value) ElectricCarBuilder
        +build() ElectricCar
        +reset() ElectricCarBuilder
    }

    class FleetRegistry {
        -registry_: VehicleTemplateMap
        +add(name, vehicle)
        +remove(name)
        +contains(name)
        +create(name)
        +at(name)
    }

    class VehicleFactory {
        -creators_: VehicleCreatorMap
        +registerCreator(name, creator)
        +remove(name)
        +contains(name)
        +create(name)
    }

    DrivingMode <|.. EcoMode
    DrivingMode <|.. ComfortMode
    DrivingMode <|.. SportMode
    DrivingMode <|.. SnowMode
    DrivingMode <|.. TrackMode
    DrivingMode <|.. AutonomousMode

    Vehicle <|-- ElectricCar
    ElectricCar <|-- SportEV
    ElectricCar <|-- RefrigeratedDeliveryVan
    Builder <|-- ElectricCarBuilder

    Vehicle o--> DrivingMode : owns one
    ElectricCarBuilder ..> ElectricCar : builds
    FleetRegistry o--> Vehicle : owns templates
    FleetRegistry ..> Vehicle : clones
    VehicleFactory ..> Vehicle : creates
```


> [!NOTE]
`EcoMode` and `SportMode` are built into `VoltEdge.hpp`.
`ComfortMode`, `SnowMode`, `TrackMode`, and `AutonomousMode` are declared in `main.cpp` as application extensions.
`RefrigeratedDeliveryVan` is also declared in `main.cpp` as a partner vehicle plugin and registered through `VehicleFactory`.
