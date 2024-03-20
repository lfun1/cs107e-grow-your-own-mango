#ifndef DASHBOARD_H
#define DASHBOARD_H
/* 
 * Module to display dashboard with sensor data and graphs.
 *
 * Author: Elias Chikwanda, Lisa Fung
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

void dashboard_draw_outline(void);

void dashboard_show(float d_temp, float d_hum, float d_soil_mois, float d_wind_speed, float d_pressure);

void change_data(void);

void data_graph_init(void);

#endif
