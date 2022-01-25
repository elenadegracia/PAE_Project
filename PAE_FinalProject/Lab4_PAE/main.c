#include "msp.h"

/**
 * Practica4_PAE
 */
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include"uart.h"
#include "motor.h"
#include "lib_PAE.h"
#include "sensor.h"


void init_timers(void)
{

    TIMER_A0->CTL = TIMER_A_CTL_ID__1 | TIMER_A_CTL_SSEL__SMCLK
            | TIMER_A_CTL_CLR | TIMER_A_CTL_MC__UP;
    TIMER_A0->CCR[0] = (1 << 15);     // 1 Hz
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; //Interrupciones activadas en CCR0

    //Divider = 1; CLK source is ACLK; clear the counter; MODE is up
    TIMER_A1->CTL = TIMER_A_CTL_ID__1 | TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_CLR
            | TIMER_A_CTL_MC__UP;
    TIMER_A1->CCR[0] = (1 << 15);     // 1 Hz
    TIMER_A1->CCTL[0] |= TIMER_A_CCTLN_CCIE; //Interrupciones activadas en CCR0

}

void find_wall() {

    uint8_t right, left, front;
    int dist;
    right = right_s();
    left = left_s();
    front = front_s();

    if (front > left) {
        // Llegeix sensor esquerra amb prioritat
        if (right > left) {
            // rota fins tenir una apret al davant
            turnLeft(300);
            dist = 255;
            while(left < dist) {
                dist = front_s();
            }

        } else { // cas paret a la dreta
            turnRight(300);
            dist = 255;
            while(right < dist) {
                dist = front_s();
            }
        }
    }
    // Avanca cap a la paret
    forward(800);
    dist = 255;
    while (dist > 8){
        dist = front_s();
    }
    turnRight(800);
    dist = 255;
    while (dist - 2 > 40 ) { // Situa paret a lesquerra i avanca
        dist = left_s();
    }
    forward(800);
}


void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    init_ucs_24MHz(); //configuracio UCS
    init_uart();
    init_timers();
    init_interrupciones();
    uint8_t right, left, front;
    int dist = 255;

    find_wall();
    while (true){

        right = right_s();
        left = left_s();
        front = front_s();

       if(front < 30) {         //cas front critic rota a la dreta
           turnRight(800);
       }else if(left > 40 ){    //Recorrem per lesquerra per tant mur esquerra es la prioritat

           if(right > 50 && front > 40) {
               turnSlightlyLeft(800);   //Girar nomes una mica a la esquerra
           } else {

               turnSlightlyRight(800);  //Girar una mica a la dreta
           }
        }else if(left < 39){   //

            turnSlightlyRight(800);     //Girar una mica a la dreta
        }else if(right < 10) {

               turnSlightlyLeft(800);   //Girar una mica a lesquerra

        }else{

            forward(800);
       }
    }
}
