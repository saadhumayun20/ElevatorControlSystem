#include "Floor.h"

Floor::Floor(int floorNumber)
    : floorNumber(floorNumber), upButtonPressed(false), downButtonPressed(false), callAssigned(false) {}

void Floor::pressUpButton() {
    if (upButtonPressed) return;
    upButtonPressed = true;
}

void Floor::pressDownButton() {
    if (downButtonPressed) return;
    downButtonPressed = true;
}

void Floor::resetButtons() {
    upButtonPressed = false;
    downButtonPressed = false;
    callAssigned = false;
}

void Floor::addPassenger(const Passenger &passenger) {
    passengers.push_back(passenger);
}

void Floor::removePassenger(int passengerId) {
    auto it = find_if(passengers.begin(), passengers.end(), [passengerId](const Passenger &p) {
        return p.getId() == passengerId;
    });
    if (it != passengers.end()) {
        passengers.erase(it);
    }
}

bool Floor::isUpButtonPressed() const {
    return upButtonPressed;
}

bool Floor::isDownButtonPressed() const {
    return downButtonPressed;
}

int Floor::getFloorNumber() const {
    return floorNumber;
}

const vector<Passenger> &Floor::getPassengers() const {
    return passengers;
}

vector<Passenger> &Floor::getPassengers() {
    return passengers;
}

bool Floor::isCallAssigned() const {
    return callAssigned;
}

void Floor::setCallAssigned(bool assigned) {
    callAssigned = assigned;
}
