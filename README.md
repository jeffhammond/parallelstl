# Parallel STL 
[![Stable release](https://img.shields.io/badge/version-20171127-green.svg)](https://github.com/intel/parallelstl/releases/tag/20171127)
[![Apache License Version 2.0](https://img.shields.io/badge/license-Apache_2.0-green.svg)](LICENSE)

Parallel STL is an implementation of the C++ standard library algorithms with support for execution policies, 
as specified in the working draft N4659 for the next version of the C++ standard, commonly called C++17. 
The implementation also supports the unsequenced execution policy specified in the ISO* C++ working group paper P0076R3.

Parallel STL offers a portable implementation of threaded and vectorized execution of standard C++ algorithms, optimized and validated for Intel(R) 64 processors.
For sequential execution, it relies on an available implementation of the C++ standard library.

## Prerequisites
To use Parallel STL, you must have the following software installed:
* C++ compiler with:
  * Support for C++11
  * Support for OpenMP* 4.0 constructs

## Release Information
Here are the latest [Changes](CHANGES) and [Release Notes](doc/Release_Notes.txt) (contains system requirements and known issues).

## License
Parallel STL is licensed under [Apache License Version 2.0](LICENSE).

## Documentation
[Getting Started](https://software.intel.com/en-us/get-started-with-pstl) with Parallel STL.

## Support
Please report issues and suggestions via
[GitHub issues](https://github.com/jeffhammond/parallelstl/issues)

------------------------------------------------------------------------
Intel and the Intel logo are trademarks of Intel Corporation or its subsidiaries in the U.S. and/or other countries.

\* Other names and brands may be claimed as the property of others.
