#include "SafetyControl.h"
#include "Elevator.h"

SafetyControl::SafetyControl(Elevator *elevator) : activeEvent("None"), elevator(elevator) {}

const string &SafetyControl::getActiveEvent() const {
    return activeEvent;
}

void SafetyControl::setActiveEvent(string event) {
    activeEvent = event;
}

void SafetyControl::handleHelpRequest() {
    if (activeEvent == "None") {
        activeEvent = "Help";
        elevator->displayMessage("HELP REQUESTED: Stopping elevator and contacting emergency services");
        elevator->stopMovement();
        cout << "HELP BUTTON PRESSED - Elevator " << elevator->getId()
             << " stopped at Floor " << elevator->getCurrentFloor() << endl;
    }
}

void SafetyControl::handleFireAlarm() {
    if (activeEvent == "None" || activeEvent == "Help" ||
        activeEvent == "Overload" || activeEvent == "PowerOut") {
        activeEvent = "Fire";
        elevator->displayMessage("EMERGENCY: Moving to floor 1");
        elevator->stopMovement();
        elevator->addRequests(1);
        elevator->setState("moving");
    }
}

void SafetyControl::handleOverload() {
    activeEvent = "Overload";
    elevator->displayMessage("Overload Detected. Please reduce load...");
    elevator->stopMovement();
}

void SafetyControl::handlePowerOut() {
    if (activeEvent == "None" || activeEvent == "Help" || activeEvent == "Overload") {
        activeEvent = "PowerOut";
        elevator->displayMessage("POWER OUTAGE: Moving to safe floor");
        elevator->stopMovement();
        elevator->moveToSafeFloor(1);
        cout << "POWER OUTAGE: Elevator " << elevator->getId()
             << " moving to floor 1" << endl;
    }
}

