#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef size_t size;
typedef uint8_t boolean;

#define TRUE 1u
#define FALSE 0u

/* AUTOSAR standard on/off and logic level symbols */
#define STD_ON   1u
#define STD_OFF  0u
#define STD_HIGH 1u
#define STD_LOW  0u

typedef enum {
    E_OK = 0,
    E_NOT_OK
} Std_ReturnType;

/* AUTOSAR module version information (Std_VersionInfoType) */
typedef struct {
    uint16 vendorID;
    uint16 moduleID;
    uint8  sw_major_version;
    uint8  sw_minor_version;
    uint8  sw_patch_version;
} Std_VersionInfoType;

#endif /* STD_TYPES_H */