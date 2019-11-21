#include <avr/io.h>		// for the input/output register
#include <util/delay.h>
#include <stdio.h>
// For the serial port

#define CPU_FREQ        16000000L       // Assume a CPU frequency of 16Mhz

void init_serial(int speed)
{
    /* Set baud rate */
    UBRR0 = CPU_FREQ/(((unsigned long int)speed)<<4)-1;

    /* Enable transmitter & receiver */
    UCSR0B = (1<<TXEN0 | 1<<RXEN0);

    /* Set 8 bits character and 1 stop bit */
    UCSR0C = (1<<UCSZ01 | 1<<UCSZ00);

    /* Set off UART baud doubler */
    UCSR0A &= ~(1 << U2X0);
}

void send_serial(unsigned char c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

unsigned char get_serial(void) 
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

// For the AD converter

void ad_init(unsigned char channel)   
{   
    ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);   
    ADMUX|=(1<<REFS0)|(1<<ADLAR);
    ADMUX=(ADMUX&0xf0)|channel;   
    ADCSRA|=(1<<ADEN);
}   

unsigned int ad_sample(void){
    ADCSRA|=(1<<ADSC);
    while(bit_is_set(ADCSRA, ADSC));
    return ADCH;
}

// For the I/O 

void output_init(void){
    DDRB |= 0b00111111; // PIN 8,9,10,11,12,13 as output
}

void output_set(unsigned char value){
    if(value==0) PORTB &= 0xfe; else PORTB |= 0x01;
}

void input_init(void){
    DDRD &= 0b10000011;  // PIN 2,3,4,5,6,7 as input
    PORTD |= 0b01111100; // Pull-up activated on PIN 2,3,4,5,6,7
}

unsigned char input_get(void){
    //return ((PIND&0b01111100)!=0)?1:0;
    return PIND ^ 0xFF;
}

// Dummy main

int main(void){
    init_serial(9600);
    output_init();
    input_init();
    
    unsigned int joystick[2]= {0,0};
    
    while(1){
        //joystick
#if 1
        int k;
        for(k=0;k<2;k++){
            ad_init(k);
            unsigned int newjoystick = ad_sample();
            newjoystick=newjoystick>>2;
            if(newjoystick!=joystick[k]){
                joystick[k]=newjoystick;
                send_serial(joystick[k] | 0b00100000);
            }
        }
#endif
#if 0
        //boutons:0
        if ((PIND&0b01111100)!=0b01111100)
        {
            unsigned char message_out = input_get();
            send_serial(message_out>>2 | 0b00100000);  
        }
#endif
        _delay_ms(20);
        
        //unsigned char message_in = get_serial();
        //output_set(message_in);
        
    }
    
    return 0;
}
