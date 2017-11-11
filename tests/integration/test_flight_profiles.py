import os
import time
import unittest

from hyperloop import ControllerInstance


PATHS = [
    os.environ.get("CORE_LOCATION", None),
    "../../proj/BUILD/dst/usr/local/bin/core",
    "../../proj/core/core",
    "./BUILD/dst/usr/local/bin/core",
    "./proj/BUILD/dst/usr/local/bin/core",
    "./proj/core/core",
]

CONTROLLER_PATH = None

for path in PATHS:
    if path and os.path.exists(path):
        CONTROLLER_PATH = path
        break


class TestFlightProfiles(unittest.TestCase):

    def setUp(self):
        if CONTROLLER_PATH is None:
            self.fail("Could not find core: ".format(PATHS))

        self.controller = ControllerInstance(CONTROLLER_PATH, imu="-",
                                             POST=True)

        self.assertTrue(self.controller.start())

    def tearDown(self):
        self.controller.shutdown()

    def test_fpget(self):
        self.controller.command(['state', '1'])
        self.controller.command(['fp', '-w', '32000000'])
        res = self.controller.command(['fpget'])
        correct_res = """watchdog_timer: 32000000
emergency_hold: 30000000
braking_wait: 2000000
pusher_timeout: 1000000
pusher_state_accel_min: 0.250000
pusher_state_min_timer: 24000000
pusher_distance_min: 70.000000
primary_braking_accel_min: -6.250000

"""
        self.assertTrue(res == correct_res)

    def test_fp_boot(self):
        self.controller.command(['state', '1'])
        res = self.controller.command(['fp', '-w', '100'])
        correct_res = """watchdog_timer: 100
emergency_hold: 30000000
braking_wait: 2000000
pusher_timeout: 1000000
pusher_state_accel_min: 0.250000
pusher_state_min_timer: 24000000
pusher_distance_min: 70.000000
primary_braking_accel_min: -6.250000

"""
        self.assertTrue(res == correct_res)

    def test_fp_not_boot(self):
        self.controller.command(['state', '2'])
        res = self.controller.command(['fp', '-w', '100'])
        correct_res = "Cannot configure flight profiles in mode 2 (HPFill). Pod must be in Boot mode.\n"
        self.assertTrue(res == correct_res)

    def test_fp_invalid_opt(self):
        self.controller.command(['state', '1'])
        res = self.controller.command(['fp', '-x', 'aksfl;a'])
        correct_res = "Invalid Argument(s)\n"
        self.assertTrue(res == correct_res)

    def test_fp_invalid_arg(self):
        self.controller.command(['state', '1'])
        res = self.controller.command('fp', '-w', 'alfkjal'])
        correct_res = """watchdog_timer: 0
emergency_hold: 30000000
braking_wait: 2000000
pusher_timeout: 1000000
pusher_state_accel_min: 0.250000
pusher_state_min_timer: 24000000
pusher_distance_min: 70.000000
primary_braking_accel_min: -6.250000

"""
        self.assertTrue(res == correct_res)


if __name__ == '__main__':
    unittest.main()
