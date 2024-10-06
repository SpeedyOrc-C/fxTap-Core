# fxTap Core

The core of rhythm game **fxTap**, 
written in pure C without any user interfaces.

## Use the library

Add this to compile the library:

```cmake
add_subdirectory(fxTap-Core/lib)
```

Add this to `#include` headers from fxTap Core:

```cmake
target_include_directories(Your-Project PRIVATE fxTap-Core/lib/include)
```

So if you want to include [this file](./lib/include/fxTap/beatmap.h)
`./lib/include/fxTap/beatmap.h`, write this in your project:

```c
#include <fxTap/beatmap.h>
```

It's also very likely that this library will be used on an embedded system,
not PC, since there are already tons of rhythm games on PC.
So you need to choose your platform by defining **one** of the following macros:

* `FXTAP_CORE_ON_GINT` for gint kernel on CASIO calculators.
* `FXTAP_CORE_ON_ARDUINO` for Arduino boards.
* `FXTAP_CORE_ON_MODERN_OS` for Windows, macOS and Linux.

And then define this macro in your `CMakeLists.txt` such as:

```cmake
target_compile_definitions(fxTap-Core PRIVATE FXTAP_CORE_ON_GINT)
```

Finally, link the library to your project:

```cmake
target_link_libraries(Your-Project fxTap-Core)
```

## Build the library

Make sure you have CMake installed.

Run [make-lib.sh](./make-lib.sh),
and the binary will be in [build-lib](./build-lib).

If you're using CMake, check [Test's Configuration](./test/CMakeLists.txt)
for how to add this library to your own project.

## Run the tests

Run [make-test.sh](./make-test.sh),
and run the [executable](./build-test/fxTap-Core-Test).

Please note that the test undoubtedly runs with `FXTAP_CORE_ON_MODERN_OS` macro.
