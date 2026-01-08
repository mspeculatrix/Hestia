// app_defines.h

#define DAT_PORT PORTD
#define DAT_ISR_VECTOR PORTD_PORT_vect
#define SB_PORT PORTE
#define SB_CLK PIN0_bm
#define SB_ACT PIN1_bm

#define SERIAL_BAUDRATE 19200
#define TX_PIN PIN0_bm
#define RX_PIN PIN1_bm
#define MSG_BUF_LEN 16

// The following define to which pins modules are connected
#define MOD_SRO4 0
