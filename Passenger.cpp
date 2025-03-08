#include "Passenger.h"
#include "Elevator.h"
#include "Floor.h"

Passenger::Passenger(int id, int currentFloor) : id(id), currentFloor(currentFloor), destinationFloor(-1) {}

int Passenger::getCurrentFloor() const {
    return currentFloor;
}

int Passenger::getDestinationFloor() const {
    return destinationFloor;
}

int Passenger::getId() const {
    return id;
}

bool Passenger::isInElevator() const {
    return currentFloor == -1;
}

void Passenger::requestElevator(int direction, Floor &floor) {
    if (direction == 1)
        floor.pressUpButton();
    else
        floor.pressDownButton();
}

void Passenger::selectDestination(int floor, Elevator &elevator) {
    destinationFloor = floor;
    elevator.addRequests(floor, true);
}

void Passenger::enterElevator(int elevatorFloor) {
    if (currentFloor == elevatorFloor)
        currentFloor = -1;
}

void Passenger::exitElevator(int destinationFloor) {
    currentFloor = destinationFloor;
    destinationFloor = -1;
}

void Passenger::setDestinationFloor(int floor) {
    destinationFloor = floor;
}

