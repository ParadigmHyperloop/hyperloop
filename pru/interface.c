/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define COND 2
#define COUNT 4
#define PUSH_ACC 16
#define TRACK_LENGTH 1600
#define ACC_LENGTH 250
#define POD_MASS 750
#define SIM_TIME 65 // s
#define LOOP_DURATION 1000;

int message = 0;
int RELAYSTATE[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int RELAYS[] = {2, 3, 4, 5, 6, 7, 8, 9}; //,22,24,26,28,31,33,35,37};
int relTotal = 8;

double TCPin_1 = A8;
double TCPin_2 = A9;
double TCPin_3 = A10;
double TCPin_4 = A11;
double TCPin_5 = A12;
// double TCPin_6 = A6;
// double TCPin_7 = A7;
// double TCPin_8 = A8;

// initialize input values
double TCValue_1, TCValue_2, TCValue_3, TCValue_4, TCValue_5, TCValue_6,
    TCValue_7, TCValue_8;

// define final thermocouple values
double V_TC_1, V_TC_2, V_TC_3, V_TC_4, V_TC_5, V_TC_6, V_TC_7, V_TC_8;

// define final values
double HTC, ATC, BTC, CTC, DTC, TC6, TC7, TC8;

// define char for print
char T1, T2, T3, T4, T5, T6, T7, T8;

typedef struct {
  // Pod structure: contains all  pod data
  float x, vx, ax, z, mass, stripx, rpm, stripv, stripa; // enter vz and az
                                                         // after
  // Later: put in variables for storing data on all orientations
  float HP, AP, BP, CP, DP, PB1P, PB2P, PB3P, EB1tankP, EB1P, EB2tankP, EB2P,
      LC1P, LC2P; // Air system pressure values
  float HT, AT, BT, CT, DT, PB1T, PB2T, PB3T, EB1tankT, EB1T, EB2tankT, EB2T,
      LC1T, LC2T; // Air system temperature values
  int pusherOn, skatesOff, PBOn, EBOn;
  float xstop, xcritical;
} pod;

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  int ind;
  int setpin;
  /*  for (ind = 0; ind < relTotal; ind++) {
      pinMode(RELAYS[ind], OUTPUT);
    }
    for (setpin = 0; setpin < relTotal; setpin++) {
      digitalWrite(RELAYS[setpin], LOW); //Set all relays off
    }*/
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.println("Program active.");
}

// FUNCTIONS
void printCC(
    double a[][COUNT]); // Print out the current Conditions and Counters matrix
int loadCC(double a[][COUNT]); // Populate the C&C matrix by reading from the CC
                               // text file
int genPhys(
    pod *p, float t1,
    float t2); // Calculate net force and determine velocity and position of pod
int readAirSuppSensors(pod *p); // Read signals from air supply pressure
                                // transducers and thermocouples
int readTempSensors(pod *p);    // Read signals from the temperature sensors
int CCdecision(pod *p); // Make decisions based on whether collected data causes
                        // conditions to be satisfied
int transmit(pod *p);
void skateSolActivate();
void skateSolShutoff();

void loop() {
  int exitLoop = 0;
  Serial.println("Awaiting command.");
  message = Serial.read();
  if (message == 'A') {
    Serial.println("Run initiated.");
    RUN();
    exitLoop = 1;
  }
  if (message == 'B') {
    Serial.println("Manual relay control active.");
    TEST();
    exitLoop = 1;
  }
  if (message == 'C') {
    Serial.println("Standing by.");
    STANDBY();
    exitLoop = 1;
  }
  delay(1000);
}

int STANDBY() {
  int kill = 0;
  while (kill == 0) {
    float HP = map(analogRead(A0), 0, 1024, 0, 5000); // A
    float AP = map(analogRead(A1), 0, 1024, 0, 5000); // HP
    float BP = map(analogRead(A2), 0, 1024, 0, 5000); // D
    float CP = map(analogRead(A3), 0, 1024, 0, 5000); // B
    float DP = map(analogRead(A4), 0, 1024, 0, 5000); // C
    HP = HP / 1000;
    AP = AP / 1000;
    BP = BP / 1000;
    CP = CP / 1000;
    DP = DP / 1000;
    HP = 500.7849 * HP - 227.419;
    AP = 37.28 * AP - 3.2553;
    BP = 37.28 * BP - 3.2553;
    CP = 37.28 * CP - 3.2553;
    DP = 37.28 * DP - 3.2553;
    Serial.println(String(HP) + " " + String(AP) + " " + String(BP) + " " +
                   String(CP) + " " + String(DP));

    while (Serial.available() > 0) {
      message = Serial.read();

      if (message == 'a') {
        Serial.println("TEST ABORTED");
        while (Serial.read() > 0) {
          delay(10);
        }
        kill = 1;
      }
    }
    delay(1000);
  }
}
int RUN() {
  Serial.println("IN RUN");
  int kill = 0;
  pod poddy;
  pod *poddypointer;
  poddypointer = &poddy;

  // Initialize the pod struct physical properties
  poddy.x = 0;
  poddy.vx = 0;
  poddy.ax = 0;
  poddy.z = 0;
  poddy.mass = POD_MASS;
  poddy.pusherOn = 1; // Pusher is on initially
  poddy.PBOn = 0;     // Brakes are all off initially
  poddy.EBOn = 0;
  poddy.skatesOff = 0; // Skates are on initially
  // Create timers for use by genPhys function. These measure time before and
  // after all functions finish
  float t0 = float(millis()) / 1000, t1 = 0,
        t2 = 0;                    // to do: change t1 t2 to floats
  float timetrack = 0, tinc = 0.5; // tinc determines interval between printing
                                   // pod motion properties
  float simTime = SIM_TIME;
  int ABORT = 0;
  skateSolActivate();
  while (poddy.x <= TRACK_LENGTH &&
         kill == 0) { // while(simTime-t2 >= 0 && poddy.x <= TRACK_LENGTH){

    while (Serial.available() > 0) {
      message = Serial.read();

      if (message == 'a') {
        Serial.println("TEST ABORTED");
        while (Serial.read() > 0) {
          delay(10);
        }
        kill = 1;
      }
    }

    t1 = float(millis()) / 1000;
    if (t2 >= timetrack) {
      transmit(poddypointer);
      timetrack = timetrack + tinc;
    }

    // Filler code to take up time
    delay(200);
    //---------------------------
    readAirSuppSensors(poddypointer);
    readTempSensors(poddypointer);
    CCdecision(poddypointer);      // Make decision
    t2 = float(millis()) / 1000;   // Convert clock cycles 2 into time
    genPhys(poddypointer, t1, t2); // Generate physical data
  }
  if (poddy.x >= TRACK_LENGTH && poddy.vx >= 0) {
    Serial.println("COLLISION DETECTED");
  }
  if (poddy.x >= TRACK_LENGTH && poddy.vx >= 0 && poddy.vx < 5) {
    Serial.println("BUMP ON THE NOSE");
  }
  if (poddy.x >= TRACK_LENGTH && poddy.vx >= 5 && poddy.vx < 10) {
    Serial.println("TIS BUT A SCRATCH");
  }
  if (poddy.x >= TRACK_LENGTH && poddy.vx >= 10 && poddy.vx < 40) {
    Serial.println("DAMAGE: CRITICAL");
  }
  if (poddy.x >= TRACK_LENGTH && poddy.vx >= 40) {
    Serial.println("SMOLDERING WRECKAGE DETECTED");
  } else {
    Serial.println("SIMULATION COMPLETE");
    return 0;
  }
}
int genPhys(pod *p, float t1, float t2) {
  float Fnet, drag, fric, push;           // Forces acting on pod
  float rho = 0.0422, C = 0.34, A = 1.5;  // Constants for aero drag
  float mu = 0.0005 + p->PBOn * (0.3395); // friction coefficient. Changes
                                          // depending on whether primary brakes
                                          // on
  float g = 9.8;
  int pusherStopDistance =
      ACC_LENGTH; // Maximum distance to which pusher can push the pod
  float pushAcc =
      PUSH_ACC; // Expected magnitude of pod acceleration resulting from pusher
  float delT = t2 - t1; // Delta time
  int sign; // Used for determining direction of drag and friction forces

  // Check to see if the pusher should still be on
  if (p->x >= pusherStopDistance) {
    p->pusherOn = 0;
  }

  // Determine which direction the friction and drag force should be acting in.
  // This check results in an
  // oscillation between positive and negative forces, likely due to the
  // discrete time of the simulation.
  // ie, as the time interval goes to zero, the oscillation amplitude should go
  // to zero and the velocity
  // goes to zero, as it should.
  if (p->vx > 0) {
    sign = -1;
  } else if (p->vx < 0) {
    sign = 1;
  } else {
    sign = 0;
  }

  push = p->pusherOn * pushAcc * p->mass;        // Pusher force
  drag = sign * rho * C * A * p->vx * p->vx / 2; // Drag force
  fric =
      sign * (p->skatesOff * p->mass * g * mu +
              p->EBOn * p->mass * 10.4); // To do for later: Fn=mg-skateForce;

  Fnet = push + drag + fric; // Calculate net force
  // Update physical values of pod
  p->ax = Fnet / p->mass;
  p->vx = p->vx + p->ax * delT;
  p->x = p->x + p->vx * delT;
  return 0;
}
void skateSolShutoff() {
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
}
void skateSolActivate() {
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}
int CCdecision(pod *p) {
  float xremain =
      TRACK_LENGTH - p->x; // Distance from pod to the end of the track
  float podvx = p->vx;
  float podax = p->ax;
  float mindecel =
      -1 * podvx * podvx / (2 * xremain); // minimum deceleration required to
                                          // reach zero velocity by end of track
  float predPBfric =
      -9.8 * 0.34; // Predicted friction due to deceleration by primary brakes
  float predEBfric =
      -10.4; // Predicted friction due to deceleration by emergency brakes
  // Acceleration Phase
  if (p->pusherOn == 0) {
    // Coast phase
    if (podax >= mindecel) {
      if (p->PBOn == 0) {
        if (predPBfric >= mindecel) {
          p->PBOn = 1;
          p->skatesOff = 1;
          skateSolShutoff();
          // Serial.println("PRIMARY BRAKES ACTIVATED");
        }
      } else if (p->EBOn == 0) {
        if (predEBfric >= mindecel) {
          p->EBOn = 1;
          // Serial.println("EMERGENCY BRAKES ACTIVATED");
        }
      }
    } else if (podax <= 1.07 * mindecel &&
               p->PBOn == 1) { // If pod is decelerating too quickly (7% over),
                               // turn off the primary brakes
      p->PBOn = 0;
    } else {
      // Continue
    }
  } else {
    // Continue accelerating
  }
  return 0;
}
int transmit(pod *p) {
  String data = String(p->x) + " " + String(p->vx) + " " + String(p->ax) + " " +
                String(p->HP) + " " + String(p->AP) + " " + String(p->BP) +
                " " + String(p->CP) + " " + String(p->DP) + " " +
                String(p->HT) + " " + String(p->AT) + " " + String(p->BT) +
                " " + String(p->CT) + " " + String(p->DT);
  Serial.println(data);
  Serial.flush();
  return 0;
}
int readAirSuppSensors(pod *p) {
  float HP = map(analogRead(A0), 0, 1024, 0, 5000); // A
  float AP = map(analogRead(A1), 0, 1024, 0, 5000); // HP
  float BP = map(analogRead(A2), 0, 1024, 0, 5000); // D
  float CP = map(analogRead(A3), 0, 1024, 0, 5000); // B
  float DP = map(analogRead(A4), 0, 1024, 0, 5000); // C
  HP = HP / 1000;
  AP = AP / 1000;
  BP = BP / 1000;
  CP = CP / 1000;
  DP = DP / 1000;
  HP = 500.7849 * HP - 227.419;
  AP = 37.28 * AP - 3.2553;
  BP = 37.28 * BP - 3.2553;
  CP = 37.28 * CP - 3.2553;
  DP = 37.28 * DP - 3.2553;

  p->HP = HP;
  p->AP = AP;
  p->BP = BP;
  p->CP = CP;
  p->DP = DP;

  return 0;
}

int readTempSensors(pod *p) {
  TCValue_1 = analogRead(TCPin_1);
  TCValue_2 = analogRead(TCPin_2);
  TCValue_3 = analogRead(TCPin_3);
  TCValue_4 = analogRead(TCPin_4);
  TCValue_5 = analogRead(TCPin_5);
  TCValue_6 = 0; // analogRead(TCPin_6);
  TCValue_7 = 0; // analogRead(TCPin_7);
  TCValue_8 = 0; // analogRead(TCPin_8);

  // convert read values to voltage
  double voltage_1 = TCValue_1 * (5.0 / 1023.0);
  double voltage_2 = TCValue_2 * (5.0 / 1023.0);
  double voltage_3 = TCValue_3 * (5.0 / 1023.0);
  double voltage_4 = TCValue_4 * (5.0 / 1023.0);
  double voltage_5 = TCValue_5 * (5.0 / 1023.0);
  double voltage_6 = TCValue_6 * (5.0 / 1023.0);
  double voltage_7 = TCValue_7 * (5.0 / 1023.0);
  double voltage_8 = TCValue_8 * (5.0 / 1023.0);

  // routing analog inout values to the values used in the main equation
  double V_out_1 = voltage_1 + 0.02;
  double V_out_2 = voltage_2 + 0.02;
  double V_out_3 = voltage_3 + 0.02;
  double V_out_4 = voltage_4 + 0.02;
  double V_out_5 = voltage_5 + 0.02;
  double V_out_6 = voltage_6 + 0.02;
  double V_out_7 = voltage_7 + 0.02;
  double V_out_8 = voltage_8 + 0.02;

  // equation to calculate temperature

  V_TC_1 = (V_out_1 - 1.245) / 96.7;

  V_TC_2 = (V_out_2 - 1.245) / 96.7;

  V_TC_3 = (V_out_3 - 1.245) / 96.7;

  V_TC_4 = (V_out_4 - 1.245) / 96.7;

  V_TC_5 = (V_out_5 - 1.245) / 96.7;

  V_TC_6 = (V_out_6 - 1.245) / 96.7;

  V_TC_7 = (V_out_7 - 1.245) / 96.7;

  V_TC_8 = (V_out_8 - 1.245) / 96.7;

  // calculating temperature according to NIST tables

  // write to file

  // for thermocouple 1
  if (V_TC_1 >= 0) { // calculating temperatures greater than 0 degrees
    p->HT = (V_TC_1 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    p->HT = (V_TC_1 * 20553) + 0.3009;
  }

  // for thermocouple 2
  if (V_TC_2 >= 0) {
    p->AT = (V_TC_2 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    p->AT = (V_TC_2 * 20553) + 0.3009;
  }

  // for thermocouple 3
  if (V_TC_3 >= 0) {
    p->BT = (V_TC_3 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    p->BT = (V_TC_3 * 20553) + 0.3009;
  }
  // for thermocouple 4
  if (V_TC_4 >= 0) {
    p->CT = (V_TC_4 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    p->CT = (V_TC_4 * 20553) + 0.3009;
  }

  // for thermocouple 5
  if (V_TC_5 >= 0) {
    p->DT = (V_TC_5 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    p->DT = (V_TC_5 * 20553) + 0.3009;
  }

  // for thermocouple 6
  if (V_TC_6 >= 0) {
    TC6 = (V_TC_6 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    TC6 = (V_TC_6 * 20553) + 0.3009;
  }

  // for thermocouple 7
  if (V_TC_7 >= 0) {
    TC7 = (V_TC_7 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    TC7 = (V_TC_7 * 20553) + 0.3009;
  }

  // for thermocouple 8
  if (V_TC_8 >= 0) {
    TC8 = (V_TC_8 * 19333) + 0.2028;
  } else { // calculations for temperatures less than 0 degrees
    TC8 = (V_TC_8 * 20553) + 0.3009;
  }
  return 0;
  // end of calculations for temperature
  Serial.println(String(p->HT));
}

int TEST() {
  Serial.println("TESTING");
  int exitTest = 0;
  while (exitTest == 0) {
    message = Serial.read();
    if (message == 'a') {
      Serial.println("Exiting testing.");
      while (Serial.read() > 0) {
        delay(10);
      }
      exitTest = 1;
    }
    if (message == 'Q') {
      if (RELAYSTATE[0] == 0) {
        Serial.println("RELAY STATE WAS ON");
        digitalWrite(RELAYS[0], HIGH);
        RELAYSTATE[0] = 1;
      } else {
        Serial.println("RELAY STATE WAS ON");
        digitalWrite(RELAYS[0], LOW);
        RELAYSTATE[0] = 0;
      }
    }

    if (message == 'W') {
      if (RELAYSTATE[1] == 0) {
        digitalWrite(RELAYS[1], HIGH);
        RELAYSTATE[1] = 1;
      } else {
        digitalWrite(RELAYS[1], LOW);
        RELAYSTATE[1] = 0;
      }
    }

    if (message == 'E') {
      if (RELAYSTATE[2] == 0) {
        digitalWrite(RELAYS[2], HIGH);
        RELAYSTATE[2] = 1;
      } else {
        digitalWrite(RELAYS[2], LOW);
        RELAYSTATE[2] = 0;
      }
    }

    if (message == 'R') {
      if (RELAYSTATE[3] == 0) {
        digitalWrite(RELAYS[3], HIGH);
        RELAYSTATE[3] = 1;
      } else {
        digitalWrite(RELAYS[3], LOW);
        RELAYSTATE[3] = 0;
      }
    }

    if (message == 'T') {
      if (RELAYSTATE[4] == 0) {
        digitalWrite(RELAYS[4], HIGH);
        RELAYSTATE[4] = 1;
      } else {
        digitalWrite(RELAYS[4], LOW);
        RELAYSTATE[4] = 0;
      }
    }

    if (message == 'Y') {
      if (RELAYSTATE[5] == 0) {
        digitalWrite(RELAYS[5], HIGH);
        RELAYSTATE[5] = 1;
      } else {
        digitalWrite(RELAYS[5], LOW);
        RELAYSTATE[5] = 0;
      }
    }

    if (message == 'U') {
      if (RELAYSTATE[6] == 0) {
        digitalWrite(RELAYS[6], HIGH);
        RELAYSTATE[6] = 1;
      } else {
        digitalWrite(RELAYS[6], LOW);
        RELAYSTATE[6] = 0;
      }
    }

    if (message == 'I') {
      if (RELAYSTATE[7] == 0) {
        digitalWrite(RELAYS[7], HIGH);
        RELAYSTATE[7] = 1;
      } else {
        digitalWrite(RELAYS[7], LOW);
        RELAYSTATE[7] = 0;
      }
    }

    /* if (message == 'q') {
       if (RELAYSTATE[8] == 0) {
         digitalWrite(RELAYS[8], HIGH);
         RELAYSTATE[8] = 1;
       }
       else {
         digitalWrite(RELAYS[8], LOW);
         RELAYSTATE[8] = 0;
       }
     }

     if (message == 'w') {
       if (RELAYSTATE[9] == 0) {
         digitalWrite(RELAYS[9], HIGH);
         RELAYSTATE[9] = 1;
       }
       else {
         digitalWrite(RELAYS[9], LOW);
         RELAYSTATE[9] = 0;
       }
     }

     if (message == 'e') {
       if (RELAYSTATE[10] == 0) {
         digitalWrite(RELAYS[10], HIGH);
         RELAYSTATE[10] = 1;
       }
       else {
         digitalWrite(RELAYS[10], LOW);
         RELAYSTATE[10] = 0;
       }
     }

     if (message == 'r') {
       if (RELAYSTATE[11] == 0) {
         digitalWrite(RELAYS[11], HIGH);
         RELAYSTATE[11] = 1;
       }
       else {
         digitalWrite(RELAYS[11], LOW);
         RELAYSTATE[11] = 0;
       }
     }

     if (message == 't') {
       if (RELAYSTATE[12] == 0) {
         digitalWrite(RELAYS[12], HIGH);
         RELAYSTATE[12] = 1;
       }
       else {
         digitalWrite(RELAYS[12], LOW);
         RELAYSTATE[12] = 0;
       }
     }

     if (message == 'y') {
       if (RELAYSTATE[13] == 0) {
         digitalWrite(RELAYS[13], HIGH);
         RELAYSTATE[13] = 1;
       }
       else {
         digitalWrite(RELAYS[13], LOW);
         RELAYSTATE[13] = 0;
       }
     }

     if (message == 'u') {
       if (RELAYSTATE[14] == 0) {
         digitalWrite(RELAYS[14], HIGH);
         RELAYSTATE[14] = 1;
       }
       else {
         digitalWrite(RELAYS[14], LOW);
         RELAYSTATE[14] = 0;
       }
     }

     if (message == 'i') {
       if (RELAYSTATE[15] == 0) {
         digitalWrite(RELAYS[15], HIGH);
         RELAYSTATE[15] = 1;
       }
       else {
         digitalWrite(RELAYS[15], LOW);
         RELAYSTATE[15] = 0;
       }
     }*/
  }
  return 0;
}
