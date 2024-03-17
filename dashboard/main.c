/* File: main.c
 * -------------
 * Sample main program.
 */
#include "printf.h"
#include "uart.h"
#include "dashboard.h"
#include "timer.h"

extern unsigned long config_float(void);

static void pause(const char *message) {
    if (message) printf("\n%s\n", message);
    printf("[PAUSED] type any key in minicom/terminal to continue: ");
    int ch = uart_getchar();
    uart_putchar(ch);
    uart_putchar('\n');
}

void main(void) {
    config_float();
    uart_init();
    uart_putstring("Hello, world!\n");
    printf("I am printf, here m%c %s!\n", 'e', "ROAR");

    // Test dashboard
    dashboard_init(2,3,GL_SILVER, GL_MOSS);
    data_graph_init();
    dashboard_draw_outline();
    dashboard_show(70, 70, 50, 20);
    pause("Go to next");
    
    float d_temp, d_hum, d_soil_mois, d_wind_speed;
    d_temp = 90.0;
    d_hum = 60.0;
    d_soil_mois = 10.0;
    d_wind_speed = 10.0;

    while (1) {
        dashboard_draw_outline();
        dashboard_show(d_temp, d_hum, d_soil_mois, d_wind_speed);
        pause("Go to next");
    }
}
