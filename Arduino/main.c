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
    //if(value==0) PORTB &= 0xfe; else PORTB |= 0x01;
    PORTB ^= value;
}

void input_init(void){
    DDRD &= 0b10000011;  // PIN 2,3,4,5,6,7 as input
    PORTD |= 0b01111100; // Pull-up activated on PIN 2,3,4,5,6,7
}

unsigned char input_get(void){
    //return ((PIND&0b01111100)!=0)?1:0;
    return PIND ^ 0xFF;
}



int main(void){
    init_serial(9600);
    output_init();
    input_init();
    output_set(0xFF);

    unsigned int joystick[2]= {0,0};
    unsigned char message = 0x00;

    while(1){
        //joystick
        int k;
        for(k=0;k<2;k++){ //car ADC0 et ADC1
            ad_init(k); //initialise l'ADC
            unsigned int newjoystick = ad_sample(); //recupere la valeur convertie du joystick
            newjoystick=newjoystick>>2; //decallage a droite
            if(newjoystick!=joystick[k]){ //compare avec l'ancienne valeur
                joystick[k]=newjoystick; //si changement on enregistre la nouvelle valeur
                send_serial(joystick[k] | 0b00100000); //on envoie la nouvelle valeure sur la liaison serie apres avoir ajoute 001 en debut de message
            }
        }

        //boutons
        if ((PIND&0b01111100)!=0b01111100) //on envoie la valeur des bouttons que si changement d'etat detecte
        {
            unsigned char message_out = input_get(); //recupere la valeur des boutons
            send_serial(message_out>>2 | 0b00100000);//on envoie les nouvelles valeure sur la liaison serie apres avoir ajoute 001 en debut de message
        }

        //Leds
        //if (Serial.available()){
          message = get_serial();
          if(message & 0x40 == 0x40) output_set(!message);
          else if(message & 0x40 == 0x00) output_set(message);
       	//}
        _delay_ms(200);
    }

    return 0;
}
