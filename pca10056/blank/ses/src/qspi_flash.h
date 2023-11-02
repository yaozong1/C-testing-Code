#ifndef _UART_C_H_
#define _UART_C_H_


extern bool  qspi_test(void);
extern void  read_REMS(uint8_t*, uint8_t*);
extern bool  qspi_read_id(uint8_t *manufacturer_id_readback);


#endif