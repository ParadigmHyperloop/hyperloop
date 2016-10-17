#include "pod.h"
#include <termios.h>

int imuFd = -1;

typedef struct {
  // All units are based in meters
  uint32_t header;
  float x;
  float y;
  float z;
  float wx;
  float wy;
  float wz;
  uint8_t status;
  uint8_t sequence;
  float temperature;
} imu_datagram_t;

// example datagram, MSB is always printed first
// uint8_t example[32] = {
//   0xFE, 0x81, 0xFF, 0x55, // Message Header, exactly these bytes
//   0x01, 0x23, 0x45, 0x65, // X rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Y rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z linear (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Y linear  (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z linear  (4 Bytes to form a float)
//   0xEE, // Status (each bit wx, wy, wz, reserved, ax, ay, az, reserved)
//   0xFF, // sequence number (0-127 wraps)
//   0x00, 0x01 // temperature bits (UInt16)
// };

bool atHeader() {
  unsigned char firstByte[1] = {0};
  // TODO: Remove these asserts and log errors instead, Definitely when
  // O_NONBLOCK is used
  assert(read(imuFd, firstByte, 1) == 1);

  debug("IMU First Byte%x\n", *firstByte);

  if (*firstByte == 0xFE) {
    unsigned char restOfHeader[3] = {0};
    assert(read(imuFd, restOfHeader, 3) == 3);

    unsigned char ideal[3] = {0x81, 0xFF, 0x55};

    int i;
    for (i = 0; i < 3; i++) {
      printf("%x\n", restOfHeader[i]);
      if (restOfHeader[i] != ideal[i]) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}


extern pod_state_t __state;

int readIMUDatagram(uint64_t t, imu_datagram_t *gram) {
#ifdef TESTING

  // Lets have a chat together... this testing simulator... is terrible...
  // It does not follow the laws of physics to the nail, but it is something
  // that excersises the system with minimal impact on the __state.
  // So... I recognize that this is terrible. I will fix it into a proper sim
  // when there is time

  #define NOISE (float)(rand() - RAND_MAX/2)/(RAND_MAX*100.0)
  static float base_ax = 0.0; // NOTE: preserved accross calls
  static uint64_t pushing = 0;
  static uint64_t coasting = 0;
  static bool moving = false;
  float vx = __state.velocity_x.value.fl;
  // If ready, start pushing at some random time
  if (getPodMode() == Ready) {
    if (rand() % 7 == 0) {
      base_ax = 2.0;
      pushing = t; // Turn on the pusher for future calls
    }
  } else if (pushing > 0) {
    moving = true;
    base_ax = 2.0;

    uint64_t q = t - pushing; // The time period after pusher has started to back off
    debug("[SIM] PUSHER IS ON => usecs: %llu\n", q);
    if (q > 5 * 1000 * 1000) { // After 5 seconds of pushing
      base_ax = 6.0 - ((float)q / (1000.0 * 1000.0));
      if (q > 6 * 1000 * 1000) {
        pushing = 0; // Turn off the pusher after 6 seconds of pushing
        coasting = t; // Turn off the pusher after 6 seconds of pushing
      }
    }
  } else if (coasting) { // or braking
    base_ax = -1.0;
  }

  float ax = base_ax;
  if (__state.tmp_brakes == 1 && moving) {
    debug("Brakes applied");
    ax -= 7.0;

    if (vx < -2*ax) {
      ax = -(vx/2.0);
    }
    if (vx < 0) {
      error("[SIM] NEGATIVE VELOCITY");
    }
  }




  ax += NOISE;
  debug("[SIM] Giving => x: %f, y: **, z: **, wx: **, wy: **, wz: **,\n", ax);
  *gram = (imu_datagram_t){
    .x = ax,
    .y = NOISE,
    .z = NOISE,
    .wx = NOISE,
    .wy = NOISE,
    .wz = NOISE
  };
  return 0;

#else

  retries = 0;
  bool at_header = false;
  while (!(at_header = atHeader()) && retries < 33)
    retries++;

  if (!at_header) {
    error("Could not find an IMU data header");
    return -1;
  }

  char buf[32];

  int count = 0;
  int n = 32;
  // Force read an entire packet
  // read() is not garenteed to give you all n bytes
  while (count < n) {
    int new_count = read(imuFd, buf, n - count);

    if (new_count <= count) {
      count = -1;
      break;
    }

    count += new_count;
  }

  assert(count == 32);

  *gram = {.header = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24),
           .wx = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24),
           .wy = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24),
           .wz = buf[12] | (buf[13] << 8) | (buf[14] << 16) | (buf[15] << 24),
           .x = buf[16] | (buf[17] << 8) | (buf[18] << 16) | (buf[19] << 24),
           .y = buf[20] | (buf[21] << 8) | (buf[22] << 16) | (buf[23] << 24),
           .z = buf[24] | (buf[25] << 8) | (buf[26] << 16) | (buf[27] << 24),
           .status = buf[28],
           .sequence = buf[29],
           .temperature = buf[30] | (buf[31] << 8)};
  return 0;
#endif
}

// Connect the serial device for the IMU
int imuConnect() {
  imuFd = open(IMU_DEVICE, O_RDWR, S_IRUSR | S_IWUSR);

  if (imuFd < 0) {
    return -1;
  }

  struct termios pts;
  tcgetattr(imuFd, &pts);
  /*make sure pts is all blank and no residual values set
  safer than modifying current settings*/
  pts.c_lflag = 0; /*implies non-canoical mode*/
  pts.c_iflag = 0;
  pts.c_oflag = 0;
  pts.c_cflag = 0;

  // Baud Rate (This is super high by default)
  pts.c_cflag |= 921600;

  // 8 Data Bits
  pts.c_cflag |= CS8;

  pts.c_cflag |= CLOCAL;
  pts.c_cflag |= CREAD;

  // Set the terminal device atts
  tcsetattr(imuFd, TCSANOW, &pts);

  return imuFd;
}

int calcState(pod_value_t * a,
              pod_value_t * v,
              pod_value_t * x,
              float new_accel,
              double dt) {

  float acceleration = getPodField_f(a);
  float velocity = getPodField_f(v);
  float position = getPodField_f(x);

  // Calculate the new_velocity (oldv + (olda + newa) / 2)

  // float dv = calcDu(dt, acceleration, new_accel);
  float dv = ((dt * ((acceleration + new_accel) / 2)) / 1000000.0);
  float new_velocity = (velocity + dv);

  // float dx = calcDu(dt, velocity, new_velocity);
  float dx = ((dt * ((velocity + new_velocity) / 2)) / 1000000.0);
  float new_position = (position + dx);

  debug("dt: %lf us, dv: %f m/s, dx: %f m\n", dt, dv, dx);

  setPodField_f(a, new_accel);
  setPodField_f(v, new_velocity);
  setPodField_f(x, new_position);

  return 0;
}

/**
 * Reads data from the IMU, computes the new Acceleration, Velocity, and
 * Position state values
 */
int imuRead(pod_state_t *p) {

  static uint64_t start_time = 0;
  static uint64_t lastCheckTime = 0;

  // Intiializes local static varaibles
  if (start_time == 0) {
    lastCheckTime = getTime();
    start_time = getTime();
    usleep(1);
  }

  uint64_t currentCheckTime = getTime(); // Same as above, assume milliseconds

  imu_datagram_t data;

  if (readIMUDatagram(currentCheckTime - start_time, &data) < 0) {
    return -1;
  }

  assert((currentCheckTime - lastCheckTime) < INT32_MAX);

  double dt = (double)(currentCheckTime - lastCheckTime);
  lastCheckTime = currentCheckTime;

  if (dt <= 0.0) {
    error("time scince last check <= 0.0, this should not happen");
    return -1;
  }

  calcState(&(p->accel_x), &(p->velocity_x), &(p->position_x), data.x, dt);
  calcState(&(p->accel_y), &(p->velocity_y), &(p->position_y), data.y, dt);
  calcState(&(p->accel_z), &(p->velocity_z), &(p->position_z), data.z, dt);

  return 0;
}
