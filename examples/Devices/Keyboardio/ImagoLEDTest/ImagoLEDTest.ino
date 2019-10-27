#include<avr/pgmspace.h>
#include <Arduino.h>

extern "C" {
#include "kaleidoscope/hardware/keyboardio/twi.h"
}

#define ELEMENTS(arr)  (sizeof(arr) / sizeof((arr)[0]))


#define ADDR_IS31 0x60
#define R 0x25
#define G 0xC0
#define B 0xFF
uint8_t i,j;



static constexpr uint8_t brightness_steps = 64;
const PROGMEM byte PWM_Gamma_Brightness[64]=
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0B,0x0D,0x0F,0x11,0x13,0x16,
    0x1A,0x1C,0x1D,0x1F,0x22,0x25,0x28,0x2E,
    0x34,0x38,0x3C,0x40,0x44,0x48,0x4B,0x4F,
    0x55,0x5A,0x5F,0x64,0x69,0x6D,0x72,0x77,
    0x7D,0x80,0x88,0x8D,0x94,0x9A,0xA0,0xA7,
    0xAC,0xB0,0xB9,0xBF,0xC6,0xCB,0xCF,0xD6,
    0xE1,0xE9,0xED,0xF1,0xF6,0xFA,0xFE,0xFF
};

void setup() {
    digitalWrite(MOSI, HIGH);
    digitalWrite(SS, HIGH);
    uint8_t twi_uninitialized = 1;
    if (twi_uninitialized--) {
        twi_init();
    }
    TWBR=10;

    //TWBR=2;
    Init_3741(R, G, B);
}

void IS31FL3741_Test_mode() {
    Init_3741(0xff, 0xff, 0xff);
    IS31FL3741_Test_mode1();

}
void loop() {
    IS31FL3741_Test_mode();//breath mode
    delay(5);
}

void TWI_Send(uint8_t addr,uint8_t Reg_Add,uint8_t Reg_Dat) {
    uint8_t data[] = {Reg_Add, Reg_Dat };
    uint8_t result = twi_writeTo(addr/2, data, ELEMENTS(data), 1, 0);

}


static constexpr uint8_t REGISTER_SET_PAGE = 0xFD;
static constexpr uint8_t REGISTER_WRITE_ENABLE = 0xFE;

static constexpr uint8_t WRITE_ENABLE_ONCE = 0b11000101;
void cmd_3741_unlock() {
    TWI_Send(ADDR_IS31,REGISTER_WRITE_ENABLE, WRITE_ENABLE_ONCE);//unlock
}

void cmd_3741_write_page(uint8_t page) {
    // Registers automatically get locked at startup and after a given write
    // It'd be nice to disable that.
    cmd_3741_unlock();
    TWI_Send(ADDR_IS31,REGISTER_SET_PAGE,page);
}



void init_pwm_data() {
    set_all_pwm_to(0);
}


void Init_3741(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata) {
    cmd_3741_write_page(2);

    //uint8_t data[] = {Reg_Add, Reg_Dat };
   // uint8_t result = twi_writeTo(addr/2, data, ELEMENTS(data), 1, 0);


    uint8_t data[181] = {};
    data[0]=0;
    for(i=1; i<0xB4; i+=3) {
	data[i] = Bdata;
	data[i+1] =Gdata;
	data[i+2] = Rdata;
    }
     twi_writeTo(ADDR_IS31/2, data, 0xB5, 1, 0);


    // For space efficiency, we reuse the LED sending buffer
    // The twi library should never send more than the number of elements
    // we say to send it.
    // The page 2 version has 180 elements. The page 3 version has only 171.

    cmd_3741_write_page(3);
    for(i=1; i<0xAB; i+=3) {
	data[i] = Bdata;
	data[i+1] = Gdata;
	data[i+2] = Rdata;
    }

     twi_writeTo(ADDR_IS31/2, data, 0xAC, 1, 0);



    init_pwm_data();

    cmd_3741_write_page(4);
    TWI_Send(ADDR_IS31,0x01,0xFF);//global current
    TWI_Send(ADDR_IS31,0x00,0x01);//normal operation
}


void set_all_pwm_to(uint8_t step) {
    cmd_3741_write_page(0);

    uint8_t data[0xB5] = {};
    data[0]=0;
    // PWM Register 0 is 0x00 to 0xB3
    for(i=1; i<=0xB4; i++) {
	data[i]=step;

    }
     twi_writeTo(ADDR_IS31/2, data, 0xB5, 1, 0);
    cmd_3741_write_page(1);
    // PWM Register 1 is 0x00 to 0xAA
    for(i=1; i<=0xAB; i++) {
	data[i]=step;

    }
     twi_writeTo(ADDR_IS31/2, data, 0xAC, 1, 0);

}

void IS31FL3741_Test_mode1() {

    cmd_3741_write_page(0); // "write first frame"
    for (j=0; j<brightness_steps; j++) { //all LED ramping up
        set_all_pwm_to(gamma_correct(j));
    }
        delay(1000);//10ms
    for (j=brightness_steps-1; j>0; j--) { //all LED ramping up
        set_all_pwm_to(gamma_correct(j));
    }
        delay(1000);//10ms

}

uint8_t gamma_correct(uint8_t i) {
    return pgm_read_byte_near(&PWM_Gamma_Brightness[i]);
}
