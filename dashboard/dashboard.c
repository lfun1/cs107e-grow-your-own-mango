/* File: dashboard.c
 * ---------------
 * Implementation for dashboard module to display data and graphs.
 * Lisa Fung
 */

#include "gl.h"
#include "malloc.h"
#include "assert.h"
#include "printf.h"
#include "strings.h"

#define DATA_SIZE 4
#define LINE_LEN 40

static struct {
    color_t bg_color, fg_color;
    int line_height;
    int nrows, ncols;
    int edge_gap, interior_gap;
    int pane_width, pane_height;
    int *panes_x, *panes_y; // arrays of x,y-coordinates of upper left corner of each pane    
    //void *contents; // address of 2D char array for dashboard contents
} module = {
    .panes_x = NULL,
    .panes_y = NULL,
    //.contents = NULL,
};

void data_strings_init(void);

static char *data_strings[4];

const static int LINE_SPACING = 5;

void dashboard_init(int nrows, int ncols, color_t foreground, color_t background) {
    const static int FB_WIDTH = 1280;
    const static int FB_HEIGHT = 720;

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.fg_color = foreground;
    module.bg_color = background;

    // Deallocate previous contents memory
    //if (module.contents) free(module.contents);
    //module.contents = NULL;

    if (module.panes_x) free(module.panes_x);
    if (module.panes_y) free(module.panes_y);
    module.panes_x = NULL;
    module.panes_y = NULL;

    module.nrows = nrows;
    module.ncols = ncols;
    module.edge_gap = 10;
    module.interior_gap = 20;
    module.pane_width = ( FB_WIDTH - 2*module.edge_gap - (module.ncols-1)*module.interior_gap ) / module.ncols;
    module.pane_height = ( FB_HEIGHT - 2*module.edge_gap - (module.nrows-1)*module.interior_gap ) / module.nrows;

    // Initialize memory for storing top-left x,y coordinates of panes
    module.panes_x = malloc(module.ncols * sizeof(int));
    module.panes_y = malloc(module.nrows * sizeof(int));
    assert(module.panes_x != NULL);
    assert(module.panes_y != NULL);

    int corner_x = module.edge_gap;
    for (int x = 0; x < module.ncols; x++) {
        module.panes_x[x] = corner_x;
        corner_x += module.pane_width + module.interior_gap;
    }
    int corner_y = module.edge_gap;
    for (int y = 0; y < module.nrows; y++) {
        module.panes_y[y] = corner_y;
        corner_y += module.pane_height + module.interior_gap;
    }

    // Initialiaze the strings to show.
    for (int k = 0; k < DATA_SIZE; k++) {
        data_strings[k] = malloc(LINE_LEN);
        data_strings[k][0] = '\0'; // Initialize line with a null pointer for concatenating.
    }
    data_strings_init();
    
    // Initialize contents memory
    //int nchars = module.nrows * module.ncols;
    //module.contents = malloc(nchars * sizeof(char));
    //assert(module.contents != NULL);
    //console_clear();

    gl_init(FB_WIDTH, FB_HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(module.bg_color); // Clear framebuffer
    gl_swap_buffer(); // Show contents
}

void data_strings_init(void) {
    strlcat(data_strings[0], "Temperature: ", LINE_LEN);
    strlcat(data_strings[1], "Soil Moisture: ", LINE_LEN);
    strlcat(data_strings[2], "Humidity:  ", LINE_LEN);
    strlcat(data_strings[3], "Pressure: ", LINE_LEN);
}

void dashboard_draw_outline(void) {
    gl_clear(module.bg_color);
    for (int x = 0; x < module.ncols; x++) {
        for (int y = 0; y < module.nrows; y++) {
            gl_draw_rect(module.panes_x[x], module.panes_y[y], module.pane_width, module.pane_height, module.fg_color);
        }
    }
}

void dashboard_clear(void) {
}


/*
 * title is a string, clipped if too long
 * data is a 2D array with dimension 2 x npoints.
 *     the first npoints are x values
 *     the next npoints are corresponding y values
 * axes color, point color
 */

typedef struct {
    int x, y; // index of graph pane
    const char *title;
    void *data;
    int ndata;
    color_t c_axes, c_points;
} graph_t;

typedef struct {
    int x, y;
    const char *title;
    int temp, soil_mois, humidty, pressure;
    color_t c_contents;
} processed_data_t;

// Draw title, clip if too long
// x, y are the pane index
// Currently only supports one line for title
static void dashboard_draw_title(int x, int y, const char* title, color_t c) {
    int x_min = module.panes_x[x] + LINE_SPACING; // inclusive
    int x_max = x_min + module.pane_width; // exclusive
    int y_min = module.panes_y[y] + LINE_SPACING;

    int ch_width = gl_get_char_width();

    int i = x_min;

    while((i < x_max) && (*title != '\0')) {
        gl_draw_char(i, y_min, *title, c);
        i += ch_width;
        title++;
    }
}

static void dashboard_draw_data(processed_data_t this_data) {
    dashboard_draw_title(this_data.x, this_data.y, this_data.title, this_data.c_contents);
    int ch_height = gl_get_char_height();

    int x_start = module.panes_x[this_data.x] + LINE_SPACING + 2 * gl_get_char_width();
    int y_start = module.panes_y[this_data.y] + LINE_SPACING + 2 * ch_height;
    // int y_end = y_start module.pane_height;
    int cur_y = y_start;

    for (int i = 0; i < DATA_SIZE; i++) {
        gl_draw_string(x_start, cur_y, data_strings[i], this_data.c_contents);
        cur_y += ch_height + LINE_SPACING;
    }
}

static void dashboard_draw_graph(graph_t graph) {
    dashboard_draw_title(graph.x, graph.y, graph.title, graph.c_axes);
    
    for (int i = 0; i < 4; i++) {
        printf("%d\n", ((int *)graph.data)[i]);
    }
}

static void draw_data_test(void) {
    processed_data_t data_today;
    data_today.x = 0, data_today.y = 0;
    data_today.title = "Today";
    data_today.temp = 0, data_today.soil_mois = 0, data_today.pressure = 0;
    data_today.c_contents = GL_BLACK;

    dashboard_draw_data(data_today);

    processed_data_t data_yes;
    data_yes.x = 0, data_yes.y = 1;
    data_yes.title = "Yesterday";
    data_yes.temp = 0, data_yes.soil_mois = 0, data_yes.pressure = 0;
    data_yes.c_contents = GL_BLACK;

    dashboard_draw_data(data_yes);
}

static void dashboard_test(void) {
    graph_t temp;
    temp.x = 1, temp.y = 0;
    temp.title = "Temp vs Time";
    temp.ndata = 2;
    temp.data = malloc(2 * temp.ndata * sizeof(int));
    int (*data)[2] = temp.data;
    /* data[0] = {4, 20}; */
    /* data[1] = {6, 23}; */
    data[0][0] = 4;
    data[0][1] = 20;
    data[1][0] = 6;
    data[1][1] = 23;
    
    temp.c_axes = GL_BLACK, temp.c_points = GL_RED;

    dashboard_draw_graph(temp);
}


void dashboard_show(void) {
    dashboard_test();
    draw_data_test();
    gl_swap_buffer();
}
