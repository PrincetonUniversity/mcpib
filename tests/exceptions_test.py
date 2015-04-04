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

class ExceptionsTestCase(unittest.TestCase):


    def testModule(self):
        """Test that custom exceptions are defined within the mcpib module.
        """
        self.assertTrue(issubclass(mcpib.WrapperError, Exception))
        self.assertTrue(issubclass(mcpib.UnknownCppException, mcpib.WrapperError))
        self.assertTrue(issubclass(mcpib.SignatureError, mcpib.WrapperError))
        self.assertTrue(issubclass(mcpib.SignatureError, TypeError))
        self.assertTrue(issubclass(mcpib.FromPythonError, mcpib.SignatureError))
        self.assertTrue(issubclass(mcpib.AmbiguousOverloadError, mcpib.SignatureError))

if __name__ == "__main__":
    unittest.main()
