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
import builtin_strings_mod as mod

class BuiltinStringsTestCase(unittest.TestCase):

    def testString(self):
        """Test that to-Python and from-Python converters for std::string work as expected."""
        self.assertEqual(mod.passthru_string("foo"), "foo")
        self.assertRaises(mcpib.FromPythonError, mod.passthru_string, 5)
        self.assertRaises(mcpib.FromPythonError, mod.passthru_string, ["bar"])

    def testCString(self):
        """Test that to-Python and from-Python converters for char const * work as expected."""
        self.assertEqual(mod.passthru_cstring("foo"), "foo")
        self.assertRaises(mcpib.FromPythonError, mod.passthru_cstring, 5)
        self.assertRaises(mcpib.FromPythonError, mod.passthru_cstring, ["bar"])

    def testCharArgs(self):
        """Test that c-string converters are not used for char values, references, or non-const pointers."""
        self.assertRaises(mcpib.FromPythonError, mod.accept_char, "foo")
        self.assertRaises(mcpib.FromPythonError, mod.accept_char_const, "foo")
        self.assertRaises(mcpib.FromPythonError, mod.accept_char_ptr, "foo")
        self.assertRaises(mcpib.FromPythonError, mod.accept_char_ref, "foo")
        self.assertRaises(mcpib.FromPythonError, mod.accept_char_const_ref, "foo")
        self.assertRaises(mcpib.ToPythonError, mod.return_char)
        self.assertRaises(mcpib.ToPythonError, mod.return_char_const)
        self.assertRaises(mcpib.ToPythonError, mod.return_char_ptr)
        self.assertRaises(mcpib.ToPythonError, mod.return_char_ref)
        self.assertRaises(mcpib.ToPythonError, mod.return_char_const_ref)

if __name__ == "__main__":
    unittest.main()
