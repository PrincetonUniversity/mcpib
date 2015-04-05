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

import builtin_numeric_mod as mod

class BuiltinNumericTestCase(unittest.TestCase):

    def checkInteger(self, func, minimum, maximum, name):
        self.assertIsNone(func(int(5)))
        self.assertIsNone(func(True))
        self.assertIsNone(func(long(minimum)))
        self.assertIsNone(func(long(maximum)))
        self.assertRaises(OverflowError, func, maximum + 1)
        self.assertRaises(OverflowError, func, minimum - 1)
        self.assertRaises(TypeError, func, "5")

    def testIntegers(self):
        if mod.char_is_signed:
            self.checkInteger(mod.accept_char, -2**(mod.bits_char - 1), 2**(mod.bits_char - 1) - 1, "char")
        else:
            self.checkInteger(mod.accept_char, 0, 2**bits + 1, "char")
        self.checkInteger(mod.accept_signed_char, -2**(mod.bits_char - 1), 2**(mod.bits_char - 1) - 1,
                          name="signed char")
        self.checkInteger(mod.accept_unsigned_char, 0, 2**mod.bits_char - 1,
                          name="unsigned char")
        for t in ("int", "short", "long", "long_long"):
            bits = getattr(mod, "bits_%s" % t)
            self.checkInteger(getattr(mod, "accept_%s" % t), -2**(bits - 1), 2**(bits - 1) - 1, t)
            self.checkInteger(getattr(mod, "accept_unsigned_%s" % t), 0, 2**bits - 1, "unsigned %s" % t)

    def checkFloat(self, func, name):
        self.assertIsNone(func(2.3))
        self.assertIsNone(func(1.0))
        self.assertIsNone(func(True))
        self.assertRaises(TypeError, func, "5")

    def testFloats(self):
        for t in ("float", "double", "long_double"):
            self.checkFloat(getattr(mod, "accept_%s" % t), t)


if __name__ == "__main__":
    unittest.main()
