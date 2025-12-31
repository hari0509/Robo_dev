# 📚 Child CMake Configuration Guide

## 📦 Creating a Library Target

```cmake
add_library(cdd_servo STATIC src/cdd_servo.c)
```

| Property | Value |
|----------|-------|
| 🎯 **Target name** | `cdd_servo` |
| 📋 **Type** | STATIC library (`.a`) |
| 📄 **Source file** | `src/cdd_servo.c` |

---

## 🔍 Adding Include Directories

```cmake
target_include_directories(cdd_servo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

### ✨ What it does
Adds an include path for the compiler:
```bash
-I apps/cdd_servo/src
```

### 📌 Benefits
- ✅ Allows clean imports in source files:
  ```c
  #include "cdd_servo.h"
  ```

---

## 🔗 Linking Libraries

```cmake
target_link_libraries(cdd_servo PUBLIC pico_stdlib)
```

### 🎓 Purpose
- **Exposes** runtime dependencies
- 📌 `cdd_servo` needs Pico SDK headers
- 🔀 Anyone using `cdd_servo` **also indirectly** depends on Pico SDK
