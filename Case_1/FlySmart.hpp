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
    friend class TravelPackageBuilder;

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

    // Boolean flags
    bool meals = false;
    bool airportPickup = false;
    bool travelInsurance = false;
    bool cityTransport = false;
    bool privateTour = false;

public:
    Package() = default;
    virtual ~Package() = default;
    void printDetails() const
    {
        std::cout << "--- Travel Package: " << departure << " to " << destination << " ---\n"
                  << "Flight Class: " << flightClass << "\n"
                  << "Seat: " << seat << "\n"
                  << "Checked Bags: " << checkedBags << "\n";

        if (!hotel.empty())
        {
            std::cout << "Hotel: " << hotel << " (" << nights << " nights)\n";
        }

        std::cout << "Add-ons: ";
        bool hasAddons = false;
        if (meals)
        {
            std::cout << "[Meals] ";
            hasAddons = true;
        }
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
        if (privateTour)
        {
            std::cout << "[Private Tour] ";
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

class TravelPackageBuilder
{
private:
    TravelPackage trip;

public:
    TravelPackageBuilder &from(std::string dep)
    {
        trip.departure = std::move(dep);
        return *this;
    }

    TravelPackageBuilder &to(std::string dest)
    {
        trip.destination = std::move(dest);
        return *this;
    }

    TravelPackageBuilder &setFlightClass(std::string fClass)
    {
        trip.flightClass = std::move(fClass);
        return *this;
    }

    TravelPackageBuilder &setHotel(std::string hotelName)
    {
        trip.hotel = std::move(hotelName);
        return *this;
    }

    TravelPackageBuilder &setNights(int n)
    {
        trip.nights = n;
        return *this;
    }

    TravelPackageBuilder &setCheckedBags(int bags)
    {
        trip.checkedBags = bags;
        return *this;
    }

    TravelPackageBuilder &setSeat(std::string seatPref)
    {
        trip.seat = std::move(seatPref);
        return *this;
    }

    TravelPackageBuilder &addMeals()
    {
        trip.meals = true;
        return *this;
    }
    TravelPackageBuilder &addAirportPickup()
    {
        trip.airportPickup = true;
        return *this;
    }
    TravelPackageBuilder &addTravelInsurance()
    {
        trip.travelInsurance = true;
        return *this;
    }
    TravelPackageBuilder &addCityTransport()
    {
        trip.cityTransport = true;
        return *this;
    }
    TravelPackageBuilder &addPrivateTour()
    {
        trip.privateTour = true;
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
            throw std::invalid_argument("Error: Airport limousine is only available for Business or First Class.");
        }

        TravelPackage result = std::move(trip);
        trip = TravelPackage();
        
        return result;
    }
};