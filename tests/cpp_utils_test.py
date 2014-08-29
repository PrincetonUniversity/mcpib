#
# Copyright (c) 2014, Jim Bosch
# All rights reserved.
#
# mcpib is distributed under a simple BSD-like license;
# see the LICENSE file that should be present in the root
# of the source distribution.
#

import unittest

import cpp_utils_mod

class CppUtilsTestCase(unittest.TestCase):

    def testPyPtr(self):
        self.assertTrue(cpp_utils_mod.testPyPtr())

if __name__ == "__main__":
    unittest.main()
