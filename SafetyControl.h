#ifndef SAFETYCONTROL_H
#define SAFETYCONTROL_H

#include <string>
#include <iostream>

using namespace std;

class Elevator;

class SafetyControl {
public:
    SafetyControl(Elevator *elevator);
    const string &getActiveEvent() const;
    void setActiveEvent(string event);
    void handleHelpRequest();
    void handleFireAlarm();
    void handleOverload();
    void handlePowerOut();
private:
    string activeEvent;
    Elevator *elevator;
};

#endif // SAFETYCONTROL_H
