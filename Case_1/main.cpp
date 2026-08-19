#include "FlySmart.hpp"
#include <iostream>

int main()
{
    TravelPackageBuilder Travel_Package;
    Director dir(Travel_Package);
    try
    {
        auto Budget_Traveler = dir.buildBudgetTravelerPackage("Ho Chi Minh", "Ha Noi");
        auto Business_Traveler = dir.buildBusinessTravelerPackage("Ho Chi Minh", "Ha Noi");
        auto Luxury_Traveler = dir.buildLuxuryTravelerPackage("Ho Chi Minh", "Ha Noi");

        Budget_Traveler.printDetails();
        Business_Traveler.printDetails();
        Luxury_Traveler.printDetails();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}