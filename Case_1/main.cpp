#include "FlySmart.hpp"
#include <iostream>

int main()
{
    TravelPackageBuilder builder;
    try
    {
        auto honeymoon = builder.from("Ho Chi Minh City")
                             .to("Paris")
                             .setFlightClass("Business")
                             .setHotel("Grand Hotel")
                             .setNights(5)
                             .addAirportPickup()
                             .addTravelInsurance()
                             .addPrivateTour()
                             .build();
        auto businessTrip = builder
                                .from("Ho Chi Minh City")
                                .to("Singapore")
                                .setFlightClass("Economy")
                                .setHotel("City Hotel")
                                .setNights(2)
                                .build();

        honeymoon.printDetails();
        businessTrip.printDetails();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}