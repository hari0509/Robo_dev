# 📚 Child CMake Configuration Guide

# 📦 Step 1: Creating a Library Target
This command tells CMake: 
 1. Compile `cdd_servo.c` into `cdd_servo.o`
 2. Use the `'ar'` tool to pack `cdd_servo.o` into `libcdd_servo.a`

 Command to show the lib `.a` bundle. 
 ```cmake 
 ar -t libcdd_servo.a
 ```
```
cdd_servo.c.o
stdlib.c.o
gpio.c.o
platform.c.o
common.c.o
panic.c.o
claim.c.o
....
```
```cmake
add_library(cdd_servo STATIC src/cdd_servo.c)
```

| Property | Value |
|----------|-------|
| 🎯 **Target name** | `cdd_servo` |
| 📋 **Type** | STATIC library (`.a`) |
| 📄 **Source file** | `src/cdd_servo.c` |

---
Static `(.a)` - Compile-time linking

Shared `(.so)` - Runtime (at startup)

---

# 🔍 Step 2. Adding Include Directories

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

# 🔗 Step 3: Linking Another Dependant Libraries

```cmake
target_link_libraries(cdd_servo PUBLIC pico_stdlib)
```

### 🎓 Purpose
- **Exposes** runtime dependencies
- 📌 `cdd_servo` needs Pico SDK headers
- 🔀 Anyone using `cdd_servo` **also indirectly** depends on Pico SDK
---

When you write:
`target_link_libraries(cdd_servo PUBLIC pico_stdlib)`

You are creating a **Dependency Chain**.

---

### 1. The Three Musketeers: PUBLIC, PRIVATE, and INTERFACE

The keyword you choose determines how the "Main App" sees the Pico SDK.

### 🔴 PRIVATE
* **Meaning:** "I use the Pico SDK, but you don't need to know about it."
* **Result:** `cdd_servo` compiles fine, but the **Main App** will crash or throw errors because it won't be able to find the Pico headers or functions through your library.

### 🟡 INTERFACE
* **Meaning:** "I don't use the Pico SDK, but anyone using me will need it."
* **Result:** Rare for this specific case. Usually used for header-only libraries.

### 🟢 PUBLIC (What you used)
* **Meaning:** "I need the Pico SDK, and anyone who uses me **also** needs it."
* **Result:** This is the correct choice for most hardware drivers. It ensures that the **Include Paths** and **Linker Flags** for the Pico SDK "bubble up" to your main project automatically.

---

### ✅ Summary for your .md Guide

| Keyword | Does `cdd_servo` get it? | Does the `Main App` get it? |
|:--- |:---:|:---:|
| **PRIVATE** | ✅ Yes | ❌ No |
| **PUBLIC** | ✅ Yes | ✅ Yes |
| **INTERFACE**| ❌ No | ✅ Yes |

---