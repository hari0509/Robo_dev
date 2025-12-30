/**
 * ============================================================================
 * FILE: ecum.c
 * ============================================================================
 * DESCRIPTION:
 *   ECU Manager (EcuM) component. Responsible for initializing and managing
 *   all platform software components (CDD servo, ASW blink, etc.).
 *
 * AUTHOR: Hari
 * ============================================================================
 */

/* --- INCLUDES --- */
/* Forward declarations/external symbols from components */
extern void cdd_servo_init(void);
extern void asw_blink_init(void);

/* --- MACROS --- */
/* None */

/* --- CONSTANTS --- */
/* None */

/* --- VARIABLES --- */
/* None */

/* --- PRIVATE CODE --- */
/* None */

/* --- PUBLIC CODE --- */
/**
 * EcuM_Init()
 * Initialize all ECU software components (CDD, ASW, etc.).
 * Called once during system startup before scheduler begins.
 */
void EcuM_Init(void)
{
    cdd_servo_init();
    asw_blink_init();
}

/* --- REVISION HISTORY --- */
/* v1.0 - Initial commit - Hari */