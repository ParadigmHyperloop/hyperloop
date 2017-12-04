import os
import time
import unittest

from controller_instance import ControllerInstance

class ControllerTests(unittest.TestCase):

    def setUp(self):
        self.PATHS = [
            os.environ.get("CORE_LOCATION", None),
            "../../proj/BUILD/dst/usr/local/bin/core",
            "../../proj/core/core",
            "./BUILD/dst/usr/local/bin/core",
            "./proj/BUILD/dst/usr/local/bin/core",
            "./proj/core/core",
        ]

        self.CONTROLLER_PATH = None

        for path in self.PATHS:
            if path and os.path.exists(path):
                self.CONTROLLER_PATH = path
                break
            
        if self.CONTROLLER_PATH is None:
            self.fail("Could not find core: ".format(self.PATHS))

        self.controller = ControllerInstance(self.CONTROLLER_PATH, imu="-",
                                             POST=True)

        self.assertTrue(self.controller.start())

    def tearDown(self):
        self.controller.shutdown()
