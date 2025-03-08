#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "Door.h"
#include "Passenger.h"
#include "SafetyControl.h"
#include "Floor.h"
#include "Logger.h"
#include <queue>
#include <vector>
#include <string>
#include <QObject>
#include <QTimer>
#include <QPointer>
#include <iostream>

using namespace std;

using std::queue;
using std::vector;
using std::string;

class Elevator : public QObject {
    Q_OBJECT
public:
    Elevator(int id, int capacity, Door &door);

    int getCurrentFloor() const;
    const string &getState() const;
    const vector<Passenger> &getPassengers() const;
    int getId() const;
    vector<Passenger> &getPassengers();
    SafetyControl &getSafetyControl();

    void openDoor();
    void closeDoor();
    void handleAutoClose();
    void addRequests(int floor, bool fromInside);
    void addRequests(int floor); // Overloaded
    void moveToFloor();
    void handlePassengers(Floor &floor);
    void addPassenger(const Passenger &passenger);
    void removePassenger(int passengerId);
    void displayMessage(const string &message);
    void pauseMovement();
    void stopMovement();
    void moveToSafeFloor(int safeFloor);
    void pressHelpButton();
    void handleOverload();
    bool hasJustArrived() const;
    void setState(string state);

    void triggerOverload();
    void resolveOverload();
    bool isInOverload() const;

    int doorOpenSteps;

private:
    int id;
    int capacity;
    int currentFloor;
    string state;
    bool isOverloaded;
    bool arrived;
    bool justStartedMoving;
    Door &door;
    queue<int> floorRequests;
    vector<Passenger> passengers;
    SafetyControl safetyControl;
};

#endif // ELEVATOR_H
