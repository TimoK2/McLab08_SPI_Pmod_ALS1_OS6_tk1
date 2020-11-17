/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
************************************************************************
*
* Test of the Pmod ambient light sensor
*
*************************************************************************
* Description: McLab08_SPI_Pmod_ALS1_OS6
* The ambient light value will be read and converted to LUX. 
*
* Material
* 1. ST NUCLEO L432KC , NXP FRDM-K64F  
* or some other micro controller board with SPI communication 
* 2. Digilent Pmod ALS ambient light sensor
* Please connect L432KC or FRDM-K64F - PmodALS with lines:
* L432KC D13 - ALS 4 SCK   hardware defined for the SPI
* L432KC D12 - ALS 3 MISO  hardware defined for the SPI
* L432KC D4  - ALS 1 CS  or any other free
*  GND     - ALS 5 GND
*  Vcc     - ALS 6 Vcc

* ALS data on the SPI
* D15 ... D12 - four zeros
* D11 ... D04 - 8 bits of ambient light data
* D03 ... D00 - four zeros
* 
* Details on the ADC IC on ALS board are given at
* http://www.ti.com/lit/ds/symlink/adc081s021.pdf
* The Pmod ALS 
* https://reference.digilentinc.com/reference/pmod/pmodals/start
* 
* Timo Karppinen 17.11.2020
**************************************************************/
#include "mbed.h"

DigitalOut LED(D1);  // LD1 to LD4 pin names are linked to D13 in L432KC.
                    // The board LD3 is the D13. Do not use. It is the SPISCK.
// PmodALS
SPI spi(D11, D12, D13); // mosi, miso, sck

DigitalOut alsCS(D4);        // chip select for sensor SPI communication

int alsScaledI = 0;         // 32 bit integer
int getALS();

int main()
{   
// SPI for the ALS        
        // Setup the spi for 8 bit data, high steady state clock,
        // second edge capture, with a 12MHz clock rate
        spi.format(8,0);           
        spi.frequency(12000000);
        // ready to wait the conversion start
        alsCS.write(1);

    while (true) {
     
        alsScaledI = getALS(); 
        printf("Ambient light scaled to LUX =  '%0d' \r\n",alsScaledI);
        
        if (alsScaledI > 100){ 
            LED.write(0);
            printf("Be aware of high UV radiation! \n\n");
            }
        else{
            LED.write(1);
            printf("Too low light for working \n\n");
            }
            
        ThisThread::sleep_for(3000ms);
    }
}

int getALS()
{
    char alsByte0 = 0; //8bit data from sensor board, char is the unsigned 8bit
    char alsByte1 = 0; // 8bit data from sensor board
    char alsByteSh0 = 0;
    char alsByteSh1 = 0;
    char als8bit = 0;
    unsigned short alsRaw = 0;   // unsigned 16 bit
    float alsScaledF = 0;       // 32 bit floating point
    
    // Begin the conversion process and serial data output
    alsCS.write(0); 
    // Reading two 8bit bytes by writing two dymmy 8bit bytes
    alsByte0 = spi.write(0x00);
    alsByte1 = spi.write(0x00);
    // End of serial data output and back to tracking mode
    alsCS.write(1);
    // Check the http://www.ti.com/lit/ds/symlink/adc081s021.pdf
    // shifting bits to get the number out
    alsByteSh0 = alsByte0 << 4;
    alsByteSh1 = alsByte1 >> 4;
    
    als8bit =( alsByteSh0 | alsByteSh1 );
    
    alsRaw = als8bit; 
    alsScaledF = (float(alsRaw))*(float(6.68)); 
    // The value 6.68 is 64 bit double precision floating point of type double.
    // Conversions to 32 bit floating point of type float.
    
    printf("Ambient light raw 8 bit 0...255 =  '%d' \r\n",alsRaw);
    //printf("Ambient light scaled to LUX =  '%0.1f' \r\n",alsScaledF);
    //Sorry, no more float printing in OS6 !
    return (int)alsScaledF; 
}

