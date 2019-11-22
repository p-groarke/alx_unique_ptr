# alx_unique_ptr
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
