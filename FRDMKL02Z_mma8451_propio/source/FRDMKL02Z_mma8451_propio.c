/*! @file : main.c
 * @author  Javier Casallas
 * @version 1.0.0
 * @date    27/01/2021
 * @brief   Driver para
 * @details
 *
*/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL02Z4.h"
#include "fsl_debug_console.h"


#include "sdk_hal_gpio.h"
#include "sdk_hal_uart0.h"
#include "sdk_hal_i2c0.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MMA851_I2C_DEVICE_ADDRESS	0x1D

#define MMA8451_WHO_AM_I_MEMORY_ADDRESS		0x0D

#define MMA8451_XMSB_MEMORY_ADDRESS		0x01

#define MMA8451_XLSB_MEMORY_ADDRESS		0x02

#define MMA8451_YMSB_MEMORY_ADDRESS		0x03

#define MMA8451_YLSB_MEMORY_ADDRESS		0x04

#define MMA8451_ZMSB_MEMORY_ADDRESS		0x05

#define MMA8451_ZLSB_MEMORY_ADDRESS		0x06

/*******************************************************************************
 * Private Prototypes
 ******************************************************************************/


/*******************************************************************************
 * External vars
 ******************************************************************************/


/*******************************************************************************
 * Local vars
 ******************************************************************************/


/*******************************************************************************
 * Private Source Code
 ******************************************************************************/


/*******************************************************************************
 * Public Source Code
 ******************************************************************************/
int main(void) {
	status_t status;
	uint8_t nuevo_byte_uart;
	uint8_t	nuevo_dato_i2c;
	uint8_t	nuevo_dato_i2c_1;
	uint8_t	nuevo_dato_i2c_2;
	uint16_t coordenada_x;
	uint16_t coordenada_y;
	uint16_t coordenada_z;


  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    (void)uart0Inicializar(115200);	//115200bps
    (void)i2c0MasterInit(100000);	//100kbps

    PRINTF("Usar teclado para controlar LEDs\r\n");
    PRINTF("r-R led ROJO\r\n");
    PRINTF("v-V led VERDE\r\n");
    PRINTF("a-A led AZUL\r\n");
    PRINTF("x-X led AZUL\r\n");
    PRINTF("y-Y led AZUL\r\n");
    PRINTF("z-Z led AZUL\r\n");
    PRINTF("M buscar acelerometro\r\n");

    i2c0MasterWriteByte(0x01, 0x2A);

    while(1) {
    	if(uart0CuantosDatosHayEnBuffer()>0){
    		status=uart0LeerByteDesdeBuffer(&nuevo_byte_uart);
    		if(status==kStatus_Success){
    			printf("dato:%c\r\n",nuevo_byte_uart);
    			switch (nuevo_byte_uart) {
				case 'a':
				case 'A':
					gpioPutToggle(KPTB10);
					break;

				case 'v':
					gpioPutHigh(KPTB7);
					break;
				case 'V':
					gpioPutLow(KPTB7);
					break;

				case 'r':
					gpioPutValue(KPTB6,1);
					break;
				case 'R':
					gpioPutValue(KPTB6,0);
					break;

				case 'M':
					i2c0MasterReadByte(&nuevo_dato_i2c, MMA851_I2C_DEVICE_ADDRESS, MMA8451_WHO_AM_I_MEMORY_ADDRESS);

					if(nuevo_dato_i2c==0x1A)
						printf("MMA8451 encontrado!!\r\n");
					else
						printf("MMA8451 error\r\n");

					break;

				case 'X':
				case 'x':
					i2c0MasterReadByte(&nuevo_dato_i2c_1, MMA851_I2C_DEVICE_ADDRESS, MMA8451_XMSB_MEMORY_ADDRESS);
					i2c0MasterReadByte(&nuevo_dato_i2c_2, MMA851_I2C_DEVICE_ADDRESS, MMA8451_XLSB_MEMORY_ADDRESS);

					coordenada_x = nuevo_dato_i2c_1;

					coordenada_x <<= 6;

					coordenada_x = coordenada_x | nuevo_dato_i2c_2;

					printf("MMA8451 coordenada X: %d\r\n", coordenada_x);

					gpioPutValue(KPTB6,0);
					gpioPutValue(KPTB7,1);
					gpioPutValue(KPTB10,1);


					break;
				case 'Y':
				case 'y':
					i2c0MasterReadByte(&nuevo_dato_i2c_1, MMA851_I2C_DEVICE_ADDRESS, MMA8451_YMSB_MEMORY_ADDRESS);
					i2c0MasterReadByte(&nuevo_dato_i2c_2, MMA851_I2C_DEVICE_ADDRESS, MMA8451_YLSB_MEMORY_ADDRESS);

					coordenada_y = nuevo_dato_i2c_1;

					coordenada_y <<= 6;

					coordenada_y = coordenada_y | nuevo_dato_i2c_2;

					printf("MMA8451 coordenada Y: %d\r\n", coordenada_y);

					gpioPutValue(KPTB6,1);
					gpioPutValue(KPTB7,0);
					gpioPutValue(KPTB10,1);

					break;

				case 'Z':
				case 'z':
					i2c0MasterReadByte(&nuevo_dato_i2c_1, MMA851_I2C_DEVICE_ADDRESS, MMA8451_ZMSB_MEMORY_ADDRESS);
					i2c0MasterReadByte(&nuevo_dato_i2c_2, MMA851_I2C_DEVICE_ADDRESS, MMA8451_ZLSB_MEMORY_ADDRESS);

					coordenada_z = nuevo_dato_i2c_1;

					coordenada_z <<= 6;

					coordenada_z = coordenada_z | nuevo_dato_i2c_2;

					printf("MMA8451 coordenada Z: %d\r\n", coordenada_z);

					gpioPutValue(KPTB6,1);
					gpioPutValue(KPTB7,1);
					gpioPutValue(KPTB10,0);

					break;
    			}

    		}else{
    			printf("error\r\n");
    		}
    	}
    }

    return 0 ;
}
