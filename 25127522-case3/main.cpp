#include "VoltEdge.hpp"

#include <iostream>
#include <memory>
#include <string>

class ComfortMode : public DrivingMode
{
public:
    std::string name() const override { return "Comfort"; }

    std::string drive() const override
    {
        return "Balanced acceleration with a comfortable suspension profile";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<ComfortMode>(*this);
    }
};

class SnowMode : public DrivingMode
{
public:
    std::string name() const override { return "Snow"; }

    std::string drive() const override
    {
        return "Gentle torque delivery with traction-focused control";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<SnowMode>(*this);
    }
};

class TrackMode : public DrivingMode
{
public:
    std::string name() const override { return "Track"; }

    std::string drive() const override
    {
        return "Maximum performance with track-oriented response";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<TrackMode>(*this);
    }
};

class AutonomousMode : public DrivingMode
{
public:
    std::string name() const override { return "Autonomous"; }

    std::string drive() const override
    {
        return "Autonomous speed, steering, and lane control";
    }

    std::unique_ptr<DrivingMode> clone() const override
    {
        return std::make_unique<AutonomousMode>(*this);
    }
};

class RefrigeratedDeliveryVan : public ElectricCar
{
public:
    std::string type() const override { return "RefrigeratedDeliveryVan"; }

    std::unique_ptr<Vehicle> clone() const override
    {
        return std::make_unique<RefrigeratedDeliveryVan>(*this);
    }
};

void printVehicle(const std::string &label, const Vehicle &vehicle)
{
    std::cout << label << "\n"
              << "  type: " << vehicle.type() << "\n"
              << "  model: " << vehicle.model() << "\n"
              << "  battery: " << vehicle.battery() << " kWh\n"
              << "  driving mode: " << vehicle.drivingModeName() << "\n"
              << "  behavior: " << vehicle.drive() << "\n";
}

int main()
{
    // Chapter 1: readable construction with validation.
    auto sportCar = ElectricCarBuilder()
                        .model("VoltEdge Performance")
                        .battery(100)
                        .motor("Dual Motor")
                        .drive("AWD")
                        .wheels(21)
                        .interior("Premium")
                        .paint("Black")
                        .autopilot()
                        .fastCharging()
                        .performancePackage()
                        .build();

    sportCar.setDrivingMode(std::make_unique<SportMode>());
    printVehicle("Configured vehicle", sportCar);

    // Chapter 2: runtime Strategy replacement.
    sportCar.setDrivingMode(std::make_unique<ComfortMode>());
    printVehicle("After switching to an application mode", sportCar);

    sportCar.setDrivingMode(std::make_unique<AutonomousMode>());
    printVehicle("After switching to another external mode", sportCar);

    // Chapter 4: deep-copy / Prototype independence.
    auto copiedCar = sportCar.clone();
    copiedCar->setPaint("Silver");
    copiedCar->setDrivingMode(std::make_unique<TrackMode>());
    printVehicle("Cloned vehicle", *copiedCar);
    printVehicle("Original remains independent", sportCar);

    // Chapter 5: saved fleet templates create independent vehicles.
    FleetRegistry fleets;
    fleets.add("city_taxi_standard", sportCar);

    auto taxi1 = fleets.create("city_taxi_standard");
    auto taxi2 = fleets.create("city_taxi_standard");
    taxi1->setPaint("Yellow");
    taxi1->setDrivingMode(std::make_unique<ComfortMode>());

    printVehicle("Fleet vehicle 1", *taxi1);
    printVehicle("Fleet vehicle 2", *taxi2);
    printVehicle("Stored template", fleets.at("city_taxi_standard"));

    // Chapter 7: partner registration through a creator function.
    VehicleFactory vehicleFactory;
    vehicleFactory.registerCreator("sport", []
                                   { return std::make_unique<SportEV>(); });
    vehicleFactory.registerCreator("refrigerated_delivery", []
                                   { return std::make_unique<RefrigeratedDeliveryVan>(); });

    auto sport = vehicleFactory.create("sport");
    auto refrigeratedVan = vehicleFactory.create("refrigerated_delivery");
    printVehicle("Factory-created sport vehicle", *sport);
    printVehicle("Partner plugin vehicle", *refrigeratedVan);

    // Chapter 6: lookup-only operations reject unknown names safely.
    try
    {
        fleets.create("police_car");
    }
    catch (const std::out_of_range &error)
    {
        std::cout << "Unknown fleet key: " << error.what() << '\n';
    }

    try
    {
        vehicleFactory.create("unknown_vehicle");
    }
    catch (const std::out_of_range &error)
    {
        std::cout << "Unknown factory key: " << error.what() << '\n';
    }

    // Validation demonstration.
    try
    {
        ElectricCarBuilder()
            .model("Invalid AWD")
            .battery(80)
            .motor("Single Motor")
            .drive("AWD")
            .build();
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << "Validation error: " << error.what() << '\n';
    }
}
