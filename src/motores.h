// =============================================================================
// * Author: Pablo Daniel Folino  <pfolino@gmail.com>
// * Date: 2020/08/17
// * Archivo: motores.h
// * Version: 1
// *===========================================================================

/*=====[Inclusions of public function dependencies]==========================*/
//#include "sapi.h"
//#include "sapi_peripheral_map.h"
#include "sapi_gpio.h"
#include "sapi_pwm.h"


#include <stdint.h>
#include  <stdbool.h>
#define bool_t bool

#define ON	1
#define OFF	0


/*=====[Definition macros of public constants]===============================*/
#define MAX_VELOCIDAD 255
#define MIN_VELOCIDAD 120

/* Se designan los pines del sentido de giro
* IMPORTANTE: nunca deben estar los dos pines en nivel alto
*/
#define M1_ENA1 T_COL0				//Motor derecho
#define M1_ENA2	T_FIL2
#define M2_ENA1	T_FIL3				//Motor izquierdo
#define M2_ENA2	T_COL1

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef enum {
	Motor_OFF,
	Motor_ON
} enableDisable_t;

typedef enum {
	MOT_DER,
	MOT_IZQ
} numeroMotor_t;

typedef enum {
	MOT_ADELANTE=0,
	MOT_ATRAS=1,
} spinMotor_t;

typedef struct {
	numeroMotor_t mot1_nombre;
	int16_t mot1_veloc;
	enableDisable_t mot1_ena;
	spinMotor_t mot1_spin;
	numeroMotor_t mot2_nombre;
	int16_t mot2_veloc;
	enableDisable_t mot2_ena;
	spinMotor_t mot2_spin;
} estadoMotores_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

// Funcion que inicializa la tarea
bool_t motor_init( void );

// Funcion que se ejecuta periodicamente
bool_t motorSpeed(int16_t velocidad, bool_t numeroMotor);
void motorEnable(enableDisable_t enableDisable, numeroMotor_t numeroMotor);
void motorSpeedMax(uint8_t valor);
void motorSpeedMin(uint8_t valor);


/*=====[Prototypes (declarations) of public interrupt functions]=============*/







