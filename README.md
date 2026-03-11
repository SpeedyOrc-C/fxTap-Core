# fxTap Core

The core of rhythm game **fxTap** written in C without any user interfaces.

## Usage

Add this to compile the library:

```cmake
add_subdirectory(fxTap-Core)
```

Add this to `#include` headers from fxTap Core:

```cmake
target_include_directories(Your-Project PRIVATE fxTap-Core/include)
```

So if you want to include [this file](include/fxTap/beatmap.h)
`include/fxTap/beatmap.h`, write this in your project:

```c
#include <fxTap/beatmap.h>
```

It's also very likely that this library will be used on an embedded system,
not PC, since there are already tons of rhythm games on PC.
So you need to choose what features you platform supports by defining these macros:

* `FXTAP_CORE_USE_CASIOWIN` for CASIO G and GII models' old file system.
* `FXTAP_CORE_HAS_DIRENT` if your platform supports walking the file system.

And then define this macro in your `CMakeLists.txt` such as:

```cmake
target_compile_definitions(fxTap-Core PRIVATE FXTAP_CORE_USE_CASIOWIN)
```

Finally, link the library to your project:

```cmake
target_link_libraries(Your-Project fxTap-Core)
```
