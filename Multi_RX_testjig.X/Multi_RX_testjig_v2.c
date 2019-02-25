
// <editor-fold defaultstate="collapsed" desc="File Header">
//Project : Multi_RX testjig

//Device : 16F1527

//Date : 1/6/2015

//Author : John Frost

//Description :

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Config and Includes">
#include <xc.h>
#include <stdlib.h>
#include <pic.h>
//#asm
//set mcu freq for __delay_() function
#define _XTAL_FREQ  16000000
#ifndef __delay_us
#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/8000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/8000.0)))
#endif
//#endasm

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT disabled)
#ifndef __DEBUG
#pragma config PWRTE = ON    // Power-up Timer Enable->PWRT enabled
#pragma config MCLRE = ON    // MCLR Pin Function Select->MCLR/VPP pin function is digital input
#pragma config CP = OFF          // Flash Program Memory Code Protection (Program memory code protection is enabled)
#endif
#ifdef __DEBUG
#pragma config PWRTE = OFF   // Power-up Timer Enable->PWRT enabled
#pragma config MCLRE = ON    // MCLR Pin Function Select->MCLR/VPP pin function is digital input
#pragma config CP = OFF          // Flash Program Memory Code Protection (Program memory code protection is enabled)
#endif
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Hardware definitions">
#define lcd_reset       RA5
#define lcd_backlight   RA4
#define i2c_lcd_address      0x7C
#define i2c_pot_address         0x5C //[tpl0401a]
#define trigger         LATD2
#define buzzer          LATC5
#define button          RE1

#define pickit_rel_1        LATC2
#define pickit_rel_2        LATC7
#define pickit_button_relay LATC6
#define pickit_LED_blue     RE4
#define pickit_LED_red      RE6
#define pickit_LED_green    RE5

//#define motor_direction_1 RA0
//#define motor_direction_2 RA1

#define CH2_high_current_K13_P10 LATB5
#define CH3_high_current_K6_P13 LATA6

#define power_supply_k7 LATD4
#define power_supply_k8 LATD5
#define power_supply_k9 LATD6

#define board_detect RF3

#define comms_MCLR_input RG1
#define comms_TX_in RG0
#define comms_TX LATG0
#define comms_TX_TRIS TRISG0
#define comms_RX RG3

#define CH2_P10_in RF1
#define CH3_P13_in RF0


#define mclr_reset_P10 LATD7
#define mclr_reset_P10_TRIS TRISD7

#define power_5v        LATA7


#define BM_COMM_input_P23   RA3
#define skip_programming        RA2

#define relay_NC_P4 RF5
#define relay_NO_P8 RF2

#define Bus_input RC1
#define Bus_fet LATC0

#define RF_button   RG0
#define RF_LED      RG3
#define RF_out     RE0

#define debug_1 LATF4
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="function declarations">

void i2c_Write(unsigned char data);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_Write_string(const unsigned char *data);
void i2c_write_char(unsigned char char_data);
void init_i2c_lcd_5v(unsigned char contrast, unsigned char single1_double2);
void clear_lcd(void);
void i2c_first_line(void);
void i2c_second_line(void);
void lcd_createChar(unsigned char location, unsigned char charmap[]);
void init_i2c(void);
void init_current_sink(void);
unsigned char get_motor_direction(void);
void power_supply_set(unsigned char supply);
unsigned int get_30v_current(void);
void Vout_set(int Volts);
void digitalpot(unsigned char value);
void debug_fast_tx(unsigned int data);
void TX_one(unsigned char data);
unsigned char RX_one(void);
unsigned char RX_one_timeout(void);
void init_ADC(void);
unsigned int ADC_get_val(unsigned char channel);
void print_error(const unsigned char *top, const unsigned char *bottom);
void print_screen(const unsigned char *top, const unsigned char *bottom);
void lcd_print_int(unsigned int number, unsigned int pos, unsigned char decimal, unsigned char ten_thou_digit);
/**
 This function initializes the USART2 module to the following parameters: \n
 TXEN = 1, transmitter enabled \n
 BRGH = 1, High baud rate \n
 SPEN = 1, serial port enable \n
 CREN = 1, Receiver enabled \n
 BRG16 = 1, 16 bit baud rate generator\n
 Baud Rate = 117647 baud.
 @param None
 @return None
 @Calls
 */
void init_uart2(void);
/**
 This function disables the USART2 module to allow the pins to be used as RF inputs. \n
  TXEN = 0, transmitter disabled \n
  CREN = 0, Receiver disabled \n
  SPEN = 0, serial port disable
 @param None
 @return None
 @Calls
 */
void disable_uart(void);
/**
 This function enables the USART2 for for RF module communications. \n
  TXEN = 1, transmitter enabled \n
  SPEN = 1, serial port enable \n
 then calls uart_reset();
 @param None
 @return None
 @Calls uart_reset();
 */
void enable_uart(void);
/**
 This function waits for the USART2 tx module to be adle and then begins transmision of 1 byte over uart.
 @param uart1_data - data to be transmitted.
 @return None
 @Calls
 */
void send_uart2(unsigned char uart1_data);
/**
 This function resets the USART2 and clears the buffer
 @param None
 @return None
 @Calls
 */
void uart_reset(void);
/**
 This function recieves 1 packet (4 bytes) over USART2.
 @param uart1_timer - the timeout for the receiver, decremented once per TMR0 interrupt.
 @return UART1_RESET - if a comms error occured and the uart was reset. \n
 UART1_BUFFER_FULL - if 4 bytes were received. \n
 UART1_TIMEOUT - if a time-out occured
 @Calls
 */
unsigned char uart_receive_four(unsigned int uart1_timer);
/**
 This function sends one packet (4 bytes, or 1 if opcode == 0x89) over uart and then receives 1 packet.
 @param opcode - the first byte to be sent\n
 byte_1, byte_2, byte3 - the second, third and fourth bytes to be transmitted respectively. \n
 uart2_receive_timer - the timeout passed to uart_receive_four();
 @return returns the state returned by uart_receive_four().
 @Calls
 */
unsigned char uart_send_receive_packet(unsigned char opcode, unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned int uart2_receive_timer);

void buzz_one(unsigned int time);
void testjig_done(unsigned char state, unsigned char device_type);
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="User definitions">

enum
{
    current_sink_off, current_sink_on, sampling_ADC, updating_PWM
} current_sink_state;

enum
{
    pickit_finish, pickit_start, pickit_push_button, pickit_busy, pickit_error, pickit_idle
} pickit_state;


#define ADC_current_sink    0b01001111//channel 19 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
//#define ADC_5v_test         0b01101011//channel 26 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
#define ADC_sink_voltage    0b01110111//channel 29 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
#define ADC_5v_test         0b01011111//channel 23 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
#define ADC_12v_test          0b01100011//channel 24 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
#define ADC_Vtest3          0b01100111//channel 25 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)
#define ADC_30v_cur_sense   0b01010011//channel 20 (channels are bit shifted up by two bits and padded with 11 to set Go_nDone and ADC_on)

#define UART1_RESET        0
#define UART1_TIMEOUT      1
#define UART1_BUFFER_FULL  2

#define NO_motor_current 0xFF
#define Direction_1 0x01
#define Direction_2 0x02

#define AC1     0
#define AC2     1
#define BAT     2
#define NONE    3

#define Ctest_off       0
#define Ctest_battery   1
#define Ctest_12v       2
#define Ctest_24v       3

#define one_second_timer_value 1000

#define Relay 0
#define Diodes 1

#define YES                     1
#define NO                      0


#define ADC_reference_voltage 4.096
#define ADC_resolution 1024

#define Vtest_P5_CH23_div_ratio 11.0
#define Vtest_P5_CH23_5v (((5/Vtest_P5_CH23_div_ratio)*ADC_resolution)/ADC_reference_voltage)
#define P5_5v_test_min (unsigned int)(Vtest_P5_CH23_5v*0.92)
#define P5_5v_test_max (unsigned int)(Vtest_P5_CH23_5v*1.08)
#define P5_5v_test_1v (unsigned int)(Vtest_P5_CH23_5v*0.2)

#define Vtest_P2_CH24_div_ratio 11.0
#define Vtest_P2_CH24_12v (((12/Vtest_P2_CH24_div_ratio)*ADC_resolution)/ADC_reference_voltage)
#define P2_12v_test_min (unsigned int)(Vtest_P2_CH24_12v*0.9)
#define P2_12v_test_max (unsigned int)(Vtest_P2_CH24_12v*1.2)



#define T0_1s_float 7518.8

#define T0_100ms    (unsigned int)(T0_1s_float/10)
#define T0_250ms    (unsigned int)(T0_1s_float/4)
#define T0_400ms    (unsigned int)(T0_1s_float/2.5)
#define T0_500ms    (unsigned int)(T0_1s_float/2)
#define T0_750ms    (unsigned int)(3*(T0_1s_float/4))
#define T0_1s       (unsigned int)(T0_1s_float)
#define T0_2s       (unsigned int)((2*T0_1s_float))
#define T0_3s       (unsigned int)((3*T0_1s_float))
#define T0_4s       (unsigned int)((4*T0_1s_float))
#define T0_5s       (unsigned int)((5*T0_1s_float))



#define Device_Condo3       0xAA
#define Device_RX3          0xA1
#define Device_Mem_Copier   0xA2


// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Global variables">
const unsigned int popup_delay = 15; //delay a popup is shown for, in 100ms
unsigned char button_beep_delay = 2;
unsigned char ten_thou, thou, hund, ten, unit; //0-256 GLOBALS!!!!
unsigned char uart_buff_work_recieve[5];
volatile int uart_receive_timer, motor_direction_timer;
volatile unsigned int pickit_timer;
volatile unsigned int current_sink_timer, testjig_timer;
unsigned char return_data;
volatile unsigned int one_second_timer = one_second_timer_value;
volatile unsigned int buzz_timer;
volatile unsigned int ADC_cur_sink_val;
volatile unsigned int curent_sink_val, current_sink_stable;
unsigned char RelayVsDiodes = Relay;
unsigned char ADC_val = 0;
unsigned char pickit_red_counter = 0;
unsigned int trip_current;
unsigned int tj_timer = 0;
// </editor-fold>

//////////////////////////////////////...Code Starts Here...///////////////////////////////////////

// <editor-fold defaultstate="collapsed" desc="Interrupt routines">

void interrupt isr(void)
{
    if(INTCONbits.TMR0IF == 1) //timer 0 interrupt, 133us
    {

        if(testjig_timer)
            testjig_timer--;
        else
            RF_out = 0;//it was flipping code sometimes, hopefully this will fix it.
        if(uart_receive_timer)
            uart_receive_timer--;
        if(motor_direction_timer)
            motor_direction_timer--;

        INTCONbits.TMR0IF = 0;

    }
    if(PIR1bits.TMR2IF == 1)//timer 2 interrupt, 1ms
    {//used to control the current sink

        if(pickit_timer)
            pickit_timer--;
        else
        {
            switch(pickit_state)
            {
                case pickit_start:
                    pickit_rel_1 = 1;
                    pickit_rel_2 = 1;
                    pickit_timer = 10; //allow 10ms for relays to switch
                    pickit_state = pickit_push_button;
                    break;
                case pickit_push_button:
                    pickit_timer = 1000; //allow 300ms for relays to push button
                    pickit_button_relay = 1;
                    pickit_state = pickit_busy;
                    break;
                case pickit_busy:
                    pickit_button_relay = 0;
                    if(pickit_LED_red == 0)
                    {
                        pickit_red_counter++;
                        if(pickit_red_counter > 100)
                            pickit_state = pickit_finish;
                    }
                    else
                        pickit_red_counter = 0;
                    if(pickit_LED_green == 0)//an error occurred
                    {
                        pickit_state = pickit_error;
                        pickit_timer = 1000;
                    }
                    else if(pickit_LED_blue == 0)//back in standby
                    {
                        pickit_state = pickit_finish;
                    }
                    pickit_timer = 1;
                    break;
                case pickit_error:
                    pickit_rel_1 = 0;
                    pickit_rel_2 = 0;
                    break;
                case pickit_finish:
                    pickit_rel_1 = 0;
                    pickit_rel_2 = 0;
                    pickit_state = pickit_idle;
                    break;
                case pickit_idle:
                    break;
            }
        }

        //        switch(current_sink_state)
        //        {
        //                //          case sampling_ADC:
        //                //            break;
        //                //          case updating_PWM:
        //                //            break;
        //            case current_sink_on:
        //                ADCON0 = ADC_current_sink; //set ADC channel
        //                while(ADCON0bits.GO_nDONE == 1); //wait for conversion to complete
        //                ADC_cur_sink_val = ADRESH << 8 | ADRESL;
        //                if(ADC_cur_sink_val > curent_sink_val + 50)//self protection, if current is way too high then switch off and start again
        //                {
        //                    CCPR9L = 0;
        //                    current_sink_stable = 0;
        //                }
        //                else
        //                {
        //                    if(current_sink_timer)
        //                        current_sink_timer--;
        //                    else
        //                    {
        //                        current_sink_timer = 2;
        //                        if(ADC_cur_sink_val > curent_sink_val)
        //                        {
        //                            if((ADC_cur_sink_val - 30) > curent_sink_val)
        //                            {
        //                                if(CCPR9L >= 10)
        //                                    CCPR9L -= 2; //((ADC_cur_sink_val - curent_sink_val)>>3);
        //                            }
        //                            else
        //                            {
        //                                if(CCPR9L != 0)
        //                                    CCPR9L--;
        //                            }
        //                        }
        //                        else if(ADC_cur_sink_val < curent_sink_val)
        //                        {
        //                            if((ADC_cur_sink_val + 30) < curent_sink_val)
        //                            {
        //                                if(CCPR9L <= 250)
        //                                    CCPR9L += 2; //((curent_sink_val - ADC_cur_sink_val)>>3);
        //                            }
        //                            else
        //                            {
        //                                if(CCPR9L != 255)
        //                                    CCPR9L++;
        //                            }
        //                        }
        //                        if(ADC_cur_sink_val > curent_sink_val - 4 && ADC_cur_sink_val < curent_sink_val + 4)
        //                            current_sink_stable++;
        //                        else
        //                            current_sink_stable = 0;
        //                    }
        //                }
        //                break;
        //            case current_sink_off:
        //                CCPR9L = 0;
        //                break;
        //        }

        if(buzz_timer)
            buzz_timer--;
        else
            buzzer = 0;
        if(one_second_timer)
            one_second_timer--;
        else
        {
            one_second_timer = one_second_timer_value;
        }
        if(tj_timer)
            tj_timer--;
        PIR1bits.TMR2IF = 0;
    }
}
// </editor-fold>

void main(void)
{
    // <editor-fold defaultstate="collapsed" desc="Oscilator and port setup">
    OSCCON = 0x7A;
    //    OPTION_REG = 0b01000001; //PULLUPS ENABLE/INT INSTR CLOCK;/PRE= divide by 4
    OPTION_REG = 0b11010000;
    TMR0 = 0x00;
    TMR0IF = 0;
    TMR0IE = 1;

    //--SETUP PORTS
    TRISA = 0b00001111; //
    TRISB = 0b11001111; //
    TRISC = 0b00011010; //
    TRISD = 0b10001011; //
    TRISE = 0b11110110; //
    TRISF = 0b00101111; //
    TRISG = 0b11101111; //
    //
    PORTA = 0b00000000;
    PORTB = 0b00000000;
    PORTC = 0b00000000; //
    PORTD = 0b01000000; //
    PORTE = 0b00000000; //
    PORTF = 0b00000000; //
    PORTG = 0b00000000; //
    //
    WPUB = 0b00000000; //
    WPUD = 0b00000000; //
    WPUE = 0b00000000; //
    //
    ANSELA = 0b00000000; //
    ANSELB = 0b00001100; //
    ANSELD = 0b00001111; //
    ANSELE = 0b00000100; //
    ANSELF = 0b00000000; //
    ANSELG = 0b00000000; //
    buzzer = 1;
    // </editor-fold>

    init_ADC();
    init_current_sink();
    pickit_state = pickit_idle;
    //    current_sink_state = current_sink_off;
    lcd_reset = 0;
    buzz_one(200);
    lcd_reset = 1; // disable reset on LCD
    init_i2c(); //init I2C module on MCU
    //    init_uart2();
    //    disable_uart();
    //    uart_reset();
    //    enable_uart();
    //    uart_reset();
    //        init_i2c_lcd(60, 1);
    init_i2c_lcd_5v(8, 0x38);
    print_screen("Multi_RX", "Test-Jig|Startup");
    lcd_backlight = 1;
    GIE = 1;
    pickit_rel_2 = 1;
    __delay_ms(100);
    debug_fast_tx(0xAAAA);
    __delay_us(100);
    debug_fast_tx(0x5555);
    __delay_us(100);
    
    RF_out = 0;

    if(PCONbits.nRMCLR == 1)//if a MCLR did not occur (power up or fault)
        testjig_done(2, 0);


    if(board_detect == 1)//check that a board is inserted
        print_error("Multi_RX", "-> Insert PCB");

    // <editor-fold defaultstate="collapsed" desc="Power up">
    print_screen("Powering board", "");

    //power 12v (P2), measure 5v, current
    Vout_set(12);
    power_supply_set(AC1);
    __delay_ms(50);
    ADC_val = ADC_get_val(ADC_5v_test); //dummy read required
    testjig_timer = T0_500ms;
    while((ADC_get_val(ADC_5v_test) < P5_5v_test_min) && (ADC_get_val(ADC_5v_test) > P5_5v_test_max) && (get_30v_current() < 30) && testjig_timer);
   /* if((ADC_get_val(ADC_5v_test) < P5_5v_test_min))
        print_error("12v-5v supply", "Volt Error L");
    else if((ADC_get_val(ADC_5v_test) > P5_5v_test_max))
        print_error("12v-5v supply", "Volt Error H");
    else if((get_30v_current() >= 60))
        print_error("12v supply", "Curr Error");
    else if(testjig_timer == 0)
        print_error("12v-5v supply", "T-out Error");
    */



    //power 16v, measure 12v, 5v, current
    power_supply_set(AC2);
    Vout_set(16);
    __delay_ms(150); //tested, works down to 75ms
    testjig_timer = T0_500ms;
    while((ADC_get_val(ADC_5v_test) < P5_5v_test_min) && (ADC_get_val(ADC_5v_test) > P5_5v_test_max)&&(ADC_get_val(ADC_12v_test) < P2_12v_test_min) && (ADC_get_val(ADC_12v_test) > P2_12v_test_max) && (get_30v_current() < 60) && testjig_timer);
    unsigned int temp;
    temp = get_30v_current();
   
    lcd_print_int(ADC_get_val(ADC_5v_test), 5, 0, 1);
    __delay_ms(1000);
    
    /*if((ADC_get_val(ADC_5v_test) < P5_5v_test_min))
        print_error("16v-5v supply", "Volt Error L");
    else if((ADC_get_val(ADC_5v_test) > P5_5v_test_max))
        print_error("16v-5v supply", "Volt Error H");
    else*/ if((ADC_get_val(ADC_12v_test) < P2_12v_test_min))
        print_error("16v-12v supply", "Volt Error L");
    else if((ADC_get_val(ADC_12v_test) > P2_12v_test_max))
        print_error("16v-12v supply", "Volt Error H");
    else if((temp >= 60))
    {
        lcd_print_int(temp, 5, 0, 1);
        __delay_ms(1000);
        print_error("16v supply", "Curr Error");
    }
    else if(testjig_timer == 0)
        print_error("16v-5v supply", "T-out Error");


    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Programming">
    if(skip_programming == 1)
    {
        power_supply_set(AC1);

        print_screen("Pickit Programming", "");
        power_5v = 1; //force the 5v rail to be powered during programming.
        __delay_ms(100); //allow relay to switch
        print_screen("Waiting for", " Programming");


        pickit_rel_1 = 1;
        pickit_rel_2 = 1;
        __delay_ms(100);//relay switch delay
        pickit_button_relay = 1;
        __delay_ms(1000);//button press delay
        pickit_button_relay = 0;
        __delay_ms(4500);//program delay
        pickit_rel_1 = 0;
        pickit_rel_2 = 0;
        __delay_ms(100);//disconnect delay


        //        pickit_state = pickit_start; //start Pickit, this runs in the interrupt        
        //        while((pickit_state != pickit_idle) && (pickit_state != pickit_error));
        //        if(pickit_state == pickit_error)
        //            print_error("Programming", "Error");
    }
    else//if skip jumper is on don't program
    {
        print_screen("Pickit Programming", " skipped");
        __delay_ms(300);
    }
    // </editor-fold>

    //    comms_TX_TRIS = 0;
    //    print_screen("Toggle", "");
    //    while(1)
    //    {
    //        TX_one(0xAA);
    //        lcd_print_int(RX_one(), 1, 0, 0);
    //        __delay_ms(10);
    //        TX_one(0x55);
    //        lcd_print_int(RX_one(), 10, 0, 0);
    //          __delay_ms(100);     
    //    }

    debug_1 = 0;
    __delay_ms(100);
    debug_1 = 1;
    __delay_ms(100);
    debug_1 = 0;
    print_screen("Establishing", "comms");
    unsigned char device_type;
    unsigned char RX_data = 0;
    unsigned int i;
    for(i = 0; i < 100; i++)
    {
        TX_one(0x01); //establish comms
        device_type = RX_one_timeout();
        if((device_type == Device_Condo3) || (device_type == Device_RX3) || (device_type == Device_Mem_Copier))
            break;
        lcd_print_int(i, 11, 0, 0);
        lcd_print_int(device_type, 5, 0, 0);
        __delay_ms(50);
    }
    if(i >= 10)
    {
//        lcd_print_int(device_type, 10, 0, 0);
//        while(1);
        print_error("Comms Error", " 1");
    }
    else
    {
        lcd_print_int(i, 11, 0, 0);
        lcd_print_int(device_type, 5, 0, 0);
        __delay_ms(50);
    }

    if((device_type == Device_Condo3)|| (device_type == Device_Mem_Copier))
    {
        //test EEPROM read/write
        print_screen("Testing ", "EEPROM read/write");
        TX_one(0x10);
        RX_data = RX_one();
        if(RX_data == 0xEE)
            print_error("EEPROM error", " Read/Write");
        else if(RX_data != 0x1A)
            print_error("Comms Error", " 2");

        //Test EEPROM size
        unsigned int mem_size;
        print_screen("Test EEPROM ", " size");
        TX_one(0x20);
        RX_data = RX_one();
        mem_size = RX_data<<8;
        TX_one(0x21);
        RX_data = RX_one();
        mem_size = mem_size | RX_data;
        if(device_type == Device_Condo3)
        {
            if(mem_size != 0x2000)//0x0020 is 64k bit. 
                print_error("EEPROM size", " Error Condo3");
        }
        else if(device_type == Device_Mem_Copier)
        {
            if(mem_size != 0x0000)//0x0000 is 1meg bit. 
                print_error("EEPROM size", " Error Mem_copier");
        }
    }

    if((device_type == Device_Condo3)|| (device_type == Device_RX3))
    {
        //Test Relay
        Vout_set(12);
        print_screen("Test Relay", "");
        if(relay_NC_P4 == 0 && relay_NO_P8 == 1)//relay de-energized
        {
            TX_one(0x30);
            if(RX_one() == 0x3A)
            {
                __delay_ms(50); //time for relay to switch
                if(relay_NC_P4 == 1 && relay_NO_P8 == 0)//relay energized
                {
                    TX_one(0x31);
                    if(RX_one() == 0x3A)
                    {
                        __delay_ms(50); //time for relay to switch
                        if(relay_NC_P4 == 0 && relay_NO_P8 == 1)//relay de-energized
                        {
                            print_screen("Test Relay", "Pass");
                        }
                        else
                            print_error("Relay de-energized", " Error 2");
                    }
                    else
                        print_error("Comms Error", " 3");
                }
                else
                    print_error("Relay energized", " Error");
            }
            else
                print_error("Comms Error", " 4");
        }
        else
            print_error("Relay de-energized", " Error 1");
    }

    if((device_type == Device_Condo3)|| (device_type == Device_RX3))
    {
        //Test CH2 
        Vout_set(15);
        print_screen("Test CH2", "");
        TX_one(0x41); //turn channel off
        if(RX_one() == 0x4A)
        {
            __delay_ms(10); //FET switch time  
            if(CH2_P10_in == 1 && CH3_P13_in == 1)//both channels off
            {
                TX_one(0x40); //turn channel on
                if(RX_one() == 0x4A)
                {
                    __delay_ms(10);
                    if(CH2_P10_in == 0 && CH3_P13_in == 1)//CH2 on
                    {
                        TX_one(0x41); //turn channel off
                        if(RX_one() == 0x4A)
                        {
                            CH2_high_current_K13_P10 = 1; //relay on, supply high current to CH2
                            __delay_ms(50); //relay switch time
                            TX_one(0x40); //turn channel on
                            if(RX_one() == 0x4A)
                            {
                                __delay_ms(50); //FET switch time
                                tj_timer = 1500;
                                while(tj_timer && CH2_P10_in == 0);
                                if(CH2_P10_in == 1 && CH3_P13_in == 1)//CH2 off (PTC trip)
                                {
                                    CH2_high_current_K13_P10 = 0; //relay off,
                                    TX_one(0x41); //turn channel off
                                    RX_one();
                                    __delay_ms(150); //PTC reset time
                                    TX_one(0x40); //turn channel on
                                    RX_one();
                                    __delay_ms(20); //FET switch time
                                    if(CH2_P10_in == 0 && CH3_P13_in == 1)//CH2 on (PTC reset)
                                    {
                                        TX_one(0x41); //turn channel off
                                        if(RX_one() == 0x4A)
                                        {
                                            __delay_ms(10);
                                            if(CH2_P10_in == 1 && CH3_P13_in == 1)//both channels off
                                            {
                                                print_screen("Test CH2", "ok");
                                            }
                                            else
                                                print_error("CH2 - OFF", " Error 2");
                                        }
                                        else
                                            print_error("Comms Error", " 5");
                                    }
                                    else
                                        print_error("CH2 - ON", "PTC Error reset");
                                }
                                else
                                {
                                    trip_current = get_30v_current();
                                    print_error("CH2 - ON", "PTC Error trip");
                                }
                            }
                            else
                                print_error("Comms Error", " 6a");
                        }
                        else
                            print_error("Comms Error", " 6b");
                    }
                    else
                        print_error("CH2 - ON", " Error");
                }
                else
                    print_error("Comms Error", " 6");
            }
            else
                print_error("CH2 - OFF", " Error 1");
        }
        else
            print_error("Comms Error", " 7");


        //Test CH3 
        print_screen("Test CH3", "");
        TX_one(0x51); //turn channel off
        if(RX_one() == 0x5A)
        {
            __delay_ms(10); //FET switch time  
            if(CH2_P10_in == 1 && CH3_P13_in == 1)//both channels off
            {
                TX_one(0x50); //turn channel on
                if(RX_one() == 0x5A)
                {
                    __delay_ms(10);
                    if(CH2_P10_in == 1 && CH3_P13_in == 0)//CH2 on
                    {
                        TX_one(0x51); //turn channel off
                        if(RX_one() == 0x5A)
                        {
                            CH3_high_current_K6_P13 = 1; //relay on, supply high current to CH2
                            __delay_ms(50); //relay switch time
                            TX_one(0x50); //turn channel on
                            if(RX_one() == 0x5A)
                            {
                                __delay_ms(50); //FET switch time
                                tj_timer = 1500;
                                while(tj_timer && CH3_P13_in == 0);
                                if(CH2_P10_in == 1 && CH3_P13_in == 1)//CH2 off (PTC trip)
                                {
                                    CH3_high_current_K6_P13 = 0; //relay off,
                                    TX_one(0x51); //turn channel off
                                    RX_one();
                                    __delay_ms(150); //PTC reset time
                                    TX_one(0x50); //turn channel on
                                    RX_one();
                                    __delay_ms(20); //FET switch time
                                    if(CH2_P10_in == 1 && CH3_P13_in == 0)//CH2 on (PTC reset)
                                    {
                                        TX_one(0x51); //turn channel off
                                        if(RX_one() == 0x5A)
                                        {
                                            __delay_ms(10);
                                            if(CH2_P10_in == 1 && CH3_P13_in == 1)//both channels off
                                            {
                                                print_screen("Test CH3", "ok");
                                            }
                                            else
                                                print_error("CH3 - OFF", " Error 2");
                                        }
                                        else
                                            print_error("Comms Error", " 8");
                                    }
                                    else
                                        print_error("CH3 - ON", "PTC Error reset");
                                }
                                else
                                    print_error("CH3 - ON", "PTC Error trip");
                            }
                            else
                                print_error("Comms Error", " 8b");
                        }
                        else
                            print_error("Comms Error", " 8a");
                    }
                    else
                        print_error("CH3 - ON", " Error");
                }
                else
                    print_error("Comms Error", " 9");
            }
            else
                print_error("CH3 - OFF", " Error 1");
        }
        else
            print_error("Comms Error", " 10");
    }

    if((device_type == Device_Condo3)|| (device_type == Device_RX3))
    {
        //test RF - 3v3
        print_screen("Test RF", "3v3");
        TX_one(0x70);
        RX_data = RX_one();
        TX_one(0x70); //it seems to work better the second time
        RX_data = RX_one();
        if(RX_data == 0)
            print_error("Comms Error", " 11");
        else if(RX_data == 0xFF)
            print_error("Comms Error", " 12");
        else if(RX_data > 179)
            print_error("Test RF 3v3", " Error - High");
        else if(RX_data < 153)
            print_error("Test RF 3v3", " Error - Micrel");
        else if(RX_data < 158)
            print_error("Test RF 3v3", " Error - Low");

        print_screen("Test RF", "3v3 - OK");

        //test RF - RSSI
        testjig_timer = T0_3s;
        RF_out = 1;
        __delay_ms(200); //tested to 100ms
        print_screen("Test RF", "RSSI");
        TX_one(0x60);
        RX_data = RX_one();
        TX_one(0x60); //it seems to work better the second time
        RX_data = RX_one();
        
//        int i;
//        for(i = 0; i < 50; i++)
//        {
//            TX_one(0x60);
//            RX_data = RX_one();
//            lcd_print_int(RX_data, 5, 0, 1);
//            __delay_ms(2000);
//        }

        
        RF_out = 0;
        if(RX_data == 0)
            print_error("Comms Error", " 13");
        else if(RX_data == 0xFF)
            print_error("Comms Error", " 14a");
        else if(RX_data == 0xEE)
            print_error("Comms Error", " 14b");
                else if(RX_data >230)
            print_error("Comms Error", " 14c");
        else if(RX_data > 120)
        {
            lcd_print_int(RX_data, 5, 0, 1);
            __delay_ms(5000);
            print_error("Test RF RSSI", " Error - High");
        }
        else if(RX_data < 90)
            print_error("Test RF RSSI", " Error - Low");
        print_screen("Test RF", "RSSI - OK");
     
        __delay_ms(100);
        
        print_screen("Test RF", "Decode");
        int j;
        RX_data = 0;
        for(j = 0; j < 50; j++)
        {
            // test RF decode
            testjig_timer = T0_3s;
            RF_out = 1;
            __delay_ms(200); //tested to 100ms
            for(i = 0; i < 5; i++)
            {
                TX_one(0x61);
                RX_data = RX_one();
                if(RX_data == 1)//break if pass
                    break;
                __delay_ms(200); //allow time for interfering remote to stop
            }
            RF_out = 0;
            if(RX_data == 1)//break if pass
                break;
            
            print_screen("Test RF", " cycling power");
            power_supply_set(NONE);
            power_5v = 0; //stop powering  5v rail
            CH2_high_current_K13_P10 = 0;
            CH3_high_current_K6_P13 = 0;
             comms_TX_TRIS = 1;//set as input, stop pulling this high
            while(ADC_get_val(ADC_5v_test) > P5_5v_test_1v);//wait for voltage to drop
            power_supply_set(AC1);
            __delay_ms(200);

            for(i = 0; i < 10; i++)
            {
                TX_one(0x01); //establish comms
                unsigned char response;
                response = RX_one_timeout();
                if((response == Device_Condo3) || (response == Device_RX3))
                    break;
                lcd_print_int(i, 11, 0, 0);
                lcd_print_int(device_type, 5, 0, 0);
                __delay_ms(50);
            }
        }
        if(RX_data == 1)//New key
            print_screen("Test RF", "Decode New - OK");
        else if(RX_data == 2)//New key
            print_error("Test RF", "Decode Old -Fail");
        else if(RX_data == 3)//decode
            print_error("Test RF", "Error - Decode");
        else
            print_error("Comms Error", " 15");

       
    }

    if(device_type == Device_Mem_Copier)
    {
        //Test T4 bus comms (CAN)
        print_screen("T4 bus (CAN)", " ");
        TX_one(0xA0);
        RX_data = RX_one();
        if(RX_data != 0xAA)
            print_error("Comms Error", " 18");

        //Test T4 bus comms (I2C)
        //no test pads, test not possible
    }

    if((device_type == Device_Condo3) || (device_type == Device_RX3) || (device_type == Device_Mem_Copier))
    {
        //Master Erase
        print_screen("Master Erase ", " ");
        TX_one(0x82);
        RX_data = RX_one();
        if(RX_data != 0x8A)//New key
            print_error("Comms Error", " 17");
    }

    if((device_type == Device_Condo3) || (device_type == Device_RX3))
    {
        //Set number of channels and users
        print_screen("Set # of ", " channels");
        TX_one(0x81);
        RX_data = RX_one();
        if(RX_data != 0x8A)//New key
            print_error("Comms Error", " 16");
    }

    if((device_type == Device_Condo3) || (device_type == Device_RX3)|| (device_type == Device_Mem_Copier))
    {
        //7seg, buzzer all on --> UP
        print_screen("7seg, buzzer,  ", " button test");
        TX_one(0x90);
        comms_TX_TRIS = 1; //allow DUT to controll screen
        __delay_ms(100);
        while(comms_MCLR_input == 1);
        print_screen("waiting  ", " for comms");
        //    __delay_ms(300);
        //    RX_data = RX_one();
        //    if(RX_data != 0x9A)//New key
        //        print_error("Comms Error", " 17");
    }

    testjig_done(1, device_type);


    // <editor-fold defaultstate="collapsed" desc="Testing">



    // </editor-fold>


    testjig_done(1, device_type);
}

void testjig_done(unsigned char state, unsigned char device_type)
{
    while(1)
    {
        RF_out = 0;
        power_supply_set(NONE);
        power_5v = 0; //stop powering  5v rail
        CH2_high_current_K13_P10 = 0;
        CH3_high_current_K6_P13 = 0;
          comms_TX_TRIS = 1;//set as input, stop pulling this high
        debug_fast_tx(11110);
        debug_fast_tx(state);
        switch(state)
        {
            case 1://pass
                if(device_type == Device_Condo3)
                    print_screen("Condo3 Pass", " Remove board");
                else if(device_type == Device_RX3)
                    print_screen("RX3 Pass", " Remove board");
                else if(device_type == Device_Mem_Copier)
                    print_screen("Mem_Copier Pass", " Remove board");
                else
                    print_screen("Multi_RX Pass", " Remove board");
                debug_fast_tx(11111);
                buzz_one(100);
                __delay_ms(200);
                buzz_one(100);
                while(board_detect == 0);
                __delay_ms(100);
                state = 0xFF;
                break;
            case 2:
                debug_fast_tx(11112);

                if(device_type == Device_Condo3)
                    print_screen("Condo3 ", " Testjig");
                else if(device_type == Device_RX3)
                    print_screen("RX3 ", " Testjig");
                else if(device_type == Device_Mem_Copier)
                    print_screen("Mem_Copier Pass", " Testjig");
                else
                    print_screen("Multi_RX", "Testjig");
                __delay_ms(2000);
                print_screen("Testjig Power-up", "wait...");
                __delay_ms(3000);
                state = 0xFF;
                break;
        }
        debug_fast_tx(11113);
        if(device_type == Device_Condo3)
            print_screen("Condo3 Testjig", "-> Insert PCB");
        else if(device_type == Device_RX3)
            print_screen("RX3 Testjig", "-> Insert PCB");
        else if(device_type == Device_Mem_Copier)
            print_screen("Mem_Copier Pass", "-> Insert PCB");
        else
            print_screen("Multi_RX Testjig", "-> Insert PCB");
        while(board_detect == 1);
        __delay_ms(100);
        if(device_type == Device_Condo3)
            print_screen("Condo3 Testjig", " Press START");
        else if(device_type == Device_RX3)
            print_screen("RX3 Testjig", " Press START");
        else if(device_type == Device_Mem_Copier)
            print_screen("Mem_Copier Pass", " Press START");
        else
            print_screen("Multi_RX Testjig", " Press START");
        while(board_detect == 0);
        __delay_ms(100);
    }
}

void buzz_one(unsigned int time)
{
    buzz_timer = time;
    buzzer = 1;
}

// <editor-fold defaultstate="collapsed" desc="LCD Functions : Functions that print things to the LCD">
#define test_bit(var,bitno) (var & (1UL << bitno))
#define bitset(var, bitno) ((var) |= 1UL << (bitno))

void i2c_Start(void)
{
    //SEN = 1;         /* Start condition enabled */
    SSP1CON2bits.SEN = 1;
    // while(SEN);      /* automatically cleared by hardware */
    while(SSP1CON2bits.SEN == 1);
    /* wait for start condition to finish */
}

void i2c_Write(unsigned char data)
{
    SSP1BUF = data; /* Move data to SSPBUF */
    while(SSP1STATbits.BF == 1); /* wait till complete data is sent from buffer */
    while((SSP1CON2 & 0x1F) || (SSP1STAT & 0x04)); /* wait for any transfer */
}

void i2c_Stop(void)
{
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN == 1);
    /* PEN automatically cleared by hardware */
}

void i2c_Write_string(const unsigned char *data)
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x40); // data write to display RAM follows...

    while(*data != NULL) //NULL('\0')
    {
        i2c_Write(*data);
        data++;
    }
    i2c_Stop();
}

void i2c_write_char(unsigned char char_data)
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x40); // data write to display RAM follows...
    i2c_Write(char_data);
    i2c_Stop();
    //    __delay_us(27);
}

void init_i2c_lcd_5v(unsigned char contrast, unsigned char single1_double2) //contrast between 0 and 15.
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00); //command mode...
    i2c_Write(0x39); //0, 0, function set, 8-bit, 2-line, normal height, extended instruction set
    i2c_Write(0x1C); //0, 0, 0, int osc frequency/bias select, bias=1/4, Oscilator = 192Hz
    i2c_Write(0x70 | contrast); //0111 (contrast set), Contrast low bits=1000
    i2c_Write(0x50); //0101 (power/icon/contrast), Icon display off, booster circuit off, contrast high bits=00
    i2c_Write(0x6C); //0110 (follower control), internal follower on, follower bits = 100
    if(single1_double2 == 1)
        i2c_Write(0x38); //0, 0, function set, 8-bit, 2-line, normal height, normal instruction set
    if(single1_double2 == 2)
        i2c_Write(0x34); //0, 0, function set, 8-bit, 2-line, normal height, normal instruction set
    i2c_Write(0x0C); //0,0,0,0,display on/off, Display on, curser off, blink off
    i2c_Write(0x06); //0,0,0,0,0, Entry Mode Set, Increment (print right), Don't shift Entire display
    i2c_Write(0x01); //clear display
    __delay_ms(2); //clear display delay
    i2c_Stop();
}

//void init_i2c_lcd_3v3(unsigned char contrast, unsigned char single0x38_double0x34) //contrast between 0 and 15. //in bootloader page, not updateable
//{
//        i2c_Start();
//    i2c_Write(i2c_device); //0x7C slave address
//    i2c_Write(0x00); //command mode...
//    i2c_Write(0x39); //0, 0, function set, 8-bit, 2-line, normal height, extended instruction set
//    i2c_Write(0x1C); //0, 0, 0, int osc frequency/bias select, bias=1/4, Oscilator = 277Hz
//    i2c_Write(0x70 | contrast); //0111 (contrast set), Contrast low bits=1000
//    char cont_high = 0x54 | ((contrast & 0b00110000) >> 4);
//    i2c_Write(cont_high); //0101 (power/icon/contrast), Icon display off, booster circuit off, contrast high bits=00
//    i2c_Write(0x6C); //0110 (follower control), internal follower on, follower bits = 100
//    //    if (single0x38_double0x34 == 1)
//    i2c_Write(single0x38_double0x34); //0, 0, function set, 8-bit, 2-line, normal height, normal instruction set
//    //    if (single0x38_double0x34 == 2)
//    //        i2c_Write(0x34); //0, 0, function set, 8-bit, 2-line, normal height, normal instruction set
//    i2c_Write(0x0C); //0,0,0,0,display on/off, Display on, curser off, blink off
//    i2c_Write(0x06); //0,0,0,0,0, Entry Mode Set, Increment (print right), Don't shift Entire display
//    i2c_Write(0x01); //clear display
//    unsigned int i;
//    for(i = 1116; i; i--); //500us delay
//    i2c_Stop();
//}

void clear_lcd(void)
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00);
    i2c_Write(0x01); //clear display
    __delay_ms(2);
    i2c_Stop();
}

void i2c_first_line(void)
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00);
    i2c_Write(0x80); //80 = first line, C0 = second line
    __delay_us(27);
    i2c_Stop();
}

void i2c_second_line(void)
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00);
    i2c_Write(0xC0); //80 = first line, C0 = second line
    __delay_us(27);
    i2c_Stop();
}

void lcd_set_cursor(unsigned char row, unsigned char col) //Set the cursor to x(row) y(col) position
{
    unsigned char LCD_position = 1;
    col--;
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00); //command mode...
    LCD_position = col & 0b11001111;
    if(row == 1)
        LCD_position |= 0b10000000;
    else
        LCD_position |= 0b11000000;
    i2c_Write(LCD_position); //80 = first line, C0 = second line  (DDRAM Adress)
    i2c_Stop();
}


// <editor-fold defaultstate="collapsed" desc="Unused LCD function">

void lcd_blink_cursor(void) //Make the LCD cursor blink
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00); //command mode...
    i2c_Write(0x0F); //Blink cursor
    i2c_Stop();
}

//void lcd_cursor_OFF(void)                    //Make the LCD cursor blink
//{
//    i2c_Start();
//    i2c_Write(i2c_device);      //0x7C slave address
//    i2c_Write(0x00);            //command mode...
//    i2c_Write(0x0C);            //Blink cursor
//    i2c_Stop();
//}
//
//void lcd_cursor_ON(void)                    //Make the LCD cursor blink
//{
//    i2c_Start();
//    i2c_Write(i2c_device);      //0x7C slave address
//    i2c_Write(0x00);            //command mode...
//    i2c_Write(0x0E);            //Blink cursor
//    i2c_Stop();
//}
//
//void lcd_home (void)
//{
//                i2c_Start();
//                i2c_Write(i2c_device);      //0x7C slave address
//                i2c_Write(0x00);            //command mode...
//                i2c_Write(0x02);
//                i2c_Stop();
//                __delay_us(1100);           //wait for LCD to complete instruction
//}
//
//void lcd_scroll_left (void)
//{
//    i2c_Start();
//    i2c_Write(i2c_device);      //0x7C slave address
//    i2c_Write(0x00);            //command mode...
//    i2c_Write(0x18);
//    i2c_Stop();
//    __delay_us(27);
//}
//
//void lcd_scroll_right (void)
//{
//                i2c_Start();
//                i2c_Write(i2c_device);      //0x7C slave address
//                i2c_Write(0x00);            //command mode...
//                i2c_Write(0x1C);
//                i2c_Stop();
//                __delay_us(27);
//}
//
//void binary_print(unsigned char data)
//{
//    if (data&0b10000000)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b01000000)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00100000)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00010000)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00001000)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00000100)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00000010)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//    if (data&0b00000001)
//        i2c_write_char (49);
//    else
//        i2c_write_char (48);
//}
//void debug_flash(unsigned char start_ptr)       //a function that prints a flash page to the LCD. for debugging only
//{
//    while (any_button_pressed());
//    unsigned char data;
//    lcd_set_cursor(1, 1);
//    for (char x = 0; x < 16; x++)
//    {
//        if (x == 8)
//            lcd_set_cursor(2, 1);
//        data = read_flash_1(start_ptr + x);
//        data = (data & 0xF0) >> 4;
//        //        i2c_write_char (data + 48);
//        if (data < 10)
//            i2c_write_char (data + 48);
//        else i2c_write_char (data + 55);
//
//        data = read_flash_1(start_ptr + x);
//        data = (data & 0x0F);
//        if (data < 10)
//            i2c_write_char (data + 48);
//        else i2c_write_char (data + 55);
//    }
//    while (any_button_pressed());
//    while (right_button == 1 && exit_button == 1);
//    while (exit_button == 1)
//    {
//        lcd_set_cursor(1, 1);
//        for (char y = 0; y < 16; y++)
//        {
//            if (y == 8)
//                lcd_set_cursor(2, 1);
//            data = read_flash_1(start_ptr + y + 16);
//            data = (data & 0xF0) >> 4;
//            if (data < 10)
//                i2c_write_char (data + 48);
//            else i2c_write_char (data + 55);
//
//            data = read_flash_1(start_ptr + y + 16);
//            data = (data & 0x0F);
//            if (data < 10)
//                i2c_write_char (data + 48);
//            else i2c_write_char (data + 55);
//        }
//    }
//}
//
//
//void lcd_scroll_double(unsigned char end, unsigned char dir)           //a routine to scroll the display 'end' positions, dir=0 -> Left | dir=1 ->right
//{
//    for (unsigned char i = 0; i !=100; i++)
//    {
//        if (left_button == 1 && right_button == 1 && exit_button == 1 && set_button == 1)
//            __delay_ms(4);                  //This should be less than 10ms for sensitivity
//    }
//    if (dir == 0)
//    {
//        lcd_scroll_left();
//        lcd_scroll_left();
//    } else
//    {
//        lcd_scroll_right();
//        lcd_scroll_right();
//    }
//    scroll_count++;
//    if (scroll_count == end||scroll_count==0)
//    {
//        for (unsigned char i = 0; 1!=200; i++)
//        {
//            if (left_button == 1 && right_button == 1 && exit_button == 1 && set_button == 1)
//                __delay_ms(4);                  //This should be less than 10ms for sensitivity
//        }
//        lcd_home();
//        scroll_count = 0;
//    }
//}
//
//void lcd_scroll(unsigned char end, unsigned char dir)           //a routine to scroll the display 'end' positions, dir=0 -> Left | dir=1 ->right
//{
//        for (unsigned char i = 0; i != 100; i++)
//    {
//        if (left_button == 1 && right_button == 1 && exit_button == 1 && set_button == 1)
//            __delay_ms(4);
//    }
//    if (dir == 0)
//        lcd_scroll_left();
//    else
//        lcd_scroll_right();
//    scroll_count++;
//    if (scroll_count == end)
//    {
//        lcd_home();
//        scroll_count = 0;
//    }
//    for (unsigned char i = 0; i !=100; i++)
//    {
//        if (left_button == 1 && right_button == 1 && exit_button == 1 && set_button == 1)
//            __delay_ms(4);
//    }
//}


// </editor-fold>

//void contrast_adj(void)                         // a function to adjust the contrast by calling contrast_set().
//{
//    unsigned char contrast = (0x80 & 0b11110000) >> 4;
//    print_screen("Set Contrast", "<              >");               // less memory to print spaces than print on new line
//    button_beep(button_beep_delay);
//    while (set_button == 0);                                    //wait for button release
//    while (set_button == 1 && exit_button == 1)
//    {
//        contrast = button_wrap_around(contrast, 0, 15, 1);      //count, min, max, wrap
//        lcd_set_cursor(2, 8);
//        ten_thou_conv(contrast);         //This can be out by as much as 11% due to resistor tolerance.
//        i2c_write_char (ten + 48);
//        i2c_write_char (unit + 48);
//        contrast_set(contrast);
//        flash_write_1(contrast << 4, MEM_contrast_setting); //load - (data, location in flash frame)
//    }
//    if (exit_button == 0)
//    {
//        button_beep(button_beep_delay * 64);
//        contrast_set(0x08);                                     //set default contrast
//        popup("Constrast set", "Exited");
//    } else save_beep();
//    while (set_button == 0);                                    //wait for button release
//}

void contrast_set(unsigned char contrast) //a function to set the contrast to the parsed char
{
    char cont_low = 0x70;
    char cont_high = 0x50;
    cont_low |= contrast;
    cont_high |= (contrast & 0b00110000) >> 4;
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00); //command mode...
    i2c_Write(0x39); //0, 0, function set, 8-bit, 2-line, normal height, extended instruction set
    i2c_Write(cont_low); //0111 (contrast set), Contrast low bits=1000
    i2c_Write(cont_high); //0101 (power/icon/contrast), Icon display off, booster circuit off, contrast high bits=00
    i2c_Write(0x38); //0, 0, function set, 8-bit, 2-line, normal height, normal instruction set
    i2c_Stop();
}

void ten_thou_conv(int n) //a function to calculate 5 digits quickly
{//about 200us slower that 'hund_conv', much more efficient than 'decimal_conv'
    unit = n & 0x1F;
    ten = (n >> 5) & 0xF;
    hund = (n >> 9) & 0xF;
    thou = (n >> 13) & 0x7;
    //calculate unit
    unit = 2 * (thou + hund + ten) + unit;
    ten_thou = (unit * 0x67) >> 10;
    unit = unit - 10 * ten_thou;
    //calculate ten
    ten = 9 * thou + hund + 3 * ten + ten_thou;
    ten_thou = (ten * 0x67) >> 10;
    ten = ten - 10 * ten_thou;
    //calculate hund
    hund = thou + 5 * hund + ten_thou;
    ten_thou = (hund * 0x67) >> 10;
    hund = hund - 10 * ten_thou;
    //calculate thou
    thou = 8 * thou + ten_thou;
    ten_thou = (thou * 0x1A) >> 8;
    thou = thou - 10 * ten_thou;
}

void hund_conv(int n) //a function to calculate 3 digits quickly
{
    unit = n & 0x1F;
    ten = (n >> 5) & 0xF;
    hund = (n >> 9) & 0xF;
    thou = (n >> 13) & 0x7;
    //calculate unit
    unit = 2 * (thou + hund + ten) + unit;
    ten_thou = (unit * 0x67) >> 10;
    unit = unit - 10 * ten_thou;
    //calculate ten
    ten = 9 * thou + hund + 3 * ten + ten_thou;
    ten_thou = (ten * 0x67) >> 10;
    ten = ten - 10 * ten_thou;
    //calculate hund
    hund = thou + 5 * hund + ten_thou;
    ten_thou = (hund * 0x67) >> 10;
    hund = hund - 10 * ten_thou;
}

//void popup(const unsigned char *top, const unsigned char *bottom)               //A function to display 'top' and 'bottom' on the LCD for 'pop_up delay' ms
//{
//    // if print_screen() is called here
//    clear_lcd();
//    i2c_first_line();
//    i2c_Write_string(top);
//    i2c_second_line();
//    i2c_Write_string(bottom);
//    while (any_button_pressed());               // wait for all buttons to be released.
//    for (char x = 100; x != 0; x--)             //Loop that only delays if a button is not pressed
//        if (!any_button_pressed())              // if no button is pressed.
//            delay_ms(popup_delay);
//    if (any_button_pressed())
//        button_beep(button_beep_delay);
//    while (any_button_pressed());               // wait for all buttons to be released.
//    clear_lcd();
//}

//void popup_elipses(const unsigned char *top, const unsigned char *bottom, unsigned char elipses1)       // this is a cool feature to pretend that the micro is busy. but the Elipses should rather be implimented in the funtion that it is 'pretending' to do.
//{
//    // if print_screen() is called here
//    clear_lcd();
//    i2c_first_line();
//    i2c_Write_string(top);
//    i2c_second_line();
//    i2c_Write_string(bottom);
//    while (any_button_pressed());               // wait for all buttons to be released.
//    for (char x = 100; x != 0; x--)             //Loop that only delays if a button is not pressed
//    {
//        if (!any_button_pressed())              // if no button is pressed.
//        {
//            delay_ms(popup_delay);
//            //            if (elipses1)
//            if (elipses1 && (x == 25 || x == 50 || x == 75 ))
//                i2c_write_char ('.');
//        }
//    }
//    if (any_button_pressed())
//        button_beep(button_beep_delay);
//    while (any_button_pressed());               // wait for all buttons to be released.
//    clear_lcd();
//}

void print_error(const unsigned char *top, const unsigned char *bottom) //A function to display 'top' and 'bottom' on the LCD perminanently
{
    power_supply_set(NONE);
    power_5v = 0; //stop powering  5v rail
    CH2_high_current_K13_P10 = 0;
    CH3_high_current_K6_P13 = 0;
      comms_TX_TRIS = 1;//set as input, stop pulling this high
    RF_out = 0;
    debug_fast_tx(12345);
    buzz_one(1000);
    while(1)
    {
        clear_lcd();
        i2c_first_line();
        i2c_Write_string(top);
        i2c_second_line();
        i2c_Write_string(bottom);
        __delay_ms(2000);
        print_screen("Press START", " to restart test");
        //        lcd_print_int(trip_current, 1, 0, 0);
        __delay_ms(1200);
    }
}

void print_screen(const unsigned char *top, const unsigned char *bottom) //A function to display 'top' and 'bottom' on the LCD perminanently
{
    clear_lcd();
    i2c_first_line();
    i2c_Write_string(top);
    i2c_second_line();
    i2c_Write_string(bottom);
}

void print_second_line(const unsigned char *bottom) //A function to display 'top' and 'bottom' on the LCD perminanently
{
    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x00); //command mode...
    i2c_Write(0b11000000); //80 = first line, C0 = second line  (DDRAM Adress)
    i2c_Stop();

    i2c_Start();
    i2c_Write(i2c_lcd_address); //0x7C slave address
    i2c_Write(0x40); // data write to display RAM follows...
    for(unsigned char x = 0; x != 16; x++)
    {
        if(*bottom != NULL) //NULL('\0')
        {
            i2c_Write(*bottom);
            bottom++;
        }
        else
            i2c_Write(0x20); //print blank
    }
    i2c_Stop();
}

void lcd_bar_create(void) //function to create characters with number of lines equal to position+1. (for bar)
{
    unsigned char bar_one[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111
    };
    lcd_createChar(0, bar_one);

    unsigned char bar_two[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b11111
    };
    lcd_createChar(1, bar_two);

    unsigned char bar_three[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(2, bar_three);

    unsigned char bar_four[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(3, bar_four);

    unsigned char bar_five[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(4, bar_five);

    unsigned char bar_six[8] = {
        0b00000,
        0b00000,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(5, bar_six);

    unsigned char bar_seven[8] = {
        0b00000,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(6, bar_seven);

    unsigned char bar_eight[8] = {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111
    };
    lcd_createChar(7, bar_eight);
}

void lcd_print_bar(unsigned char num) // a function to print a bar of 'num' value. 'size' sets the width, either 8 or 16 blocks
{
    for(int i = 0; i != num + 1; i++)
    {
        i2c_write_char(i);
    }
}

void lcd_createChar(unsigned char location, unsigned char charmap[]) //a function to create a character at a specific location given a character map
{
    location &= 0x07; // we only have 8 locations 0-7
    i2c_Start();
    i2c_Write(i2c_lcd_address);
    i2c_Write(0x00); //command mode
    i2c_Write(0x40 | (location << 3)); //set location
    i2c_Stop();
    __delay_us(27); // >26.3us
    for(int i = 0; i != 8; i++)
    {
        i2c_write_char(charmap[i]); //write data sequentially
    }
    clear_lcd();
}

//void flash_created_character_zero (unsigned char pos)                                       // this function flashes the character in position '0' every 1s.
//{
//    lcd_set_cursor(2, pos);
//    i2c_write_char(32);                      //print blank
//    while (set_button == 1 && exit_button == 1 && right_button == 1  && x != 100)     //off for 100*5ms. also checking buttons
//    {
//        __delay_ms(5);
//        x++;
//    }
//    x = 0;
//    lcd_set_cursor(2, pos);
//    i2c_write_char(0);                      //print blank
//    while (set_button == 1 && exit_button == 1 && right_button == 1 && x != 100)     //on for 100*5ms. also checking buttons
//    {
//        __delay_ms(5);
//        x++;
//    }
//    x = 0;
//}

//void page_down_sunroutine(const unsigned char *page11, const unsigned char *page12, const unsigned char *page21, const unsigned char *page22)  //function to print flashing arrow and page down on RIGHT, skip on SET
//{
//    lcd_createChar(0, down_arrow);
//    print_screen(page11, page12);
//    while (set_button == 0);
//    while (set_button == 1 && exit_button == 1 && right_button == 1)
//    {
//        flash_created_character_zero (16);                       //flash a down arrow at position (pos)
//    }
//    if (right_button == 0)
//    {
//        button_beep(button_beep_delay);
//        print_screen(page21, page22);
//    }
//    while (right_button == 0);
//    while (set_button == 1 && exit_button == 1);
//    button_beep(button_beep_delay);
//    while (set_button == 0);                        //wait for set button to be pressed
//}

//void print_number(unsigned int number, unsigned int pos, unsigned char decimal_delay)            //prints 3 digit number at given LCD position can popup and set decimal
//{
//    lcd_set_cursor(2, pos);
//    ten_thou_conv(number);
//
//    if (hund != 0 || decimal_delay & 0x04)                                      //do not print leading 100s '0'.
//        i2c_write_char(hund + 48);
//    else i2c_write_char(32);
//    if (decimal_delay & 0x04)i2c_write_char ('.');
//    i2c_write_char(ten + 48);
//    if (decimal_delay & 0x02)i2c_write_char ('.');
//    i2c_write_char(unit + 48);
//}

void lcd_print_int(unsigned int number, unsigned int pos, unsigned char decimal, unsigned char ten_thou_digit) //prints 3 digit number at given LCD position can popup and set decimal
{
    lcd_set_cursor(2, pos + 1 - ten_thou_digit); //adjust position by one if digit is not being printed.
    ten_thou_conv(number);
    if(ten_thou_digit)
    {
        if(ten_thou != 0 || decimal & 0x10) //do not print leading 100s '0'.
            i2c_write_char(ten_thou + 48);
        else i2c_write_char(' ');
    }
    if(decimal & 0x10)i2c_write_char('.');
    if(thou != 0 || ten_thou != 0 || decimal & 0x18) //do not print leading 100s '0'.
        i2c_write_char(thou + 48);
    else i2c_write_char(' ');
    if(decimal & 0x08)i2c_write_char('.');
    if(hund != 0 || thou != 0 || ten_thou != 0 || decimal & 0x1C) //do not print leading 100s '0'.
        i2c_write_char(hund + 48);
    else i2c_write_char(' ');
    if(decimal & 0x04)i2c_write_char('.');
    if(ten != 0 || hund != 0 || thou != 0 || ten_thou != 0 || decimal & 0x1E) //do not print leading 100s '0'.
        i2c_write_char(ten + 48);
    else i2c_write_char(' ');
    if(decimal & 0x02)i2c_write_char('.');
    i2c_write_char(unit + 48);
}

//void print_hr_min_min_s (unsigned int value, unsigned char pos)     //a function to time format settings
//{
//    save_beep();
//    lcd_set_cursor(2, pos);
//    unsigned char x = 0;
//    x = value / 60;
//    hund_conv(x);
//    if (ten != 0)                                           //do not print leading 100s '0'.
//        i2c_write_char(ten + 48);
//    else i2c_write_char(32);
//    i2c_write_char(unit + 48);
//    popup_print_number(value - (x * 60), 12, 1);
//}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Uart reciever communications">

//void init_uart2(void)
//{ //init uart, 57600baud, No-parity, 8-bits, 1-stop, No hardware flow control (57600 8N1 None)
//    TX2STA = 0b00100100;
//    RC2STA = 0b10010000;
//    BAUD2CON = 0b00001000;
//    SP2BRGH = 0x00; //57600baud
//    SP2BRGL = 66; //117647baud
//}

//void disable_uart(void)
//{
//    TX2STAbits.TXEN = 0; //disable transmitter
//    RC2STAbits.CREN = 0; //disable receiver
//    RC2STAbits.SPEN = 0; //disable UART
//    //    RF_In_3_RX_TRIS = 1; //RX is an input
//    //    RF_In_4_TX_TRIS = 1; //TX is an input
//}

//void enable_uart(void)
//{
//    TX2STAbits.TXEN = 1; //enable transmitter
//    //    RC2STAbits.CREN = 1; //enable receiver
//    RC2STAbits.SPEN = 1; //enable UART
//    uart_reset();
//    //    RF_In_3_RX_TRIS = 0; //RX is an output
//    //    RF_In_4_TX_TRIS = 0; //TX is an output
//}

void send_uart2(unsigned char uart2_data)
{
    while(TX2STAbits.TRMT == 0); //Transmission still in progress? or exit pressed
    TX2REG = uart2_data; //send data
}

void uart_reset(void)
{
    unsigned char dummy_clear;
    RC2STAbits.CREN = 0; // --RESET receiver
    NOP();
    RC2STAbits.CREN = 1;
    while(RC2IF == 1)
        dummy_clear = RC2REG; //make sure buffer is clear
}

unsigned char uart_receive_four(unsigned int uart1_timer)
{
    unsigned char uart_rx_byte_count1 = 1;
    uart_receive_timer = uart1_timer;
    while(uart_rx_byte_count1 < 2 && uart_receive_timer)//wait for the timer or byte recieved or exit button press
    {
        if(RC2IF == 1)
        {
            if((RC2STAbits.OERR == 1) || (RC2STAbits.FERR == 1)) //IF ANY RX ERROR OCCURS // RESET RX SESSION!! >>
            {//yes
                uart_reset();
                return UART1_RESET; //reset receive buffer & clear data
            }
            else
                uart_buff_work_recieve[uart_rx_byte_count1++] = RC2REG;
        }
    }
    if(uart_rx_byte_count1 == 2)
        return UART1_BUFFER_FULL;
    else
        return UART1_TIMEOUT;
}

unsigned char uart_send_receive_packet(unsigned char opcode, unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned int uart2_receive_timer)
{
    RC2STAbits.OERR = 0; //reset receive buffer & clear data
    send_uart2(opcode);
    if(opcode != 0x89) //does more data need to be sent
    {
        send_uart2(byte_1);
        send_uart2(byte_2);
        send_uart2(byte_3);
    }
    //    else
    //        __delay_us(500);
    return uart_receive_four(uart2_receive_timer); //return - USART1_TIMEOUT, USART1_BUFFER_FULL
}

// </editor-fold>

void init_i2c(void)
{
    lcd_reset = 1; //reset pin high
    __delay_ms(200);
    TRISC3 = 1; /* SDA and SCL as input pin */
    TRISC4 = 1; /* these pins must be configured as i/p for 1527 */
    SSP1STAT |= 0x80; /* Slew rate disabled */
    SSP1CON1 = 0x28; /* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    SSP1ADD = 0x14; /* 100kHz @ 4MHz Fosc */
}

void init_ADC(void)
{
    FVRCON = 0b10000011; //4.096v reference
    //    ADCON0 = 0b00000001;//channel 0, conversion not started, ADC on
    ADCON1 = 0b10100011; //right justified, Fosc/32, Vref is FVR
}

unsigned int ADC_get_val(unsigned char channel)
{
    unsigned int return_val;
    GIE = 0;
    ADCON0 = channel; //set ADC channel
    while(ADCON0bits.GO_nDONE == 1); //wait for conversion to complete
    return_val = ADRESH << 8 | ADRESL;
    GIE = 1;
    return return_val;
}

void init_current_sink(void)
{
    T2CON = 0b00000110; //timer on, interrupt every 1ms
    PR2 = 0xFA; //1ms
    PIE1bits.TMR2IE = 1; //enable timer interrupt

    T8CON = 0b00000100; //timer 8 on, prescaler = 1.
    PR8 = 0xFF;
    CCP9CON = 0b00001100; //CCP in PWM mode
    CCPTMRS2 = 0b00000001; //CCP9 based off timer 8
    CCPR9L = 0x00; //turn off PWM
}

void power_supply_set(unsigned char supply)
{
    switch(supply)
    {
        case AC1:
            power_supply_k7 = 1;
            power_supply_k8 = 1;
            power_supply_k9 = 1;
            break;
        case AC2:
            power_supply_k7 = 1;
            power_supply_k8 = 0;
            power_supply_k9 = 1;
            break;
        case BAT:
            power_supply_k7 = 0;
            power_supply_k8 = 0;
            power_supply_k9 = 0;
            break;
        case NONE:
            power_supply_k7 = 0;
            power_supply_k8 = 0;
            power_supply_k9 = 1;
            break;
    }
}

unsigned int get_30v_current(void)
{
    unsigned int temp;
    temp = ADC_get_val(ADC_30v_cur_sense);
    if(temp > 4)
        return ( temp * 3.4) - 16; //this is multiplied by 4 to get mA
    else
        return 0;
}

void Vout_set(int Volts)
{
    unsigned long digi_step = 0;
    //   unsigned long res_variable = (SMPS_REF*PASSIVE_RES)/(milliVolts-1);

    // float fitting = SMPS_GAIN*(float)(milliVolts) + SMPS_CONST;

    //   digi_step = 128-(res_variable*128)/DIGITAL_RES_MAX;
    switch(Volts)
    {
        case 6:
            digi_step = 20;
            break;
        case 7:
            digi_step = 39;
            break;
        case 8:
            digi_step = 52;
            break;
        case 9:
            digi_step = 62;
            break;
        case 10:
            digi_step = 70;
            break;
        case 11:
            digi_step = 76;
            break;
        case 12:
            digi_step = 81;
            break;
        case 13:
            digi_step = 85;
            break;
        case 14:
            digi_step = 88;
            break;
        case 15:
            digi_step = 91;
            break;
        case 16:
            digi_step = 94;
            break;
        case 17:
            digi_step = 96;
            break;
        case 18:
            digi_step = 98;
            break;
        case 20:
            digi_step = 102;
            break;
        case 25:
            digi_step = 112;
            break;
        default:
            digi_step = 0;
            break;
    }

    //   test_funct_display("step", (int)digi_step);

    //the above is the ideal, 

    digitalpot((unsigned char) digi_step);
}

void digitalpot(unsigned char value)
{
    i2c_Start();
    i2c_Write(i2c_pot_address); //0x5C slave address
    i2c_Write(0b00000000);
    i2c_Write(value);
    i2c_Stop();
    __delay_ms(20);
}

void debug_fast_tx(unsigned int data)
{
    GIE = 0;
    trigger = 0;
    //    NOP();
    //    NOP();
    //    NOP();
    //    NOP();
    //    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    unsigned char i;
    for(i = 0; i < 16; i++)
    {
        trigger = data & 0x01;
        data = data >> 1;
    }
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    trigger = 1;
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    GIE = 1;
}

void TX_one(unsigned char data)
{
    comms_TX_TRIS = 0;
    GIE = 0;//interrupts disabled to increase delay timing accuracy
    comms_TX = 0;
    __delay_us(105);

    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        comms_TX = data & 0x01;
        data = data >> 1;
        __delay_us(105);
    }
    comms_TX = 1;
    GIE = 1;
    __delay_us(100);
}

unsigned char RX_one(void)
{
    //    comms_RX_TRIS = 1; //make receive pin an input
    while(comms_RX == 1); //wait for start bit
    GIE = 0;//interrupts disabled to increase delay timing accuracy
    __delay_us(150);
    unsigned char i;
    unsigned char data = 0;
    for(i = 0; i < 8; i++)
    {
         debug_1 = 1;
        if(comms_RX == 0)
            data = data >> 1;
        else
            data = (data >> 1) | 0x80; // copies the old bit 0 into bit 7
          debug_1 = 0;
        __delay_us(105);
    }
    GIE = 1;
    __delay_us(100);
    //    debug_fast_tx(data);
    return data;
}

unsigned char RX_one_timeout(void)
{
    //    comms_RX_TRIS = 1; //make receive pin an input
    uart_receive_timer = 500; //66ms
    while(comms_RX == 1 && uart_receive_timer); //wait for start bit
    __delay_us(150);
    unsigned char i;
    unsigned char data = 0;
    for(i = 0; i < 8; i++)
    {
         debug_1 = 1;
        if(comms_RX == 0)
            data = data >> 1;
        else
            data = (data >> 1) | 0x80; // copies the old bit 0 into bit 7
          debug_1 = 0;
        __delay_us(100);
    }
    __delay_us(100);
    //    debug_fast_tx(data);
    return data;
}


//TODO:Tighten up tolerances when using 1% resistors
//TODO: write code to test when diodes are used instead of a relay
