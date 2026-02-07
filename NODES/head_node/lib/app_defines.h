/* app_defines.h for SB_Node_dev app */

#define SB_DATPORT 		PORTD
#define SB_DAT_ISR_VEC	PORTD_PORT_vect
#define SB_PORT 		PORTE
#define SB_CLK 			PIN0_bm
#define SB_ACT 			PIN1_bm

#define SERIAL_BAUDRATE 57600
#define TX_PIN PIN0_bm
#define RX_PIN PIN1_bm
#define MSG_BUF_LEN 16

// The following define to which DAT pins SB modules are connected
#define MOD_SERVO PIN1_bm
