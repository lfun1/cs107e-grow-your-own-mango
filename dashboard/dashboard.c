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
#include "printf.h"

#define DATA_SIZE 4
#define LINE_LEN 26
#define TEMP_INDEX 0
#define SOIL_INDEX 1
#define HUMI_INDEX 2
#define PRES_INDEX 3
#define ADJUST 3
#define LABEL_SIZE 8
#define GRAPH_ARRAY_S 5


static struct {
    color_t bg_color, fg_color, line_color;
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

typedef struct {
    int x, y; // index of graph pane
    const char *title;
    float raw_data[GRAPH_ARRAY_S];
    int *proc_data[GRAPH_ARRAY_S];
    int ndata;
    color_t c_axes, c_points;
    char *x_y_label[2];
    float max_val;
    float min_val;
} graph_t;

typedef struct {
    int x, y;
    const char *title;
    float temp, soil_mois, humidty;
    int pressure;
    color_t c_contents;
} processed_data_t;

void data_strings_init(void);

static char *data_strings[4];

const static int LINE_SPACING = 5;

void dashboard_init(int nrows, int ncols, color_t foreground, color_t background) {
    const static int FB_WIDTH = 1280;
    const static int FB_HEIGHT = 720;

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.fg_color = foreground;
    module.bg_color = background;
    module.line_color = GL_PURPLE;

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



static graph_t temp;
static graph_t hum;
static graph_t soil_mois;
static graph_t wind_speed;
static processed_data_t data_today;
static processed_data_t data_yes;


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
    char buf[LINE_LEN];

    for (int i = 0; i < DATA_SIZE; i++) {
        // Join the values with the strings
        buf[0] = '\0';
        size_t line_len = 0;
        int num_dec = 0;
        switch (i) {
            case TEMP_INDEX:
                num_dec = (int)this_data.temp;
                snprintf(buf, LINE_LEN, "%d.%02d F", (int)this_data.temp, (int)((this_data.temp - num_dec) * 100));
                break;
            case SOIL_INDEX:
                num_dec = (int)this_data.soil_mois;
                snprintf(buf, LINE_LEN, "%d.%02d %%", (int)this_data.soil_mois, (int)((this_data.soil_mois - num_dec) * 100));
                break;
            case HUMI_INDEX:
                num_dec = (int)this_data.humidty;
                snprintf(buf, LINE_LEN, "%d.%02d %%", (int)this_data.humidty, (int)((this_data.humidty - num_dec) * 100));
                break;
            case PRES_INDEX:
                num_dec = (int)this_data.pressure;
                snprintf(buf, LINE_LEN, "%d Pa", this_data.pressure);
                break;
        }
        line_len = strlen(data_strings[i]);
        strlcat(data_strings[i], buf, LINE_LEN);
        cur_y += ch_height + LINE_SPACING;
        gl_draw_string(x_start, cur_y, data_strings[i], this_data.c_contents);
        data_strings[i][line_len] = '\0';
    }
}

static void graph_init(graph_t *graph, int x, int y) {
    // Initialize the axis.
    graph->x = x; graph->y = y;
    // Get space for labels.
    (*graph).x_y_label[0] = malloc(LABEL_SIZE); (*graph).x_y_label[0][0] = '\0';
    (*graph).x_y_label[1] = malloc(LABEL_SIZE); (*graph).x_y_label[1][0] = '\0';

    // Get space for raw data.
    // (*graph).raw_data = malloc(sizeof(float) * GRAPH_ARRAY_S);

    // Get space for the processed data.
    for (int i = 0; i < GRAPH_ARRAY_S; i++) {
        (*graph).proc_data[i] = malloc(sizeof(int) * 3);
    }
}

static void put_labels(graph_t *graph, const char *_x, const char *_y, const char *title) {
    graph->title = title;
    strlcat((*graph).x_y_label[0], _x, LABEL_SIZE);
    strlcat((*graph).x_y_label[1], _y, LABEL_SIZE);
}

static int get_pane_x_min(int x) {
    return module.panes_x[x];
}

static int get_pane_y_min(int y) {
    return module.panes_y[y];
}

static int get_pane_x_max(int x) {
    return get_pane_x_min(x) + module.pane_width ;
}

static int get_pane_y_max(int x) {
    return get_pane_y_min(x) + module.pane_height;
}

static void draw_axis_units(graph_t graph) {
    int char_width = gl_get_char_width();
    int x_min = get_pane_x_min(graph.x) + 3 * char_width; // Draw after a space of 2 characters.
    int x_max = get_pane_x_max(graph.x);
    int y_min = get_pane_y_min(graph.y) + module.line_height; // Draw after 1 line(s).
    int y_max = get_pane_y_max(graph.y) - module.line_height; // Leave one space after drawing.

    gl_draw_line(x_min, y_min, x_min, y_max, graph.c_axes);
    gl_draw_line(x_min, y_max, x_max, y_max, graph.c_axes);

    int vert_x_label = get_pane_x_min(graph.x);
    int vert_y_label = (y_min + y_max) / 2;
    int hor_x_label = (x_max + x_min) / 2;
    int hor_y_label = get_pane_y_max(graph.y) - module.line_height + LINE_SPACING;

    gl_draw_string(vert_x_label, vert_y_label, graph.x_y_label[0], graph.c_points);
    gl_draw_string(hor_x_label, hor_y_label, graph.x_y_label[1], graph.c_points);
    gl_draw_string(vert_x_label, y_min + module.line_height, "max", GL_MAGENTA);
    gl_draw_string(vert_x_label, y_max, "min", GL_MAGENTA);
    
    if (strcmp(wind_speed.title, graph.title) == 0) {
        gl_draw_string(vert_x_label, y_min, "40", GL_MAGENTA);
        gl_draw_string(vert_x_label, y_max - module.line_height + LINE_SPACING, "0", GL_MAGENTA);
        
    } else if (strcmp(hum.title, graph.title) == 0 || strcmp(temp.title, graph.title) == 0) {
        gl_draw_string(vert_x_label, y_min, "100", GL_MAGENTA);
        gl_draw_string(vert_x_label, y_max - module.line_height + LINE_SPACING, "40", GL_MAGENTA);
    } else {
        gl_draw_string(vert_x_label, y_min, "100", GL_MAGENTA);
        gl_draw_string(vert_x_label, y_max - module.line_height + LINE_SPACING, "0", GL_MAGENTA);
    }
}

static void get_plotting_points(graph_t *graph) {
    int char_width = gl_get_char_width();
    int x_min = get_pane_x_min(graph->x) + 3 * char_width;
    int x_max = get_pane_x_max(graph->x);
    int y_min = get_pane_y_min(graph->y) + module.line_height;
    int y_max = get_pane_y_max(graph->y) - module.line_height;

    float data_min = graph->raw_data[0]; float data_max = graph->raw_data[0];

    // Get the max and min of the data values.
    for (int i = 1; i < GRAPH_ARRAY_S; i++) {
        if (graph->raw_data[i] < data_min) data_min = graph->raw_data[i];
        if (graph->raw_data[i] > data_max) data_max = graph->raw_data[i];
    }

    int data_gap = data_max - data_min;
    int hor_gap = (x_max - x_min) / GRAPH_ARRAY_S;
    int graph_height = (y_max - y_min) / 2;
    int cur_y = y_max;
    int cur_x = x_min;

    // The first data point always takes the middle part.
    // graph->proc_data[0][0] = cur_x;
    // graph->proc_data[0][1] = cur_y;

    // Average
    int average = (graph->max_val + graph->min_val) / 2;

    for (int j = 0; j < GRAPH_ARRAY_S; j++) {
        cur_y = y_max - (((graph->raw_data[j] - graph->min_val) / (graph->max_val - graph->min_val)) * (graph_height * 2));
        // if (data_gap <= 0) {
        //     if (data_max > average) {
        //         cur_y = (y_min + graph_height) - (data_max / (int)(graph->max_val - graph->min_val)) * graph_height;
        //     } else {
        //         cur_y = (y_min + graph_height) + (data_max / (int)(graph->max_val - graph->min_val)) * graph_height;
        //     }
        // } else {
        //     cur_y -= (int)(((graph->raw_data[j] - graph->raw_data[j - 1]) / data_gap) * graph_height);
        //     if (cur_y < y_min) {
        //         cur_y = y_min + module.line_height;
        //     } else if (cur_y > y_max) {
        //         cur_y = y_max - module.line_height;
        //     }
        // }
        printf("%s cur_y %d\n", graph->title, cur_y);
        // printf("y_max %d\n", y_max);
        graph->proc_data[j][0] = cur_x;
        graph->proc_data[j][1] = cur_y;
        cur_x += hor_gap;
    }
}

static void plot_points(graph_t *graph) {
    for (int i = 0; i < GRAPH_ARRAY_S - 1; i++) {
        gl_draw_line(graph->proc_data[i][0], graph->proc_data[i][1], graph->proc_data[i + 1][0], graph->proc_data[i + 1][1], module.line_color);
    }
}

static void dashboard_draw_graph(graph_t *graph) {
    dashboard_draw_title(graph->x, graph->y, graph->title, graph->c_axes);
    draw_axis_units(*graph);
}

static void add_another_value(graph_t *graph, float data) {
    // printf("Unprocessed before at index 0: %d and at index 4: %d\n", (int)(graph->raw_data[0]), (int)(graph->raw_data[1]));
    // int start = graph->raw_data[0]; // for chaning graphs
    for (int i = 0; i < GRAPH_ARRAY_S - 1; i++) {
        graph->raw_data[i] = graph->raw_data[i + 1];
    }
    graph->raw_data[GRAPH_ARRAY_S - 1] = data;
    // printf("Data value at position for %s _x: %d _y: %d\n", graph->title, graph->proc_data[0][0], graph->proc_data[0][1]);
    // printf("Unprocessed data at index 0: %d and at index 4: %d\n", (int)(graph->raw_data[0]), (int)(graph->raw_data[1]));
    // int *starter = (*graph).proc_data[0];
}

void data_graph_init(void) {
    graph_init(&temp, 1, 0);
    put_labels(&temp, "T/F", "t/min", "Temp vs Time");
    temp.raw_data[0] = 70; temp.raw_data[1] = 70; temp.raw_data[2] = 70;
    temp.raw_data[3] = 70; temp.raw_data[4] = 70;
    temp.c_axes = GL_BLACK, temp.c_points = GL_RED;
    temp.max_val = 100;
    temp.min_val = 40;

    graph_init(&hum, 2, 0);
    put_labels(&hum, "H/%", "t/day", "Hum vs Time(Day)");
    hum.raw_data[0] = 70; hum.raw_data[1] = 70; hum.raw_data[2] = 70;
    hum.raw_data[3] = 70; hum.raw_data[4] = 70;
    hum.c_axes = GL_BLACK, hum.c_points = GL_RED;
    hum.max_val = 100;
    hum.min_val = 40;

    
    graph_init(&soil_mois, 1, 1);
    put_labels(&soil_mois, "M/%", "t/day", "Soil Moisture vs Time(Day)");
    soil_mois.raw_data[0] = 50; soil_mois.raw_data[1] = 50; soil_mois.raw_data[2] = 50;
    soil_mois.raw_data[3] = 50; soil_mois.raw_data[4] = 50;
    soil_mois.c_axes = GL_BLACK, soil_mois.c_points = GL_RED;
    soil_mois.max_val = 100;
    soil_mois.min_val = 0;

    graph_init(&wind_speed, 2, 1);
    put_labels(&wind_speed, "m/s", "t/min", "Wind Speed vs Time");
    wind_speed.raw_data[0] = 20; wind_speed.raw_data[1] = 20; wind_speed.raw_data[2] = 20;
    wind_speed.raw_data[3] = 20; wind_speed.raw_data[4] = 20;
    wind_speed.c_axes = GL_BLACK, wind_speed.c_points = GL_RED;
    wind_speed.max_val = 40;
    wind_speed.min_val = 0;

    data_today.x = 0, data_today.y = 0;
    data_today.title = "Today";
    data_today.temp = 50.23, data_today.soil_mois = 45.20, data_today.pressure = 101325;
    data_today.humidty = 99.99;
    data_today.c_contents = GL_BLACK;

    data_yes.x = 0, data_yes.y = 1;
    data_yes.title = "Yesterday";
    data_yes.temp = 80.45, data_yes.soil_mois = 56.35, data_yes.pressure = 101000;
    data_yes.humidty = 75.25;
    data_yes.c_contents = GL_BLACK;
}

static void graph_run(graph_t *graph) {
    get_plotting_points(graph);
    plot_points(graph);
    dashboard_draw_graph(graph);
}

void dashboard_show(float d_temp, float d_hum, float d_soil_mois, float d_wind_speed) {
    add_another_value(&temp, d_temp); add_another_value(&soil_mois, d_soil_mois);
    add_another_value(&hum, d_hum); add_another_value(&wind_speed, d_wind_speed);
    
    graph_run(&temp); graph_run(&hum); graph_run(&soil_mois);  graph_run(&wind_speed);
    dashboard_draw_data(data_yes); dashboard_draw_data(data_today);
    gl_swap_buffer();
}

void print_all_graphs(void) {
    for (int i = 0; i < GRAPH_ARRAY_S; i++) {
        printf("index: %d , _x: %d , _y: %d ", i, temp.proc_data[i][0], temp.proc_data[i][1]);
    }
    printf("\n");
}