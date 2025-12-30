/**
 * ============================================================================
 * FILE: ecum.h
 * ============================================================================
 * DESCRIPTION:
 *   ECU Manager (EcuM) header. Public interface for ECU initialization.
 *
 * AUTHOR: Hari
 * ============================================================================
 */

#ifndef ECUM_H
#define ECUM_H

/* --- INCLUDES --- */
/* None */

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- PUBLIC FUNCTION DECLARATIONS --- */
/**
 * EcuM_Init()
 * Initialize all ECU software components (CDD, ASW, etc.).
 * Called once during system startup before scheduler begins.
 */
void EcuM_Init(void);

#endif /* ECUM_H */

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */
