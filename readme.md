# alx_unique_ptr
[![Build Status](https://travis-ci.org/p-groarke/alx_unique_ptr.svg?branch=master)](https://travis-ci.org/p-groarke/alx_unique_ptr)
[![Build status](https://ci.appveyor.com/api/projects/status/994h0yod285e5nfl/branch/master?svg=true)](https://ci.appveyor.com/project/p-groarke/alx-unique-ptr/branch/master)

A unique_ptr implementation with weak_ptr capabilities.

## Examples

```c++

```

## Build
`alx_unique_ptr` is a single header with no dependencies other than the stl.

The unit tests depend on gtest and bench_util, which will be automatically installed when running cmake setup. Use `-DBUILD_TESTING=Off` to disable unit tests.

**Install recent conan, cmake and compiler.**

### Windows
On VS2017 and earlier, add `-A x64` to your cmake setup command.

```
mkdir build && cd build
cmake .. && cmake --build . --config debug
bin\alx_unique_ptr_tests.exe

// Optionally
cmake --build . --target install
```

### Unixes
```
mkdir build && cd build
cmake .. -DBUILD_TESTING=On && cmake --build . --config debug
bin\alx_unique_ptr_tests.exe

// Optionally
cmake --build . --target install
```
