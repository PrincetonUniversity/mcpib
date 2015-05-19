# mcpib
The Modern C++/Python Interface Builder

## Overview
MCPIB will be a toolkit for providing Python access to C++ code, with capabilities roughly comparable to Boost.Python and Swig.  It is currently in a a very early stage of development, with very few of its planned features implemented.  Eventually, it will consist of two main components:
 - The low-level component is a C++ wrapper-generator library, much like Boost.Python.  Unlike Boost.Python, MCPIB will be developed in C++14 with no support for older compilers and no dependency on other Boost libraries (though some optional features may require Boost).  This approach allows us to have a vastly simpler codebase and a faster development process, in keeping with an overall theme of making the code more transparent and extensible (even at a low level), rather than syntactically sugary.  MCPIB will also have better support for function overloading and constness, and will aim to provide more complete built-in support for the STL and NumPy.
 - The upper level will be a code generator tool that parses the C++ interface using libclang and emits C++ wrapper code according to a definition written in Python: the user will write the Python interface for C++ classes directly in Python, using decorators to indicate where inject wrapped C++ code alongside pure-Python extensions.  These definitions will be imported in a special context and inspected to generate the C++ wrapper code.  Once the wrappers are compiled, the same interface definition module can be imported as a regular Python module, which will import the binary module and delegate to it as specified.  The code generator driver system will naturally be extensible in Python, allowing users to choose between carefully defining every member of each wrapped class or generating wrappers programmatically for multiple classes and methods from the C++ interface.  And because code comments from the C++ interface will be available to the Python interface definition system, excerpts from those comments can be used to build docstrings for the Python interface.

## Architecture
 - There are no custom psuedo-languages (such as Cython's "almost-Python" or Swig's "almost-C-preprocessor"); all extensions and customizations are written in real C++ or real Python.
 - We're using a real C++ front-end parser to read C++ interfaces.
 - The type system is implemented in C++ using RTTI.  This allows extensions to the wrappers to be implemented in C++ with full access to the type conversion system and the Python C API.  More importantly, this means groups of interdependent modules can be generated entirely separately, preventing dependencies from increasing the size or compile times of dependent modules.
 - The code generation layer is as thin as possible: most of the logic is implemented in either the Python definition library or the C++ wrapper library, minimizing the size of the only layer that cannot be debugged using normal Python or C++ debugging techniques.

## Status
The core of the type system in the C++ library has been implemented, as well as fairly complete support for wrapping free functions (with support for overloading that already exceeds Boost.Python's capabilities).  However, type converters must still be written manually, because there's no support for wrapping custom classes yet.

At present, MCPIB isn't useful for someone who just wants to get their code wrapped, and while we'd love to have more developers, anyone who is interested should contact us (via the issue page) before contributing anything, to ensure it's consistent with our development plans.
