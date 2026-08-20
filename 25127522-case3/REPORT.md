# VoltEdge Electric Vehicle Platform

## Chapter 4. Deep Copy Challenge

The expression below is not sufficient when `Vehicle` owns a `std::unique_ptr<DrivingMode>`:

```cpp
std::unique_ptr<ElectricCar> clone() const
{
    return std::make_unique<ElectricCar>(*this);
}
```

The default copy operation cannot copy a `std::unique_ptr`, because `unique_ptr` is move-only. A correct solution must define a deep-copy constructor and copy assignment operator for `Vehicle`. Value fields are copied normally, while the polymorphic driving mode is copied with `driving_mode_->clone()`.

Each concrete vehicle then implements `clone()` by copying itself. This creates an independent vehicle and an independent driving-mode object. Changing the clone's paint or driving mode does not modify the original vehicle or its mode.

## Chapter 6. Registry Lookup Trap

For a raw `std::unordered_map`, `registry["police_car"]` is not a lookup-only operation. If the key does not exist, `operator[]` inserts a new entry with a default value. For a map of `std::unique_ptr<ElectricCar>`, that value is a null pointer, which can silently modify the registry and may cause a later dereference failure.

`find()` checks for a key without inserting anything. `at()` also performs lookup without insertion, but throws `std::out_of_range` when the key is missing. The `FleetRegistry` implementation uses `find()` internally and its `operator[]` delegates to `at()`, so unknown names are handled safely.

## Chapter 8. Two Similar-Looking Creation Systems

The following systems are not the same merely because both contain a `create()` operation:

```cpp
// System A: stores configured prototypes.
std::unordered_map<std::string, std::unique_ptr<ElectricCar>> prototypes;
return prototypes.at(name)->clone();

// System B: stores creator functions.
std::unordered_map<std::string,
                   std::function<std::unique_ptr<ElectricCar>()>> creators;
return creators.at(name)();
```

System A stores existing configured objects and creates a new object by cloning the selected prototype. It is Prototype-based. System B stores executable creation logic and constructs a new object by invoking a creator function. It is Factory-based. Both produce fresh objects, but they differ in what is stored and how the instance is produced.

## Chapter 9. The Factory-Method Naming Trap

A method named `create()` inside a class named `VehicleFactory` does not automatically make the design the GoF Factory Method pattern. Factory Method is a specific pattern in which a base class declares a creation method and subclasses override it to decide which concrete product is created.

The current `VehicleFactory` uses a registry of `std::function` creators. It is better described as a Factory Registry or a registry-based Simple Factory. Its design intention is runtime registration and composition, not subclass-based Factory Method overriding.

## Deliverables for Case 3
### 31. UML Class Diagram
> [!NOTE]
The UML class diagram is provided in [`Mermaid.md`](Mermaid.md).

### 32, 33. C++ Implementation and Complex Vehicle Configuration

The reusable implementation is provided in [`VoltEdge.hpp`](VoltEdge.hpp), and the direct demonstration is in [`main.cpp`](main.cpp).

`ElectricCarBuilder` constructs an `ElectricCar` through readable chained calls. Required fields are the model, battery, and motor. Optional fields include drive system, wheel size, seats, interior, paint, autopilot, fast charging, performance package, winter package, and taxi package.

The builder validates the following rules:

- required fields must be present;
- battery, wheel size, and seat count must be positive;
- AWD requires a multi-motor configuration;
- the performance package requires a high-performance motor;
- high-power charging requires a battery of at least 75 kWh;
- a two-seat vehicle cannot use the taxi package.

Invalid configurations throw `std::invalid_argument`.

### 34. Runtime Replaceable Driving Behavior

`DrivingMode` is the Strategy interface. `EcoMode` and `SportMode` are built-in library modes. `ComfortMode`, `SnowMode`, `TrackMode`, and `AutonomousMode` are application-level extensions declared in `main.cpp`.

`Vehicle::setDrivingMode()` replaces the owned strategy at runtime. The vehicle does not contain a conditional branch for every driving mode.

### 35, 36. Fleet Templates and Named Registry Operations

`FleetRegistry` is a Prototype Registry. It stores configured `Vehicle` objects by name and creates new vehicles with `clone()`.

The registry supports `add()`, `remove()`, `contains()`, `create()`, and lookup through `at()`. Unknown keys throw `std::out_of_range`. The registry owns its stored templates with `std::unique_ptr`, while each call to `create()` returns an independent clone.

### 37. Extensible Vehicle Creation

`VehicleFactory` stores creator functions with the type:

```cpp
std::function<std::unique_ptr<Vehicle>()>
```

Partner code can register `RefrigeratedDeliveryVan` or another vehicle without adding a new central `if/else` branch. The factory is therefore a Factory Registry based on creators.

### 38. Test and Demonstration Scenarios

The Case 3 program follows the direct demonstration style used in Case 2. It covers at least eight scenarios:

1. Build and print a valid, fully configured vehicle.
2. Replace the driving mode at runtime with an application-defined mode.
3. Replace it again with another external mode.
4. Clone a vehicle and modify the clone independently.
5. Create two vehicles from one saved fleet template and verify independent state.
6. Register and create a partner vehicle through `VehicleFactory`.
7. Handle unknown fleet and factory keys with exceptions.
8. Demonstrate validation failure for an incompatible AWD configuration.

The program output is intended as a readable integration demonstration rather than a unit-test harness.

### 39. Architecture Explanation

The platform separates five responsibilities. `ElectricCarBuilder` handles complicated step-by-step construction and validates the temporary configuration before producing a car. `DrivingMode` handles interchangeable driving behavior, so a vehicle can change from Eco to Sport without changing the vehicle class. `Vehicle` and its subclasses implement the Prototype interface through `clone()`. `FleetRegistry` stores configured vehicle templates and creates independent copies by cloning them. `VehicleFactory` stores creator functions for vehicle types that may be added by partner plugins at runtime.

Ownership is explicit. A vehicle owns its driving mode through `std::unique_ptr`, and the fleet registry owns its stored templates. The copy constructor of `Vehicle` copies value fields and clones the polymorphic driving mode, which prevents a copied vehicle from sharing the original mode object. This is a deep-copy solution for the owned polymorphic component.

The two creation systems are intentionally different. `FleetRegistry` copies an existing configured object, while `VehicleFactory` executes a registered creator function. Both return fresh objects, but one is prototype-based and the other is factory-based. Lookup uses `find()` internally, so unknown names do not insert accidental entries. The architecture therefore keeps construction, behavior, copying, lookup, and extensibility in separate components.

## Architecture Reasoning Questions

### 40. Which Subsystem Handles Complicated Object Construction, and Why?

`ElectricCarBuilder` handles construction because the vehicle has many optional fields and cross-field validation rules. It makes client code readable and prevents a long, error-prone constructor.

### 41. Which Subsystem Handles Interchangeable Driving Behavior, and Why?

`DrivingMode` and its concrete classes handle driving behavior. The Strategy pattern allows the vehicle to replace its behavior at runtime without modifying `Vehicle`.

### 42. Which Subsystem Creates an Object from an Existing Configured Object?

`FleetRegistry::create()` calls `clone()` on a stored vehicle template. This is the Prototype pattern.

### 43. Which Subsystem Maps Names to Saved Configurations?

`FleetRegistry` maps names such as `city_taxi_standard` to stored configured vehicle prototypes.

### 44. Which Subsystem Decides Which Concrete Vehicle Type Is Instantiated?

`VehicleFactory` selects a registered creator by name. The creator determines the concrete type, such as `SportEV` or `RefrigeratedDeliveryVan`.

### 45. Can One Class Participate in More Than One Design Pattern?

Yes. `Vehicle` participates in the Strategy structure by owning a `DrivingMode`, and it participates in the Prototype structure by implementing `clone()`. `ElectricCarBuilder` is part of the Builder pattern, while its resulting `ElectricCar` can also be cloned.

### 46. Is a Registry Necessarily a Factory?

No. A registry is primarily a name-to-object or name-to-creator mapping. `FleetRegistry` is prototype-based because it stores objects and calls `clone()`. `VehicleFactory` is factory-based because it stores creator functions.

### 47. Is a Factory Necessarily Implemented Using Inheritance?

No. `VehicleFactory` uses `std::function` and registration, so it does not require a factory subclass for every vehicle type. The product hierarchy still uses inheritance because callers need a common polymorphic `Vehicle` interface.

### 48. Does Every Class with `clone()` Correctly Implement Prototype?

No. A class should provide a meaningful prototype operation that creates an independent object with the intended copied state. A shallow copy of owned raw pointers would not satisfy the requirement. Here, `Vehicle` clones its `DrivingMode`, and concrete vehicle classes return copies of their own dynamic type.

### 49. Does Every Function Returning `std::unique_ptr<Base>` Represent a Factory?

No. The return type only expresses polymorphic ownership. `FleetRegistry::create()` returns a cloned prototype, while `VehicleFactory::create()` invokes a registered creator. Both return `std::unique_ptr<Vehicle>`, but they use different creation mechanisms.

## Test Evidence
*Log output:*

```
Configured vehicle
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Sport
  behavior: Aggressive throttle response and performance-oriented handling
After switching to an application mode
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Comfort
  behavior: Balanced acceleration with a comfortable suspension profile
After switching to another external mode
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Autonomous
  behavior: Autonomous speed, steering, and lane control
Cloned vehicle
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Track
  behavior: Maximum performance with track-oriented response
Original remains independent
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Autonomous
  behavior: Autonomous speed, steering, and lane control
Fleet vehicle 1
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Comfort
  behavior: Balanced acceleration with a comfortable suspension profile
Fleet vehicle 2
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Autonomous
  behavior: Autonomous speed, steering, and lane control
Stored template
  type: ElectricCar
  model: VoltEdge Performance
  battery: 100 kWh
  driving mode: Autonomous
  behavior: Autonomous speed, steering, and lane control
Factory-created sport vehicle
  type: SportEV
  model: 
  battery: 0 kWh
  driving mode: None
  behavior: No driving mode selected
Partner plugin vehicle
  type: RefrigeratedDeliveryVan
  model: 
  battery: 0 kWh
  driving mode: None
  behavior: No driving mode selected
Unknown fleet key: Unknown fleet template: police_car
Unknown factory key: Unknown vehicle type: unknown_vehicle
Validation error: AWD requires a compatible multi-motor configuration
```
