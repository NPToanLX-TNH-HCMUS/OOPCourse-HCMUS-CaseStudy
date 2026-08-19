#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <utility>

class TravelPackageBuilder;

class Package
{
private:
    friend class Builder;

protected:
    // Required parameters
    std::string departure;
    std::string destination;
    std::string flightClass;

    // Optional parameters
    std::string hotel = "";
    int nights = 0;
    int checkedBags = 0;
    std::string seat = "Unassigned";
    std::string meals = "None";

    // Boolean flags
    bool airportPickup = false;
    bool travelInsurance = false;
    bool cityTransport = false;
    bool localTour = false;

public:
    Package() = default;
    virtual ~Package() = default;

    void printDetails() const
    {
        std::cout << "--- Travel Package: " << departure << " to " << destination << " ---\n"
                  << "Flight Class: " << flightClass << "\n"
                  << "Seat: " << seat << "\n"
                  << "Checked Bags: " << checkedBags << "\n"
                  << "Meals: " << meals << "\n";

        if (!hotel.empty())
        {
            std::cout << "Hotel: " << hotel << " (" << nights << " nights)\n";
        }

        std::cout << "Add-ons: ";
        bool hasAddons = false;
        if (airportPickup)
        {
            std::cout << "[Airport Pickup] ";
            hasAddons = true;
        }
        if (travelInsurance)
        {
            std::cout << "[Travel Insurance] ";
            hasAddons = true;
        }
        if (cityTransport)
        {
            std::cout << "[City Transport] ";
            hasAddons = true;
        }
        if (localTour)
        {
            std::cout << "[Local Tour] ";
            hasAddons = true;
        }
        if (!hasAddons)
            std::cout << "None";
        std::cout << "\n\n";
    }
};

class TravelPackage : public Package
{
};

class Builder
{
protected:
    TravelPackage trip;

public:
    virtual ~Builder() = default;

    void reset()
    {
        trip = TravelPackage();
    }

    Builder &from(std::string dep)
    {
        trip.departure = std::move(dep);
        return *this;
    }

    Builder &to(std::string dest)
    {
        trip.destination = std::move(dest);
        return *this;
    }

    Builder &setFlightClass(std::string fClass)
    {
        trip.flightClass = std::move(fClass);
        return *this;
    }

    Builder &setHotel(std::string hotelName)
    {
        trip.hotel = std::move(hotelName);
        trip.nights = 1;
        return *this;
    }

    Builder &setNights(int n)
    {
        trip.nights = n;
        return *this;
    }

    Builder &setCheckedBags(int bags)
    {
        trip.checkedBags = bags;
        return *this;
    }

    Builder &setSeat(std::string seatPref)
    {
        trip.seat = std::move(seatPref);
        return *this;
    }

    Builder &setMeals(std::string meal)
    {
        trip.meals = std::move(meal);
        return *this;
    }

    Builder &addAirportPickup()
    {
        trip.airportPickup = true;
        return *this;
    }

    Builder &addTravelInsurance()
    {
        trip.travelInsurance = true;
        return *this;
    }

    Builder &addCityTransport()
    {
        trip.cityTransport = true;
        return *this;
    }

    Builder &addLocalTour()
    {
        trip.localTour = true;
        return *this;
    }

    TravelPackage build()
    {
        if (trip.departure.empty() || trip.destination.empty() || trip.flightClass.empty())
        {
            throw std::invalid_argument("Error: Departure, destination, and flight class are required.");
        }
        if (trip.departure == trip.destination)
        {
            throw std::invalid_argument("Error: Departure and destination cannot be the same.");
        }
        if (trip.nights < 0)
        {
            throw std::invalid_argument("Error: Number of nights cannot be negative.");
        }
        if (trip.checkedBags < 0)
        {
            throw std::invalid_argument("Error: Checked baggage cannot be negative.");
        }
        if (!trip.hotel.empty() && trip.nights <= 0)
        {
            throw std::invalid_argument("Error: If a hotel is selected, nights must be greater than zero.");
        }
        if (trip.airportPickup && (trip.flightClass != "Business" && trip.flightClass != "First Class"))
        {
            throw std::invalid_argument("Error: Private airport limousine is only available for Business or First Class.");
        }

        TravelPackage result = std::move(trip);
        reset();
        return result;
    }
};

class TravelPackageBuilder : public Builder
{
};

class Director
{
private:
    Builder &builder;

public:
    explicit Director(Builder &builder) : builder(builder) {}

    TravelPackage buildBudgetTravelerPackage(std::string departure, std::string destination)
    {
        builder.reset();
        return builder.from(std::move(departure))
            .to(std::move(destination))
            .setFlightClass("Economy")
            .setCheckedBags(1)
            .setHotel("Standard Hotel")
            .build();
    }

    TravelPackage buildBusinessTravelerPackage(std::string departure, std::string destination)
    {
        builder.reset();
        return builder.from(std::move(departure))
            .to(std::move(destination))
            .setFlightClass("Business")
            .addAirportPickup()
            .addTravelInsurance()
            .build();
    }

    TravelPackage buildLuxuryTravelerPackage(std::string departure, std::string destination)
    {
        builder.reset();
        return builder.from(std::move(departure))
            .to(std::move(destination))
            .setFlightClass("First Class")
            .setHotel("Five-star Hotel")
            .addTravelInsurance()
            .addLocalTour()
            .setMeals("Premium")
            .build();
    }
};
