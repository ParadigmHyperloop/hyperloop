// Written by OpenLoop Hyperloop Team, August 2016

#include "libBBB.h"
#include <pthread.h>
#include <sys/time.h>

// methods of pod control
const int TEST_MODE = 1;
const int PUSHER_PHASE = 2;
const int COASTING_PHASE = 3;
const int BRAKING_PHASE = 4;


// locks sensor data (imu, distance, photoelectric)
pthread_mutex_t sensorDataMutex;
// locks states (pos, vel, quaternions)
pthread_mutex_t statesMutex;
// locks desired positions of plants
pthread_mutex_t plantCommandMutex;
// locks pof phase (testing, pusher, coasting, braking, etc)
pthread_mutex_t podPhaseMutex;
// locks emergency brake and lat control commands
pthread_mutex_t emergencyFlagMutex;


// variables to keep track of when each task started, used
// to make sure they stay consistent with periods
long kalmanStart = 0;
long photoelectricStart = 0;
long distanceStart = 0;
long imuStart = 0;
long lateralControlStart = 0;
long brakingStart = 0;
long dataDisplayStart = 0;

long kalmanPeriod = 1000;
long photoelectricPeriod = 200;
long distancePeriod = 5000;
long imuPeriod = 1000;
long lateralControlPeriod = 15000;
long brakingPeriod = 10000;
long dataDisplayPeriod = 20000;


// returns the time in microseconds
long getTime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

// pos, vel, quaternions
double states[10];
// acc, rotvel
double IMUData[6];
// 8 distance sensors
double distanceSensorData[8];
// 9 PE sensors
double photoelectricSensorData[9];

// isset flags are needed for kalman.
// When sensors read, set to 1. When kalman is used, set all to 0
int imuSet = 0;
int distanceSensorSet = 0;
int photoelectricSet = 0;

// emergency control flags from command point
int forcedBreak;
int forcedEmergencyBreak;
int forcedLateralCorrect;


void *kalmanFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		kalmanStart = getTime();

		pthread_mutex_lock(&sensorDataMutex);
		pthread_mutex_unlock(&sensorDataMutex);

		// Kalman code goes here
		
		pthread_mutex_lock(&statesMutex);
		imuSet = 0;
		distanceSensorSet = 0;
		photoelectricSet = 0;
		pthread_mutex_unlock(&statesMutex);

		timespent = getTime() - kalmanStart;
		if (timespent > kalmanPeriod )
				printf("Error: Kalman failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(kalmanPeriod - timespent);
	}
}

void *photoelectricFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		photoelectricStart = getTime();
		// get photoelectric data
		// calculate stuff
		pthread_mutex_lock(&sensorDataMutex);
		photoelectricSet = 1;
		pthread_mutex_unlock(&sensorDataMutex);

		timespent = getTime() - photoelectricStart;
		if (timespent > photoelectricPeriod )
				printf("Error: photoelectric failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(photoelectricPeriod - timespent);
	}
}

void *distanceSensorFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		distanceStart = getTime();
		// get distance sensor data
		// calculate stuff
		pthread_mutex_lock(&sensorDataMutex);
		distanceSensorSet = 1;
		pthread_mutex_unlock(&sensorDataMutex);

		timespent = getTime() - distanceStart;
		if (timespent > distancePeriod )
				printf("Error: distance sensor failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(distancePeriod - timespent);
	}
}

void *imuDataFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		imuStart = getTime();
		// get IMU data
		pthread_mutex_lock(&sensorDataMutex);
		imuSet = 1;
		pthread_mutex_unlock(&sensorDataMutex);

		timespent = getTime() - imuStart;
		if (timespent > imuPeriod )
				printf("Error: imu failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(imuPeriod - timespent);
	}
}

void *lateralControlFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		lateralControlStart = getTime();
		pthread_mutex_lock(&emergencyFlagMutex);
		pthread_mutex_unlock(&emergencyFlagMutex);

		pthread_mutex_lock(&statesMutex);
		pthread_mutex_unlock(&statesMutex);

		// determine lateral control plan

		timespent = getTime() - lateralControlStart;
		if (timespent > lateralControlPeriod )
				printf("Error: lat control failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(lateralControlPeriod - timespent);
	}
}

void *brakingFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		brakingStart = getTime();
		pthread_mutex_lock(&emergencyFlagMutex);
		pthread_mutex_unlock(&emergencyFlagMutex);

		pthread_mutex_lock(&statesMutex);
		pthread_mutex_unlock(&statesMutex);

		// determine braking

		timespent = getTime() - brakingStart;
		if (timespent > brakingPeriod )
				printf("Error: braking failed to meet Deadline by %li nanosecons\n", timespent);
		else
			usleep(brakingPeriod - timespent);
	}
}

void *DataDisplayFunction(void *arg) {
	long timespent = 0; 
	while(1) {
		dataDisplayStart = getTime();
		pthread_mutex_lock(&sensorDataMutex);
		// display shit
		pthread_mutex_unlock(&sensorDataMutex);

		pthread_mutex_lock(&statesMutex);
		// display shit
		pthread_mutex_unlock(&statesMutex);

		timespent = getTime() - dataDisplayStart;
		if (timespent > dataDisplayPeriod )
				printf("Error: data display failed to meet Deadline by %li nanosecons\n", timespent);
		else
		usleep(dataDisplayPeriod - timespent);
	}
}


void setPriority(pthread_t task, int priority) {
	struct sched_param sp;
    sp.sched_priority = priority;
    if(pthread_setschedparam(task, SCHED_RR, &sp)){
            fprintf(stderr,"WARNING: failed to set thread"
                    "to real-time priority\n");
    }
}

int main()
{
	pthread_mutex_init(&sensorDataMutex, NULL);
	pthread_mutex_init(&statesMutex, NULL);
	pthread_mutex_init(&podPhaseMutex, NULL);
	pthread_mutex_init(&plantCommandMutex, NULL);
	pthread_mutex_init(&emergencyFlagMutex, NULL);

	pthread_t kalman;
	pthread_t photoelectric;
	pthread_t imu;
	pthread_t distance;
	pthread_t braking;
	pthread_t lateralControl;
	pthread_t dataDisplay;

	pthread_create(&kalman, NULL, kalmanFunction, NULL);
	pthread_create(&photoelectric, NULL, photoelectricFunction, NULL);
	pthread_create(&imu, NULL, imuDataFunction, NULL);
	pthread_create(&distance, NULL, distanceSensorFunction, NULL);
	pthread_create(&braking, NULL, brakingFunction, NULL);
	pthread_create(&lateralControl, NULL, lateralControlFunction, NULL);
	pthread_create(&dataDisplay, NULL, DataDisplayFunction, NULL);


	// we're using the built-in linux Round Roboin scheduling
	// priorities are 1-99, higher is more important
	// important not: this is not hard real-time
	setPriority(kalman, 30);
	setPriority(photoelectric, 30);
	setPriority(imu, 30);
	setPriority(distance, 25);
	setPriority(braking, 30);
	setPriority(lateralControl, 25);
	setPriority(dataDisplay, 15);

	while(1){

		usleep(1000);
		fflush(stdout);
	}

	pthread_mutex_destroy(&sensorDataMutex);
	pthread_mutex_destroy(&statesMutex);
	pthread_mutex_destroy(&podPhaseMutex);
	pthread_mutex_destroy(&plantCommandMutex);
	pthread_mutex_destroy(&emergencyFlagMutex);

	return 0;
}
