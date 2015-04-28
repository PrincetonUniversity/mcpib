#
# Copyright (c) 2014-2015, Jim Bosch
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
import overloads_mod as mod

class OverloadsTestCase(unittest.TestCase):

    def testPositionalResolution(self):
        """Test that we can resolve simple overloads involving a single argument."""
        self.assertEqual(mod.f0(2), "int")
        self.assertEqual(mod.f0(2.0), "double")

    def testKeywordResolution(self):
        """Test that keyword argument names trump types in resolution."""
        self.assertEqual(mod.f0(i=2), "int")
        self.assertEqual(mod.f0(d=2.0), "double")
        self.assertEqual(mod.f0(d=3), "double")

    def testFailures(self):
        """Test various forms of invalid signatures."""
        self.assertRaises(mcpib.SignatureError, mod.f0, "2")
        self.assertRaises(mcpib.SignatureError, mod.f0, i=3.0)
        self.assertRaises(mcpib.SignatureError, mod.f0, s="2")
        self.assertRaises(mcpib.SignatureError, mod.f0, 2, 4.0)
        self.assertRaises(mcpib.SignatureError, mod.f0, 2, d=4.0)
        self.assertRaises(mcpib.SignatureError, mod.f0)
        self.assertRaises(mcpib.SignatureError, mod.f0, i=3.0, d=4.0)

    def testAmbiguous(self):
        """Test that we can identify ambiguous overloads."""
        self.assertRaises(mcpib.AmbiguousOverloadError, mod.f1, 1, 2)

if __name__ == "__main__":
    unittest.main()
