/* File: main.c
 * -------------
 * Sample main program.
 */
#include "printf.h"
#include "uart.h"
#include "dashboard.h"

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
    pause("Dashboard initialized");
    dashboard_draw_outline();
    dashboard_show();
    printf("Data before changae\n");
    print_all_graphs();
    dashboard_draw_outline();
    dashboard_show();
    pause("Showing dashboard outlines: Click to use change data values");
    printf("Data after changae\n");
    print_all_graphs();
    pause("Show next");
    // while (1) {
    //     change_data();
    //     pause("Go to next");
    // }
}
