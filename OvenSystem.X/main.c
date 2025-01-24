//CONFIG1
#pragma config FOSC = HS       // Oscillator Sele
#pragma config WDTE = OFF      // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF     // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF        // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF       // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF     // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF  // Clock Out Enable (CLKOUT function is enabled on the CLKOUT pin)
#pragma config IESO = OFF      // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)


 //CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)age on MCLR/VPP must be used for programming)

#define _XTAL_FREQ 19600000     // Clock Frequency 19.6 MHz
#include <xc.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>  

//Keypad ROW
#define R1 PORTDbits.RD4
#define R2 PORTDbits.RD5
#define R3 PORTDbits.RD6
#define R4 PORTDbits.RD7
//Keypad Columns
#define C1 LATDbits.LATD0
#define C2 LATDbits.LATD1
#define C3 LATDbits.LATD2



void lcd_init()
{
    __delay_ms(60);
    LATB = 0x23; // enable
    LATB = 0x03; //sent
    __delay_ms(20);
    LATB = 0x23; // enable
    LATB = 0x03; //sent
    __delay_ms(5);
    LATB = 0x23; //enable
    LATB = 0x03; //sent
    __delay_ms(5);
    LATB = 0x23; //enable
    LATB = 0x03; //sent
    __delay_ms(5);
}

void lcd_cmd(unsigned char cmd)
{
    LATB = 0x10 + ((cmd >> 4) & 0x0f); // enable + MSB of instruction
    LATB = ((cmd >> 4) & 0x0f); // sent
    __delay_ms(2);
    LATB = (0x10 + (cmd & 0x0f)); // enable + LSB
    LATB = (cmd & 0x0f); // sent
    __delay_ms(2);
}

void lcd_char(unsigned char c)
{
    LATB = 0x30 + ((c >> 4) & 0x0f); // enable + MSB of char
    LATB = 0x20 + ((c >> 4) & 0x0f); // sent
    __delay_ms(2);
    LATB = 0x30 + (c & 0x0f); // enable + LSB of char
    LATB = 0x20 + (c & 0x0f); //sent
    __delay_ms(2); 
}

void lcd_DisplayString(unsigned char *str){
    while(*str)
    {
        lcd_char(*str);
        str++;
    }
}

void display_float(float num){
    char str[16];
    sprintf(str, "%.2f", num);
    lcd_DisplayString(str);
}

void display_int(int num){
    char str[16];
    sprintf(str, "%d", num);
    lcd_DisplayString(str);
} 

void ADC_init(){
    
    ADCON0bits.ADON = 1; // Turn On ADC
    ADCON1bits.ADFM = 1; //Right justify
    ADCON1bits.ADCS = 011;
    ADRESH = 0; //Flush ADC
    ADRESL = 0;
}

float ADC_Convert(int chan)
{
    if(chan == 0){// select channel
      ADCON0bits.CHS = 00000 ;
    }
    else if(chan == 1){
      ADCON0bits.CHS = 00001  ;  
    }
    else if(chan == 2){
      ADCON0bits.CHS = 2  ;  
    } 
    __delay_ms(1);
    ADCON0bits.GO_nDONE = 1; // Initialize AD convertor 
    ADCON0bits.ADGO = 1; //start converting
    while(ADCON0bits.GO_nDONE);// wait for conversion to complete
    unsigned int result = (ADRESH>>8) + ADRESL; //Right Justified
    float voltage = (result * 5.0) / 1023; //Converting Voltage
    return voltage;
}

char keypad(void){
    C1 =1; C2 = 0; C3 = 0;
    __delay_us(10);
    if(R1==1){
        while(R1 == 1);
        return '1';
    }
    else if(R2==1){
        while(R2 == 1);
        return '4';
    }
    else if(R3==1){
        while(R3 == 1);
        return '7';
    }
    else if(R4==1){
        while(R4 == 1);
        return '*';
    }
    
    C1 =0; C2 = 1; C3 = 0;
    __delay_us(10);
    if(R1==1){
        while(R1 == 1);
        return '2';
    }
    else if(R2==1){
        while(R2 == 1);
        return '5';
    }
    else if(R3==1){
        while(R3 == 1);
        return '8';
    }
    else if(R4==1){
        while(R4 == 1);
        return '0';
    }
    
    C1 =0; C2 = 0; C3 = 1;
    __delay_us(10);
    if(R1==1){
        while(R1 == 1);
        return '3';
    }
    else if(R2==1){
        while(R2 == 1);
        return '6';
    }
    else if(R3==1){
        while(R3 == 1);
        return'9';
    }
    else if(R4==1){
        while(R4 == 1);
        return '#';
    }
    C1 =0; C2 = 0; C3 = 0;
    __delay_us(10);
    return ' ';
}

int getInteger(char num){
    return num - '0';
}

void main(void) {
    
    TRISD=0xF0; // MSB is input and LSB is output
    ANSELD = 0; //DIGITAL PINS
    TRISB=0x00; //set all pins in port B as outputs 
    TRISC=0x00; //set all pins in port C as outputs
    ANSELA = 0xff; //  pin 0,1,2 as analog input    
    TRISA = 0xff;//input
    
    lcd_init(); // initialise the display
    lcd_cmd(0x28); // set 2 lines, 4 bit interface, 5x8 dots
    lcd_cmd(0x0C); // set display on, no cursor displayed
    lcd_cmd(0x06); // set cursor to move right after each character
    lcd_cmd(0x01); // clear the display
    
    lcd_cmd(0x82);
    lcd_DisplayString("OVEN SYSTEM");
    lcd_cmd(0xC4);
    lcd_DisplayString("GROUP- H");
    __delay_ms(3000);
    lcd_cmd(0x01);
    
    ADC_init();
    
    char inputTemp[3] = {'0','0','0'};
    char inputTime[6] = {'0','0',':','0','0',0x00};
    
    int temp = 0;
    
    int hr = 0;
    int min = 0;
    
    bool tempBool = true;
    bool timeBool = false;
    bool start = false;
    while (1){
        // ****FSR implementation****
//        float mV = ADC_Convert(1);
//        float resistance = 5000-mV;
////        resistance = resistance*1000;
//        resistance = resistance/mV;
//        float conductance = 1000000;
//        conductance = conductance/resistance;
//        
//        lcd_cmd(0x80);
//        lcd_DisplayString("w");
//        if(conductance<=1000){
//            float force = conductance/80;
//            display_float(force*100);
//        }
//        else{
//            float force = conductance - 1000;
//            force = force/30;
//            display_float(force*100);
//        }

        
        if (tempBool){
            if(temp != 0){
                lcd_cmd(0x80);
                lcd_DisplayString("Inputed Temp:");
                lcd_cmd(0xC0);
                display_int(temp);
                __delay_ms(2000);
                tempBool = false;
                timeBool = true;
                lcd_cmd(0x01);
            }
            else{
                lcd_cmd(0x80);
                lcd_DisplayString("Enter Temp:");
                lcd_cmd(0xC0);
                lcd_DisplayString(inputTemp);
                char v = keypad();
                if( v =='#'){
                    lcd_cmd(0x01);
                    temp = getInteger(inputTemp[2]) + getInteger(inputTemp[1])*10 + getInteger(inputTemp[0])*100;
                }
                else if(v == '*'){
                    inputTemp[2] = inputTemp[1];
                    inputTemp[1] = inputTemp[0];
                    inputTemp[0] ='0';
                }
                else if( v != ' '){
                    inputTemp[0] = inputTemp[1];
                    inputTemp[1] = inputTemp[2];
                    inputTemp[2] = v;
                }
            }
        }
        else if(timeBool){
            if ((hr!=0) || (min!=0)){
                lcd_cmd(0x80);
                lcd_DisplayString("Inputed Time:");
                lcd_cmd(0xC0);
                display_int(hr);
                lcd_char(':');
                display_int(min);
                __delay_ms(2000);
                lcd_cmd(0x01);
                lcd_cmd(0x80);
                lcd_DisplayString("OVEN STARTING ");
                lcd_cmd(0xC0);
                lcd_DisplayString("NOW...");
                __delay_ms(2000);
                lcd_cmd(0x01);
                
                
                timeBool = false;
                start = true;
                
                PORTCbits.RC0 = 1; //start heater
            }
            else{
                lcd_cmd(0x80);
                lcd_DisplayString("Enter Time:");
                lcd_cmd(0xC0);
                lcd_DisplayString(inputTime);
                
                char key = keypad();
                if(key == '#'){
                    lcd_cmd(0x01);
                    
                    hr = getInteger(inputTime[1]) + getInteger(inputTime[0])*10;
                    min = getInteger(inputTime[4]) + getInteger(inputTime[3])*10;
                }
                else if(key =='*'){
                    inputTime[4] = inputTime[3];
                    inputTime[3] = inputTime[1];
                    inputTime[1] = inputTime[0];
                    inputTime[0] = '0';
                }
                else if(key !=' '){
                    inputTime[0] = inputTime[1];
                    inputTime[1] = inputTime[3];
                    inputTime[3] = inputTime[4];
                    inputTime[4] = key;
                }
            }
        }
        else if((!timeBool)&&(!tempBool)){
            if(start){
                int seconds = hr*3600 + min * 60;
                while(seconds > 0 ){
                    lcd_cmd(0x80);
                    lcd_DisplayString("TIME : ");
                    display_int(seconds/3600);
                    lcd_char(':');
                    display_int((seconds%3600)/60);
                    lcd_char(':');
                    display_int(seconds%60);
                    
                    lcd_cmd(0xC0);
                    float tempVoltage  = ADC_Convert(0);
                    float currentTemperature = tempVoltage*100;
                    lcd_DisplayString("TEMP:");
                    display_float(currentTemperature);
                    
                    float ldrVoltage  = ADC_Convert(1);
                    if(ldrVoltage>0.4){
                        LATCbits.LATC4 = 1;
                    }else{
                        LATCbits.LATC4 = 0;
                    }
                    
                    __delay_ms(990);
                    lcd_cmd(0x01);
                    seconds--;
                    
                    float inputTemperature = temp+5;
                    if(inputTemperature >= currentTemperature){
                        LATCbits.LATC0 = 1;//HEATER ON
                        LATCbits.LATC1 = 0;
                        
                        LATCbits.LATC2 = 0;// MOTOR OFF
                        LATCbits.LATC3 = 1;
                    }
                    else{
                        LATCbits.LATC0 = 0;//HEATER OFF
                        LATCbits.LATC1 = 1;
                        
                        LATCbits.LATC2 = 1;// MOTOR ON
                        LATCbits.LATC3 = 0;
                    }
                }
                LATCbits.LATC0 = 0;//HEATER OFF
                LATCbits.LATC1 = 1;
                LATCbits.LATC2 = 0;// MOTOR OFF
                LATCbits.LATC3 = 1;
                
                start = false;
            }
            lcd_cmd(0x80);
            lcd_DisplayString("TIMER FINISHED");
            lcd_cmd(0xC6);
            lcd_DisplayString("****");
            __delay_ms(5000);
            lcd_cmd(0x01);
            tempBool = true;
            temp = 0;
            hr = 0;
            min = 0;
            inputTemp[0] = '0';
            inputTemp[1] = '0';
            inputTemp[2] = '0';
            inputTime[4] = '0';
            inputTime[3] = '0';
            inputTime[1] = '0';
            inputTime[0] = '0';
        }     
    }
}