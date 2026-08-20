#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

class ElectricCarBuilder;

class DrivingMode
{
public:
    virtual ~DrivingMode() = default;

    virtual std::string name() const = 0;
    virtual std::string drive() const = 0;
    virtual std::unique_ptr<DrivingMode> clone() const = 0;
};

class EcoMode final : public DrivingMode
{
public:
    std::string name() const override { return "Eco"; }
    std::string drive() const override
    {
        return "Smooth acceleration and reduced power consumption";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<EcoMode>(*this);
    }
};

class SportMode final : public DrivingMode
{
public:
    std::string name() const override { return "Sport"; }
    std::string drive() const override
    {
        return "Aggressive throttle response and performance-oriented handling";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<SportMode>(*this);
    }
};

class Vehicle
{
    friend class ElectricCarBuilder;

private:
    std::string model_;
    std::string motor_;
    std::string drive_system_ = "RWD";
    std::string interior_ = "Standard";
    std::string paint_ = "White";

    int battery_kwh_ = 0;
    int wheels_inch_ = 18;
    int seats_ = 5;

    bool autopilot_ = false;
    bool fast_charging_ = false;
    bool performance_package_ = false;
    bool winter_package_ = false;
    bool taxi_package_ = false;

    std::unique_ptr<DrivingMode> driving_mode_;

public:
    Vehicle() = default;

    Vehicle(const Vehicle &other)
        : model_(other.model_),
          motor_(other.motor_),
          drive_system_(other.drive_system_),
          interior_(other.interior_),
          paint_(other.paint_),
          battery_kwh_(other.battery_kwh_),
          wheels_inch_(other.wheels_inch_),
          seats_(other.seats_),
          autopilot_(other.autopilot_),
          fast_charging_(other.fast_charging_),
          performance_package_(other.performance_package_),
          winter_package_(other.winter_package_),
          taxi_package_(other.taxi_package_),
          driving_mode_(other.driving_mode_ ? other.driving_mode_->clone() : nullptr)
    {
    }

    Vehicle &operator=(const Vehicle &other)
    {
        if (this == &other)
            return *this;

        model_ = other.model_;
        motor_ = other.motor_;
        drive_system_ = other.drive_system_;
        interior_ = other.interior_;
        paint_ = other.paint_;
        battery_kwh_ = other.battery_kwh_;
        wheels_inch_ = other.wheels_inch_;
        seats_ = other.seats_;
        autopilot_ = other.autopilot_;
        fast_charging_ = other.fast_charging_;
        performance_package_ = other.performance_package_;
        winter_package_ = other.winter_package_;
        taxi_package_ = other.taxi_package_;
        driving_mode_ = other.driving_mode_ ? other.driving_mode_->clone() : nullptr;

        return *this;
    }

    Vehicle(Vehicle &&) noexcept = default;
    Vehicle &operator=(Vehicle &&) noexcept = default;
    virtual ~Vehicle() = default;

    void setDrivingMode(std::unique_ptr<DrivingMode> mode)
    {
        if (!mode)
            throw std::invalid_argument("Driving mode cannot be null");
        driving_mode_ = std::move(mode);
    }

    std::string drivingModeName() const
    {
        return driving_mode_ ? driving_mode_->name() : "None";
    }

    std::string drive() const
    {
        return driving_mode_ ? driving_mode_->drive() : "No driving mode selected";
    }

    void setPaint(std::string paint)
    {
        if (paint.empty())
            throw std::invalid_argument("Paint cannot be empty");
        paint_ = std::move(paint);
    }

    const std::string &model() const noexcept { return model_; }
    const std::string &motor() const noexcept { return motor_; }
    const std::string &driveSystem() const noexcept { return drive_system_; }
    const std::string &interior() const noexcept { return interior_; }
    const std::string &paint() const noexcept { return paint_; }

    int battery() const noexcept { return battery_kwh_; }
    int wheels() const noexcept { return wheels_inch_; }
    int seats() const noexcept { return seats_; }

    bool hasAutopilot() const noexcept { return autopilot_; }
    bool hasFastCharging() const noexcept { return fast_charging_; }
    bool hasPerformancePackage() const noexcept { return performance_package_; }
    bool hasWinterPackage() const noexcept { return winter_package_; }
    bool hasTaxiPackage() const noexcept { return taxi_package_; }

    virtual std::string type() const = 0;
    virtual std::unique_ptr<Vehicle> clone() const = 0;
};

class ElectricCar : public Vehicle
{
public:
    ElectricCar() = default;
    ElectricCar(const ElectricCar &) = default;
    ElectricCar &operator=(const ElectricCar &) = default;
    ElectricCar(ElectricCar &&) noexcept = default;
    ElectricCar &operator=(ElectricCar &&) noexcept = default;
    ~ElectricCar() override = default;

    std::string type() const override { return "ElectricCar"; }

    std::unique_ptr<Vehicle> clone() const override
    {
        return std::make_unique<ElectricCar>(*this);
    }
};

class SportEV final : public ElectricCar
{
public:
    std::string type() const override { return "SportEV"; }

    std::unique_ptr<Vehicle> clone() const override
    {
        return std::make_unique<SportEV>(*this);
    }
};

class Builder
{
public:
    virtual ~Builder() = default;
};

class ElectricCarBuilder final : public Builder
{
private:
    std::optional<std::string> model_;
    std::optional<int> battery_kwh_;
    std::optional<std::string> motor_;

    std::string drive_system_ = "RWD";
    int wheels_inch_ = 18;
    int seats_ = 5;
    std::string interior_ = "Standard";
    std::string paint_ = "White";

    bool autopilot_ = false;
    bool fast_charging_ = false;
    bool performance_package_ = false;
    bool winter_package_ = false;
    bool taxi_package_ = false;

    static bool isMultiMotor(const std::string &motor)
    {
        return motor.find("Dual") != std::string::npos ||
               motor.find("Tri") != std::string::npos ||
               motor.find("Quad") != std::string::npos ||
               motor.find("Multi") != std::string::npos;
    }

    static bool isHighPerformanceMotor(const std::string &motor)
    {
        return motor.find("Performance") != std::string::npos ||
               motor.find("Dual") != std::string::npos ||
               motor.find("Tri") != std::string::npos ||
               motor.find("Quad") != std::string::npos;
    }

    void validate() const
    {
        if (!model_ || model_->empty())
            throw std::invalid_argument("Vehicle model is required");
        if (!battery_kwh_)
            throw std::invalid_argument("Battery capacity is required");
        if (*battery_kwh_ <= 0)
            throw std::invalid_argument("Battery capacity must be positive");
        if (!motor_ || motor_->empty())
            throw std::invalid_argument("Motor configuration is required");
        if (wheels_inch_ <= 0)
            throw std::invalid_argument("Wheel size must be positive");
        if (seats_ <= 0)
            throw std::invalid_argument("Seat count must be positive");

        if (drive_system_ == "AWD" && !isMultiMotor(*motor_))
            throw std::invalid_argument("AWD requires a compatible multi-motor configuration");

        if (performance_package_ && !isHighPerformanceMotor(*motor_))
            throw std::invalid_argument("Performance package requires a high-performance motor configuration");

        if (fast_charging_ && *battery_kwh_ < 75)
            throw std::invalid_argument("High-power charging requires a battery of at least 75 kWh");

        if (seats_ == 2 && taxi_package_)
            throw std::invalid_argument("A two-seat sport configuration cannot use the taxi package");
    }

public:
    ElectricCarBuilder &model(std::string value)
    {
        model_ = std::move(value);
        return *this;
    }

    ElectricCarBuilder &battery(int kwh)
    {
        battery_kwh_ = kwh;
        return *this;
    }

    ElectricCarBuilder &motor(std::string value)
    {
        motor_ = std::move(value);
        return *this;
    }

    ElectricCarBuilder &drive(std::string value)
    {
        drive_system_ = std::move(value);
        return *this;
    }

    ElectricCarBuilder &wheels(int inch)
    {
        wheels_inch_ = inch;
        return *this;
    }

    ElectricCarBuilder &seats(int count)
    {
        seats_ = count;
        return *this;
    }

    ElectricCarBuilder &interior(std::string value)
    {
        interior_ = std::move(value);
        return *this;
    }

    ElectricCarBuilder &paint(std::string value)
    {
        paint_ = std::move(value);
        return *this;
    }

    ElectricCarBuilder &autopilot(bool enabled = true)
    {
        autopilot_ = enabled;
        return *this;
    }

    ElectricCarBuilder &fastCharging(bool enabled = true)
    {
        fast_charging_ = enabled;
        return *this;
    }

    ElectricCarBuilder &performancePackage(bool enabled = true)
    {
        performance_package_ = enabled;
        return *this;
    }

    ElectricCarBuilder &winterPackage(bool enabled = true)
    {
        winter_package_ = enabled;
        return *this;
    }

    ElectricCarBuilder &taxiPackage(bool enabled = true)
    {
        taxi_package_ = enabled;
        return *this;
    }

    ElectricCarBuilder &reset()
    {
        model_.reset();
        battery_kwh_.reset();
        motor_.reset();
        drive_system_ = "RWD";
        wheels_inch_ = 18;
        seats_ = 5;
        interior_ = "Standard";
        paint_ = "White";
        autopilot_ = false;
        fast_charging_ = false;
        performance_package_ = false;
        winter_package_ = false;
        taxi_package_ = false;
        return *this;
    }

    ElectricCar build()
    {
        validate();

        ElectricCar car;
        car.model_ = *model_;
        car.battery_kwh_ = *battery_kwh_;
        car.motor_ = *motor_;
        car.drive_system_ = drive_system_;
        car.wheels_inch_ = wheels_inch_;
        car.seats_ = seats_;
        car.interior_ = interior_;
        car.paint_ = paint_;
        car.autopilot_ = autopilot_;
        car.fast_charging_ = fast_charging_;
        car.performance_package_ = performance_package_;
        car.winter_package_ = winter_package_;
        car.taxi_package_ = taxi_package_;

        reset();
        return car;
    }
};

class FleetRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<Vehicle>> registry_;

    const Vehicle &require(const std::string &name) const
    {
        const auto it = registry_.find(name);
        if (it == registry_.end())
            throw std::out_of_range("Unknown fleet template: " + name);
        return *it->second;
    }

public:
    bool contains(const std::string &name) const
    {
        return registry_.find(name) != registry_.end();
    }

    void add(std::string name, std::unique_ptr<Vehicle> vehicle)
    {
        if (name.empty())
            throw std::invalid_argument("Fleet template name cannot be empty");
        if (!vehicle)
            throw std::invalid_argument("Fleet template cannot be null");

        const auto [it, inserted] = registry_.emplace(std::move(name), std::move(vehicle));
        if (!inserted)
            throw std::invalid_argument("Fleet template is already registered");
    }

    void add(std::string name, const Vehicle &vehicle)
    {
        add(std::move(name), vehicle.clone());
    }

    bool remove(const std::string &name)
    {
        return registry_.erase(name) != 0;
    }

    std::unique_ptr<Vehicle> create(const std::string &name) const
    {
        return require(name).clone();
    }

    Vehicle &operator[](const std::string &name)
    {
        return at(name);
    }

    const Vehicle &operator[](const std::string &name) const
    {
        return at(name);
    }

    Vehicle &at(const std::string &name)
    {
        const auto it = registry_.find(name);
        if (it == registry_.end())
            throw std::out_of_range("Unknown fleet template: " + name);
        return *it->second;
    }

    const Vehicle &at(const std::string &name) const
    {
        return require(name);
    }
};

class VehicleFactory
{
public:
    using Creator = std::function<std::unique_ptr<Vehicle>()>;

private:
    std::unordered_map<std::string, Creator> creators_;

public:
    bool contains(const std::string &name) const
    {
        return creators_.find(name) != creators_.end();
    }

    void registerCreator(std::string name, Creator creator)
    {
        if (name.empty())
            throw std::invalid_argument("Vehicle type name cannot be empty");
        if (!creator)
            throw std::invalid_argument("Vehicle creator cannot be empty");

        const auto [it, inserted] = creators_.emplace(std::move(name), std::move(creator));
        if (!inserted)
            throw std::invalid_argument("Vehicle creator is already registered");
    }

    bool remove(const std::string &name)
    {
        return creators_.erase(name) != 0;
    }

    std::unique_ptr<Vehicle> create(const std::string &name) const
    {
        const auto it = creators_.find(name);
        if (it == creators_.end())
            throw std::out_of_range("Unknown vehicle type: " + name);

        auto vehicle = it->second();
        if (!vehicle)
            throw std::runtime_error("Vehicle creator returned nullptr for type: " + name);

        return vehicle;
    }
};
