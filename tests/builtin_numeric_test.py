#
# Copyright (c) 2014, Jim Bosch
# All rights reserved.
#
# mcpib is distributed under a simple BSD-like license;
# see the LICENSE file that should be present in the root
# of the source distribution.
#

import unittest
import os
import sys

buildPythonPath = os.path.join(os.path.split(__file__)[0], "..", "python")
if os.path.exists(buildPythonPath): sys.path.insert(0, buildPythonPath)

import builtin_numeric_mod

class BuiltinNumericTestCase(unittest.TestCase):

    def testSingleArgument(self):
        self.assertIsNone(builtin_numeric_mod.accept_int_return_1(5))
        self.assertIsNone(builtin_numeric_mod.accept_int_return_1(long(5)))
        self.assertIsNone(builtin_numeric_mod.accept_int_return_1(True))

if __name__ == "__main__":
    unittest.main()
