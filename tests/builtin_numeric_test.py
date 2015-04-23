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

    def checkModifiedTypes(self, name, value):
        """Test pointer and references to numeric types.

        Const pointers and references should both be converted from Python to C++, while
        non-const pointers and references should not (because those imply that the function
        may modify them, which we can't support in Python).

        No pointers or references should be converted from C++ to Python, since we don't know
        how the user would want us to manage their lifetimes.
        """
        self.assertRaises(mcpib.FromPythonError, getattr(mod, "accept_%s_ref" % name), value)
        self.assertEqual(getattr(mod, "accept_%s_cref" % name)(value), None)
        self.assertRaises(mcpib.FromPythonError, getattr(mod, "accept_%s_ptr" % name), value)
        self.assertEqual(getattr(mod, "accept_%s_cptr" % name)(value), None)
        self.assertRaises(mcpib.ToPythonError, getattr(mod, "return_%s_ref" % name))
        self.assertRaises(mcpib.ToPythonError, getattr(mod, "return_%s_cref" % name))
        self.assertRaises(mcpib.ToPythonError, getattr(mod, "return_%s_ptr" % name))
        self.assertRaises(mcpib.ToPythonError, getattr(mod, "return_%s_cptr" % name))

    def checkInteger(self, minimum, maximum, name):
        func = getattr(mod, "passthru_%s" % name)
        self.assertEqual(func(int(5)), 5)
        self.assertEqual(func(True), True)
        self.assertEqual(func(long(minimum)), long(minimum))
        self.assertEqual(func(long(maximum)), long(maximum))
        self.assertRaises(OverflowError, func, maximum + 1)
        self.assertRaises(OverflowError, func, minimum - 1)
        self.assertRaises(mcpib.FromPythonError, func, "5")
        self.checkModifiedTypes(name, 4)

    def testIntegers(self):
        self.checkInteger(-2**(mod.bits_signed_char - 1), 2**(mod.bits_signed_char - 1) - 1,
                           name="signed_char")
        self.assertEqual(type(mod.passthru_signed_char(5)), int)
        self.checkInteger(0, 2**mod.bits_unsigned_char - 1, name="unsigned_char")
        self.assertEqual(type(mod.passthru_unsigned_char(5)), int)
        for t in ("int", "short", "long", "long_long"):
            bits = getattr(mod, "bits_%s" % t)
            self.checkInteger(-2**(bits - 1), 2**(bits - 1) - 1, t)
            self.checkInteger(0, 2**bits - 1, "unsigned_%s" % t)
            passthru_signed = getattr(mod, "passthru_%s" % t)
            passthru_unsigned = getattr(mod, "passthru_unsigned_%s" % t)
            if bits < mod.bits_long:
                self.assertEqual(type(passthru_signed(5)), int)
                self.assertEqual(type(passthru_unsigned(5)), int)
            elif bits > mod.bits_long:
                self.assertEqual(type(passthru_signed(5)), long)
                self.assertEqual(type(passthru_unsigned(5)), long)
            else:
                self.assertEqual(type(passthru_signed(5)), int)
                self.assertEqual(type(passthru_unsigned(5)), long)

    def checkFloat(self, name):
        func = getattr(mod, "passthru_%s" % name)
        self.assertEqual(func(2.5), 2.5)
        self.assertEqual(func(1.0), 1.0)
        self.assertEqual(func(True), 1.0)
        self.assertRaises(mcpib.FromPythonError, func, "5")
        self.checkModifiedTypes(name, 1.5)

    def testFloats(self):
        for name in ("float", "double", "long_double"):
            self.checkFloat(name)


if __name__ == "__main__":
    unittest.main()
