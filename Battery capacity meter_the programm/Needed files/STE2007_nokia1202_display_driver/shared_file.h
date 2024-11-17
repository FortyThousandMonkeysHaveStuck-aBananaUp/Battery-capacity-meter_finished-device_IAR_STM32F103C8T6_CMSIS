//struct different_varaibles varaibles; //definition this struct in the main.c file!!!!

#define THIS_PROGRAM_FOR_STM32
//#define THIS_PROGRAM_FOR_PIC
//#include "./nokia_1202_ste2007_display_fonts.h"

#ifdef THIS_PROGRAM_FOR_STM32

    #define STM32F103xB
    #include "stm32f1xx.h"

    /* For Nokia display*/
    //PB3 for LED
    //PB4 for SCK   //ch1
    //PB5 for MOSI  //ch3
    //PB6 for CS    //ch5
    //PB7 for RESET //ch7

    #define LED_up GPIOB->BSRR|=GPIO_BSRR_BS3;
    #define LED_down GPIOB->BSRR|=GPIO_BSRR_BR3;

    #define SCK_up GPIOB->BSRR|=GPIO_BSRR_BS4;
    #define SCK_down GPIOB->BSRR|=GPIO_BSRR_BR4;

    #define MOSI_up GPIOB->BSRR|=GPIO_BSRR_BS5;
    #define MOSI_down GPIOB->BSRR|=GPIO_BSRR_BR5;

    #define CS_up GPIOB->BSRR|=GPIO_BSRR_BS6;
    #define CS_down GPIOB->BSRR|=GPIO_BSRR_BR6;

    #define RESET_up GPIOB->BSRR|=GPIO_BSRR_BS7;
    #define RESET_down GPIOB->BSRR|=GPIO_BSRR_BR7;
    /* For Nokia display*/

#endif

#ifdef THIS_PROGRAM_FOR_PIC
    //something
#endif

typedef enum{
    display_initialization,
    clear_all,
    permanent_words,
    voltage_displaying,
    seconds,
    current,
    capacity_word,
    initial_voltage,
    final_voltage
}command_word;

struct different_varaibles{
float voltage;
unsigned int voltage_to_calculate_the_current;
unsigned int capacity;
unsigned int seconds_counter;
};

void transmit_command(char command);
void transmit_data(char data);
void user_delay(unsigned long delay);
void ste2007_display_driver(command_word word, struct different_varaibles * varaibles);
void display_symbol(char symbol);
void page_and_column_address_set_AND_display_symbols_array(char page, char column, char array[]);

