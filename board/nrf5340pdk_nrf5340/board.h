#ifndef _BOARD_H_
#define _BOARD_H_

#define LED_GREEN 30 /* P0.30 */
#define LED_BLUE  31 /* P0.31 */
#define LED_RED   29 /* P0.29 */
#define LED1      28 /* P0.28 */

#define LED_BLINK_NET (LED_RED)

#if defined(NRF5340_XXAA_APPLICATION)
#define LED_BLINK (LED1)
#elif defined(NRF5340_XXAA_NETWORK)
#define LED_BLINK (LED_BLINK_NET)
#endif

#endif
