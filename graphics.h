//author Griffin McCool

typedef unsigned short color_t;

#define RGB(x,y,z) ((x<<11) | (y << 5) | (z))

void init_graphics();

void exit_graphics();

char getkey();

void sleep_ms(long ms);

void clear_screen(void *img);

void draw_pixel(void *img, int x, int y, color_t color);

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c);

void draw_circle(void *img, int x, int y, int r, color_t c);

void *new_offscreen_buffer();

void blit(void *src);
