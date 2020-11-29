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
#define CANTIDAD_MOTORES sizeof(pwmVirtual)/sizeof(pwmMap_t)


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
    for(int i=0;i<CANTIDAD_MOTORES;i++){
        pwmVirtual[i]=VELOCIDAD_POSITIVA;

        TEST_ASSERT_TRUE(pwmWrite( MOT_IZQ,0 ));
        TEST_ASSERT_EQUAL_HEX(0,pwmVirtual[MOT_IZQ]);
    }

}


// Test de la función auxiliar gpioWrite
void test_gpioWrite(void){
    pinesVirtuales[M2_ENA2]=false;
    
    TEST_ASSERT_TRUE(gpioWrite( M2_ENA2,1 ));
    TEST_ASSERT_EQUAL(true,pinesVirtuales[M2_ENA2]);
}

// Test de la función inicialización
void test_initInicializacion(void){
    //TEST_ASSERT_TRUE(motor_init());
    TEST_ASSERT_TRUE_MESSAGE(motor_init(), "Init Motores anda");
}

// Test de velocidad positiva
void test_velocPositiva(void){
    TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_POSITIVA, MOT_DER), "Init Motores anda");
    TEST_ASSERT_EQUAL_HEX8(VELOCIDAD_POSITIVA,pwmVirtual[MOT_DER]);
    TEST_ASSERT_TRUE(pinesVirtuales[M1_ENA1]);
    TEST_ASSERT_FALSE(pinesVirtuales[M1_ENA2]);
}

// Test de velocidad negativa.
void test_velocNegativa(void){
    TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_NEGATIVA, MOT_DER), "Init Motores anda");
    TEST_ASSERT_EQUAL_HEX8(VELOCIDAD_POSITIVA,pwmVirtual[MOT_DER]);
    TEST_ASSERT_FALSE(pinesVirtuales[M1_ENA1]);
    TEST_ASSERT_TRUE(pinesVirtuales[M1_ENA2]);
}

// Test de zona muerta + .
void test_velocBandaPositiva(void){
    TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_BAJA_POSI, MOT_DER), "Init Motores anda");
    TEST_ASSERT_EQUAL_HEX8(VELOCIDAD_CERO,pwmVirtual[MOT_DER]);
}

// Test de zona muerta - .
void test_velocBandaNegativa(void){
    TEST_ASSERT_TRUE_MESSAGE(motorSpeed( VELOCIDAD_BAJA_NEGA, MOT_DER), "Init Motores anda");
    TEST_ASSERT_EQUAL_HEX8(VELOCIDAD_CERO,pwmVirtual[MOT_DER]);
}


// Cambio de límite superior de velocidad. Puebo si satura.
void test_velocUpNew(void){
    motorSpeedMax(NEW_VELOCIDAD_UP);
    motorSpeed( VELOCIDAD_NEGATIVA, MOT_DER);
    TEST_ASSERT_EQUAL_HEX8(NEW_VELOCIDAD_UP,pwmVirtual[MOT_DER]);
}


/*
//Se puede apagar todos los leds
void test_LedsOffAfterCreate(void){
    uint16_t ledsVirtuales=0xFFFF;
    Leds_Create(&ledsVirtuales, RegistrarError);
    TEST_ASSERT_EQUAL_HEX16(0,ledsVirtuales);
}

//Se puede prender un LED individual
void test_prender_ledindividual(void) {
    Leds_On(1);
    TEST_ASSERT_EQUAL_HEX16(1,ledsVirtuales);
}

// Se puede apagar un led individual
void test_apagar_ledindividual(void) {
    Leds_On(1);
    Leds_Off(1);
    TEST_ASSERT_EQUAL_HEX16(0,ledsVirtuales);
}

// Se puede prender y apagar múltiplas LED's
void test_prender_apagar_multiples(void) {
    Leds_On(3);
    Leds_On(4);
    Leds_Off(3);
    TEST_ASSERT_EQUAL_HEX16(1 << 3,ledsVirtuales);
}

// Revisar parámetros fuera de los límites
void test_prender_led_invalido(void) {
    Leds_On(17);
    TEST_ASSERT_TRUE(error_informado);
}

void test_prender_led_invalido1(void) {
    Leds_On(0);
    TEST_ASSERT_TRUE(error_informado);
}

// Se consulta el estado de un led
void test_consultar_led_ON(void) {
    Leds_On(16);
    TEST_ASSERT_TRUE(Leds_Test(16));
}

void test_consultar_led_OFF(void) {
    TEST_ASSERT_FALSE(Leds_Test(16));
}
*/