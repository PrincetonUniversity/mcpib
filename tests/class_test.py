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
import class_mod as mod

class ClassTestCase(unittest.TestCase):

    def assertIsSubclass(self, a, b):
        self.assertTrue(issubclass(a, b), msg="%s is not a subclass of %s" % (a, b))

    def assertNotIsSubclass(self, a, b):
        self.assertFalse(issubclass(a, b), msg="%s is a subclass of %s" % (a, b))

    def testClassRelationships(self):
        """Test for the correct class and metaclass relationships for a wrapped type."""
        self.assertIsInstance(mcpib.Type, type)
        self.assertIsSubclass(mcpib.Type, type)
        self.assertIsInstance(mcpib.Object, mcpib.Type)
        self.assertIsInstance(mcpib.Object, type)
        self.assertIsInstance(mod.A, mcpib.Type)
        self.assertIsInstance(mod.A, type)
        self.assertNotIsSubclass(mod.A, type)
        self.assertNotIsSubclass(mod.A, mcpib.Type)
        self.assertIsInstance(mod.a1, mod.A)
        self.assertIsInstance(mod.a1, mcpib.Object)

    def testFromPython(self):
        """Test from-Python converteres for wrapped classes."""
        self.assertEqual(mod.accept_val(a=mod.a1), "A")
        self.assertEqual(mod.accept_ref(a=mod.a1), "A")
        self.assertEqual(mod.accept_cref(a=mod.a1), "A")
        self.assertEqual(mod.accept_ptr(a=mod.a1), "A")
        self.assertEqual(mod.accept_cptr(a=mod.a1), "A")

if __name__ == "__main__":
    unittest.main()
