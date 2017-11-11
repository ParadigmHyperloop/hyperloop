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


class TestStateMachine(unittest.TestCase):

    def setUp(self):
        if CONTROLLER_PATH is None:
            self.fail("Could not find core: ".format(PATHS))

        self.controller = ControllerInstance(CONTROLLER_PATH, imu="-",
                                             POST=True)

        self.assertTrue(self.controller.start())

    def tearDown(self):
        self.controller.shutdown()

    def test_starts_in_boot(self):
        res = self.controller.command(['state'])
        self.assertIn('Boot', res)

    def test_can_fill(self):
        res = self.controller.command(['fill'])
        self.assertIn('Entered HP Fill', res)

    def test_can_standby(self):
        res = self.controller.command(['state', 'Standby'])
        self.assertIn('Standby', res)

    def test_can_emergency(self):
        res = self.controller.command(['emergency'])
        self.assertIn('Pod Mode: 11', res)

    def test_can_e(self):
        res = self.controller.command(['e'])
        self.assertIn('Pod Mode: 11', res)


if __name__ == '__main__':
    unittest.main()
