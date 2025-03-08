#ifndef FLOOR_H
#define FLOOR_H

#include "Passenger.h"
#include <vector>
#include <algorithm>

using namespace std;

class Floor {
public:
    Floor(int floorNumber);

    void pressUpButton();
    void pressDownButton();
    void resetButtons();
    void addPassenger(const Passenger &passenger);
    void removePassenger(int passengerId);

    bool isUpButtonPressed() const;
    bool isDownButtonPressed() const;
    int getFloorNumber() const;
    const vector<Passenger> &getPassengers() const;
    vector<Passenger> &getPassengers();

    bool isCallAssigned() const;
    void setCallAssigned(bool assigned);
private:
    int floorNumber;
    bool upButtonPressed;
    bool downButtonPressed;
    bool callAssigned;
    vector<Passenger> passengers;
};

#endif // FLOOR_H
