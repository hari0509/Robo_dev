#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef size_t size;

typedef enum {
    E_OK = 0,
    E_NOT_OK
} Std_ReturnType;

#endif /* STD_TYPES_H */