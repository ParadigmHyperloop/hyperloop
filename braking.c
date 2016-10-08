#include "pod.h"

void *brakingMain(void *arg) {
    pod_mode_t podState;
    while(1) {
        podState = getPodState(); //CHANGE ME! Get current pod state
        switch (podState) {
            case Braking:
                // MAKE THE BRAKES BRAKE!?!?
                break;
            case Emergency:
                // DO THE EMERGENCY STOP PROTOCOL
                break;
                
            default:
                break;
        }
    }
}
