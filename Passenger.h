#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>
using namespace std;

class Elevator;
class Floor;

class Passenger {
public:
    Passenger(int id, int currentFloor);

    int getCurrentFloor() const;
    int getDestinationFloor() const;
    int getId() const;
    bool isInElevator() const;

    void requestElevator(int direction, Floor &floor);
    void selectDestination(int floor, Elevator &elevator);
    void enterElevator(int elevatorFloor);
    void exitElevator(int destinationFloor);
    void setDestinationFloor(int floor);
private:
    int id;
    int currentFloor;
    int destinationFloor; // -1 indicates no destination.
};

#endif // PASSENGER_H

