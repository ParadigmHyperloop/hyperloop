#include "pod.h"

long readPhotoelectricSensor(int sensorno) {
    //TODO: Actually read in phoeletric value
    return 0;
}

void photoelectricCheck(pod_state_t *podState) {
    //Are there any checks we want to make with the photo-electric sensor?
}

void *photoelectricMain(void *arg) {
    debug("[photoelectricMain] Thread Start");
    pod_state_t *podState = getPodState();
    while(1) {
        setPodField(&(podState->photoelectric_r), readPhotoelectricSensor(0)); //TODO: read in photoElectric value
        setPodField(&(podState->photoelectric_g), readPhotoelectricSensor(1));
        setPodField(&(podState->photoelectric_b), readPhotoelectricSensor(2));
        
        photoelectricCheck(podState);
        
        usleep(PHOTOELECTRIC_THREAD_SLEEP);
    }
}
