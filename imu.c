#include "pod.h"
#include "main.c"

long maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
long distanceBeforeBraking = 100; //CHANGE ME! ARBITRARY!
long standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!

long acceleration = 0; //In m/s^2
long forwardVelocity = 0; //In m/s
long totalDistanceTraveled = 0; //In m
long lastCheckTime = getTime(); //Should be the method from main to get the current system time, assume milliseconds.
long timeSinceLast = 0; //In milliseconds

long accelToVelocity() {
    return (acceleration * timeSinceLast / 1000)
}

long velocityToDistance() {
    return (forwardVelocity * timeSinceLast / 1000)
}

void pushingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        continue; //CHANGE ME!!! Make state Emergency
    }
    else if (acceleration <= 0) {
        continue; //CHANGE ME!!! Make state Coasting
    }
}

void coastingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        continue; //CHANGE ME!!! Make state Emergency
    }
    else if (totalDistanceTraveled > standardDistanceBeforeBraking) {
        continue; //CHANGE ME!! Make state Braking
    }
}

void brakingChecks() {
    //QUESTION: Are there emergency state criteria for Braking state?
    if (forwardVelocity <= 0) {
        continue; //CHANGE ME!! Make state Shutdown
    }
}

int main() {
    while (1) {
        long currentCheckTime = getTime(); //Same as above, assume milliseconds
        timeSinceLast = lastCheckTime - currentCheckTime;
        lastCheckTime = currentCheckTime;
        
        acceleration = 0; //CHANGE ME!!! Get acceleration from Sensor assume m/s^2. Assume long.
        forwardVelocity = accelToVelocity();
        totalDistanceTraveled += velocityToDistance();
        
        podState = Ready; //CHANGE ME!!! Should read from the pthread.
        
        switch(podState) {
            case Pushing :
                pushingChecks();
                break;
            case Coasting :
                coastingChecks();
                break;
            case Braking :
                brakingChecks();
                break;
            default :
                continue;
        }
    }
}
