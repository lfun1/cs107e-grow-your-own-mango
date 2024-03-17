#ifndef DASHBOARD_H
#define DASHBOARD_H
/* 
 * Module to display dashboard with sensor data and graphs.
 *
 * Author: Lisa Fung
 */

#include "gl.h"

/*
 * `dashboard_init` : Required initialization for dashboard screen
 *
 * Initialize the dashboard. The dashboard begins empty with panes.
 *
 * @param nrows  the requested number of rows in panes
 * @param ncols  the requested number of columns in panes
 * @param foreground  foreground color used for text
 * @param background  background color
 *
 * A subsequent call to dashboard_init after the first does a reinitialization.
 */
void dashboard_init(int nrows, int ncols, color_t foreground, color_t background);

/*
 * `dashboard_clear`
 *
 * Clear all dashboard data and graphs to only display panes.
 */
void dashboard_clear(void);

void dashboard_draw_outline(void);

void dashboard_show(void);

void change_data(void);

void print_all_graphs(void);

#endif
