#include <msp430.h> 


/**
 * main.c
 *
 *  Autor: Ing. Alfredo Renter�a Villanueva
 *  Fecha: 18-06-2020
 *
 */

unsigned char rx_buffer[512];
unsigned char rx_counter = 0;
unsigned char *rx_buffer_pointer = &rx_buffer[0];

const unsigned char test[5] = {"AT\r\n\0"};
const unsigned char reset[9] = {"AT+RST\r\n\0"};
const unsigned char disable_at_echo[7] = {"ATE0\r\n\0"};
const unsigned char client_mode[14] = {"AT+CWMODE=1\r\n\0"};
const unsigned char disconnect_from_wifi[11] = {"AT+CWQAP\r\n\0"};
const unsigned char connect_to_wifi[39] = {"AT+CWJAP=\"nombre-de-tu-red\",\"tu-password\"\r\n\0"};
const unsigned char set_ip[30] = {"AT+CIPSTA=\"192.168.100.182\"\r\n\0"};
const unsigned char enable_multiple_connections[14] = {"AT+CIPMUX=1\r\n\0"};
const unsigned char be_a_server[20] = {"AT+CIPSERVER=1,80\r\n\0"};
const unsigned char send_package[18] = {"AT+CIPSEND=0,76\r\n\0"};
unsigned char json_response[77] = {
    "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"data\":{\"temp\": \"25.0\"}}\0"
};
const unsigned char close_connection[16] = {"AT+CIPCLOSE=5\r\n\0"};
const unsigned char success[11] = {"Success!\r\n\0"};

unsigned int i = 0;
unsigned char connection_id = 0;
unsigned char wait_for_request = 0;

unsigned int adc_value = 0;
float temperature = 0.0;
unsigned char temperature_ascii[] = {0,0,0,0};



void send_string(unsigned char *string) {
    while(*string != '\0') {
        while (!(IFG2&UCA0TXIFG));
        UCA0TXBUF = *string;

        string++;
    }
}

unsigned int string_length(unsigned char *str) {
    unsigned int str_length = 0;

    while(*str != '\0') {
        str++;
        str_length++;
    }

    return str_length;
}

unsigned char compare_strings(unsigned char *str1, unsigned char *str2) {
    unsigned int str1_length = 0;
    unsigned int str2_length = 0;

    str1_length = string_length(str1);
    str2_length = string_length(str2);

    if(str1_length != str2_length) return 0;

    while(str2_length != 0) {
        str2_length--;

        if(*str1 == *str2) {
            str1++;
            str2++;
        } else {
            return 0;
        }
    }
    return 1;
}

void send_at_command(unsigned char expected_byte_num, unsigned char *command) {
    unsigned char dummy_rx = UCA0RXBUF;         // Dummy read to clean the buffer
    rx_counter = expected_byte_num;

    if(expected_byte_num != 0) {
        send_string(command);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
    } else {
        __disable_interrupt();
        send_string(command);
    }
}

unsigned char configure_esp8266(void) {
    __delay_cycles(2000000);        //  Wait until ESP8266 has started

    send_at_command(0, reset);
    __delay_cycles(2000000);

    send_at_command(0, disable_at_echo);
    __delay_cycles(1000000);

    send_at_command(0, client_mode);
    __delay_cycles(100000);

    send_at_command(0, disconnect_from_wifi);
    __delay_cycles(500000);

    send_at_command(35, connect_to_wifi);
    if(!compare_strings(rx_buffer_pointer, "WIFI CONNECTED\r\nWIFI GOT IP\r\n\r\nOK\r\n")) {
        return 0;
    }
    __delay_cycles(100000);

    send_at_command(6, set_ip);
    if(!compare_strings(rx_buffer_pointer, "\r\nOK\r\n")) return 0;
    __delay_cycles(100000);

    send_at_command(6, enable_multiple_connections);
    if(!compare_strings(rx_buffer_pointer, "\r\nOK\r\n")) return 0;
    __delay_cycles(100000);

    send_at_command(6, be_a_server);
    if(!compare_strings(rx_buffer_pointer, "\r\nOK\r\n")) return 0;
    __delay_cycles(100000);

    return 1;
}

void listen_requests(void) {
    rx_counter = 40;
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
    __disable_interrupt();
    __delay_cycles(1000000);
}

unsigned char send_http_response(void){
    send_at_command(8, send_package);
    if(!compare_strings(rx_buffer_pointer, "\r\nOK\r\n> ")) return 0;
    __delay_cycles(100000);

    send_at_command(28, json_response);
    if(!compare_strings(rx_buffer_pointer, "\r\nRecv 76 bytes\r\n\r\nSEND OK\r\n")) return 0;
    __delay_cycles(100000);

    send_at_command(16, close_connection);
    if(!compare_strings(rx_buffer_pointer, "0,CLOSED\r\n\r\nOK\r\n")) return 0;
    __delay_cycles(100000);

    return 1;
}

void float_to_string(float num, unsigned char* array)
{
    unsigned int int_part = (int)num;
    unsigned int float_part = (num - int_part) * 10;

    array[0] = ((int_part % 100) / 10) + 48;
    array[1] = (int_part % 10) + 48;
    array[2] = '.';
    array[3] = (float_part % 10) + 48;
}

void insert_temperature_to_json(void) {
    json_response[69] = temperature_ascii[0];
    json_response[70] = temperature_ascii[1];
    json_response[71] = temperature_ascii[2];
    json_response[72] = temperature_ascii[3];
}

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;


    P1DIR |= 0x01;                            // P1.0 output
    P3SEL = 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD

    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 8;                            // 1MHz 115200
    UCA0BR1 = 0;                              // 1MHz 115200
    UCA0MCTL = UCBRS_6;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

    ADC10CTL0 = ADC10SHT_2 + ADC10ON;         // ADC10ON
    ADC10CTL1 = ADC10SSEL_3 | INCH_4;                    // SMCK
    ADC10AE0 |= 0x01;                         // P2.0 ADC option select

    configure_esp8266();

    while(1) {
        listen_requests();

        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        while(!(ADC10CTL0&ADC10IFG));           // Wait until conversion ends
        adc_value = ADC10MEM;                   // Save binary value
        adc_value = ADC10MEM;                   // Save binary value
        temperature = (adc_value * 40.0) / 1023;

        float_to_string(temperature, temperature_ascii);

        insert_temperature_to_json();

        send_http_response();
    }
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    rx_counter--;
    *rx_buffer_pointer = UCA0RXBUF;
    rx_buffer_pointer++;

    if(rx_counter == 0) {
        // Add \0 at the end of the response to differ between responses
        *rx_buffer_pointer = 0x00;

        rx_buffer_pointer = &rx_buffer[0];
        __disable_interrupt();
        __bic_SR_register_on_exit(LPM0_bits);
    }
}
