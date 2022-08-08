//
//  time4io.c
//  
//
//  Created by Anuja  dadhich on 2021-02-23.
//

#include "time4io.h"
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void){
   
    int returnvalue=0;
    int switches =  0x00000f00;
    switches = switches & PORTD;
    switches = switches >> 8;
    returnvalue = switches;

    return returnvalue;
}

int getbtns (void){

    int returnvalue1 =0;
    int buttons =  0x000000e0;
    buttons = buttons & PORTD;
    buttons = buttons >> 5;
    returnvalue1 = buttons;
    
    return returnvalue1;
    
}
