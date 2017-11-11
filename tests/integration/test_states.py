import os
import time
import unittest

from controller_tests import ControllerTests

class TestStateMachine(ControllerTests):

    def setUp(self):
        super(TestStateMachine, self).setUp()

    def tearDown(self):
        super(TestStateMachine, self).tearDown()

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
