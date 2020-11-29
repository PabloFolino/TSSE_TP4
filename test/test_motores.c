// Listado de pruebas:
// --> Test de la función auxiliar pwmWrite.
// --> Test de la función auxiliar gpioWrite.
// --> Test de la función inicialización.
// --> Test de velocidad positiva.
// --> Test de velocidad negativa.
// --> Test de zona muerta.
// --> Cambio de límite superior de velocidad.


#include "unity.h"

#include "motores.h"

#include "mock_sapi_pwm.h"
#include "mock_sapi_gpio.h"

//======================= Constantes locales =====================================
#define GPIO_OUTPUT 1
#define VELOCIDAD_CERO      0
#define VELOCIDAD_POSITIVA  200
#define VELOCIDAD_NEGATIVA  -200
#define VELOCIDAD_BAJA_POSI 50      // Si la velocidad es menor a +/-120
#define VELOCIDAD_BAJA_NEGA -50     // el motor se para, ya que no tiene 
                                    // fuerza para mover el robot.
#define NEW_VELOCIDAD_UP    195     // Si la velocidad es mayor que la máxima
                                    // se satura al limite superior. 

//======================== Variables locales ======================================
uint16_t ledsVirtuales;
gpioMap_t pinesVirtuales[]={0,0,M1_ENA1,M1_ENA2,M2_ENA1,0,M2_ENA2};
pwmMap_t pwmVirtual[]={MOT_DER,MOT_IZQ};
char mensaje[32];


typedef struct{
    pwmMap_t motor;
    gpioMap_t enable1;
    gpioMap_t enable2;
} ejemplo_t;

ejemplo_t EJEMPLOS[]={ 
    {.motor=MOT_DER,.enable1=M1_ENA1,.enable2=M1_ENA2},
    {.motor=MOT_IZQ,.enable1=M2_ENA1,.enable2=M2_ENA2}
    };


#define CANTIDAD_EJEMPLOS sizeof(EJEMPLOS)/sizeof(ejemplo_t)



//======================== Funciones Internas ======================================
bool_t aux_pwmWrite( pwmMap_t pwmNumber, uint8_t percent ){
    switch (pwmNumber) {
        case MOT_DER:
            pwmVirtual[MOT_DER]=percent;
            break;
        case MOT_IZQ:
            pwmVirtual[MOT_IZQ]=percent;
            break; 
        default:
            return false;
    }       
    return true;
}
bool_t aux_gpioWrite( gpioMap_t pin, bool_t value ){
    switch (pin) {
        case M1_ENA1:
            pinesVirtuales[M1_ENA1]=value;
            break;
        case M1_ENA2:
            pinesVirtuales[M1_ENA2]=value;
            break;
        case M2_ENA1:
            pinesVirtuales[M2_ENA1]=value;
            break;
        case M2_ENA2:
            pinesVirtuales[M2_ENA2]=value;
            break;
        default:
            return false;
    }
    return true;
};


// ======================= Gestión de Errores =====================================
bool error_informado=false;

void RegistrarError(void) {
    error_informado=true;
}

//========================== Funciones del software de testeo =====================
void setUp(void) {                      // Se llama cada vez que comienza una prueba
     error_informado=false;
    // Leds_Create(&ledsVirtuales, RegistrarError);
    pwmWrite_fake.custom_fake=aux_pwmWrite;
    gpioWrite_fake.custom_fake=aux_gpioWrite;
    motor_init();                         // Condiciones iniciales de cada  Test
}

void tearDown(void) {                   // Se llama cada vez que termina una prueba

}

//======================================== Testeos ==================================

// Test de la función auxiliar pwmWrite
void test_pwmWrite(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];
        sprintf(mensaje,"Ejemplo %d", i);
        
        pwmVirtual[ejemplo->motor]=VELOCIDAD_POSITIVA;

        TEST_ASSERT_TRUE_MESSAGE(pwmWrite( ejemplo->motor ,VELOCIDAD_CERO ),mensaje);
        TEST_ASSERT_EQUAL_HEX_MESSAGE(VELOCIDAD_CERO,pwmVirtual[ejemplo->motor],mensaje);
    }

}


// Test de la función auxiliar gpioWrite
void test_gpioWrite(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];
        sprintf(mensaje,"Ejemplo %d", i);
        
        pinesVirtuales[ejemplo->enable2]=false;

        TEST_ASSERT_TRUE_MESSAGE(gpioWrite( ejemplo->enable2,1 ),mensaje);
        TEST_ASSERT_EQUAL_MESSAGE(true,pinesVirtuales[ejemplo->enable2],mensaje);
    }
}

// Test de la función inicialización
void test_initInicializacion(void){
    //TEST_ASSERT_TRUE(motor_init());
    TEST_ASSERT_TRUE_MESSAGE(motor_init(), "Init Motores anda");
}

// Test de velocidad positiva
void test_velocPositiva(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];
        sprintf(mensaje,"Ejemplo %d", i);

        TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_POSITIVA, ejemplo->motor), mensaje);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(VELOCIDAD_POSITIVA,pwmVirtual[ejemplo->motor],mensaje);
        TEST_ASSERT_TRUE_MESSAGE(pinesVirtuales[ejemplo->enable1],mensaje);
        TEST_ASSERT_FALSE_MESSAGE(pinesVirtuales[ejemplo->enable2],mensaje);
    }
}

// Test de velocidad negativa.
void test_velocNegativa(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];    
        sprintf(mensaje,"Ejemplo %d", i);

        TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_NEGATIVA, ejemplo->motor), mensaje);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(VELOCIDAD_POSITIVA,pwmVirtual[ejemplo->motor],mensaje);
        TEST_ASSERT_FALSE_MESSAGE(pinesVirtuales[ejemplo->enable1],mensaje);
        TEST_ASSERT_TRUE_MESSAGE(pinesVirtuales[ejemplo->enable2],mensaje);
    }    
}

// Test de zona muerta + .
void test_velocBandaPositiva(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];    
        sprintf(mensaje,"Ejemplo %d", i);

        TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_BAJA_POSI, ejemplo->motor), mensaje);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(VELOCIDAD_CERO,pwmVirtual[ejemplo->motor],mensaje);
    }
}

// Test de zona muerta - .
void test_velocBandaNegativa(void){
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];     
        sprintf(mensaje,"Ejemplo %d", i);

        TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_BAJA_NEGA, ejemplo->motor), mensaje);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(VELOCIDAD_CERO,pwmVirtual[ejemplo->motor], mensaje);
    }
}


// Cambio de límite superior de velocidad. Puebo si satura.
void test_velocUpNew(void){
    motorSpeedMax(NEW_VELOCIDAD_UP);
    for(int i=0;i<CANTIDAD_EJEMPLOS;i++){
        ejemplo_t * ejemplo=&EJEMPLOS[i];  
        sprintf(mensaje,"Ejemplo %d", i);

        motorSpeed( VELOCIDAD_NEGATIVA, ejemplo->motor);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(NEW_VELOCIDAD_UP,pwmVirtual[ejemplo->motor],mensaje);
    }
}