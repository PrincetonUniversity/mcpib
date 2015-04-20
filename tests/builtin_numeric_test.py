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

import mcpib
import builtin_numeric_mod as mod

class BuiltinNumericTestCase(unittest.TestCase):

    def checkInteger(self, func, minimum, maximum, name):
        self.assertEqual(func(int(5)), 5)
        self.assertEqual(func(True), True)
        self.assertEqual(func(long(minimum)), long(minimum))
        self.assertEqual(func(long(maximum)), long(maximum))
        self.assertRaises(OverflowError, func, maximum + 1)
        self.assertRaises(OverflowError, func, minimum - 1)
        self.assertRaises(mcpib.FromPythonError, func, "5")

    def testIntegers(self):
        self.checkInteger(mod.passthru_signed_char, -2**(mod.bits_signed_char - 1),
                          2**(mod.bits_signed_char - 1) - 1,
                          name="signed char")
        self.assertEqual(type(mod.passthru_signed_char(5)), int)
        self.checkInteger(mod.passthru_unsigned_char, 0, 2**mod.bits_unsigned_char - 1,
                          name="unsigned char")
        self.assertEqual(type(mod.passthru_unsigned_char(5)), int)
        for t in ("int", "short", "long", "long_long"):
            bits = getattr(mod, "bits_%s" % t)
            passthru_signed = getattr(mod, "passthru_%s" % t)
            passthru_unsigned = getattr(mod, "passthru_unsigned_%s" % t)
            self.checkInteger(passthru_signed, -2**(bits - 1), 2**(bits - 1) - 1, t)
            self.checkInteger(passthru_unsigned, 0, 2**bits - 1, "unsigned %s" % t)
            if bits < mod.bits_long:
                self.assertEqual(type(passthru_signed(5)), int)
                self.assertEqual(type(passthru_unsigned(5)), int)
            elif bits > mod.bits_long:
                self.assertEqual(type(passthru_signed(5)), long)
                self.assertEqual(type(passthru_unsigned(5)), long)
            else:
                self.assertEqual(type(passthru_signed(5)), int)
                self.assertEqual(type(passthru_unsigned(5)), long)

    def checkFloat(self, func, name):
        self.assertEqual(func(2.5), 2.5)
        self.assertEqual(func(1.0), 1.0)
        self.assertEqual(func(True), 1.0)
        self.assertRaises(mcpib.FromPythonError, func, "5")

    def testFloats(self):
        for t in ("float", "double", "long_double"):
            self.checkFloat(getattr(mod, "passthru_%s" % t), t)


if __name__ == "__main__":
    unittest.main()
