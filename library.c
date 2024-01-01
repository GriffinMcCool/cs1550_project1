//author Griffin McCool
#include "graphics.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include "linux/fb.h"
#include "termios.h"
#include "sys/mman.h"
#include "time.h"

static color_t* fb;
static int fbfd;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;

void init_graphics(){
    	//open, ioctl, mmap
    	fbfd = open("/dev/fb0", O_RDWR);
    	//get screen info
    	ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    	ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
    	//map memory
    	fb = (color_t*)mmap(NULL, vinfo.yres_virtual * finfo.line_length,
    	PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);
    	struct termios t;
    	//turn of keypress echo and buffering the keypress
    	ioctl(0, TCGETS, &t);
    	t.c_lflag &= ~(ICANON | ECHO);
    	ioctl(0, TCSETS, &t);
    	return;
}

void exit_graphics(){
    	//ioctl
    	struct termios t;
    	//turn keypress and buffering back on
    	ioctl(0, TCGETS, &t);
    	t.c_lflag |= (ICANON | ECHO);
    	ioctl(0, TCSETS, &t);
    	munmap(fb, vinfo.yres_virtual * finfo.line_length);
    	close(fbfd);
    	return;
}

char getkey() {
    	//select, read
    	char buffer;
	//this code is mostly from help from TA
    	while(1){
    		buffer = '\0';
    		fd_set set;
    		FD_ZERO(&set);
    		FD_SET(STDIN_FILENO, &set);
    		struct timeval timeout = {0, 0};
    		if (select(1, &set, NULL, NULL, &timeout) == 1){
    	    		read(STDIN_FILENO, &buffer, sizeof(buffer));
    	    		return buffer;
    		}
    	}
}

void sleep_ms(long ms) {
    	//nanosleep
    	if (ms < 0) return;
    	struct timespec time = {0, ms * 1000000};
    	int x = nanosleep(&time, NULL);
    	return;
}

void clear_screen(void *img){
    	if (img == NULL) return;
    	color_t color = RGB(0,0,0);
    	color_t* img2 = (color_t*)img;
    	int y, x;
    	//loop through img and change every pixel
    	for (y = 0; y < vinfo.yres_virtual; y++){
    		for (x = 0; x < vinfo.xres_virtual; x++){
    	    		img2[(x + y * vinfo.xres_virtual)] = color;
    		}
    	}
    	return;
}

void draw_pixel(void *img, int x, int y, color_t color){
	if (img == NULL) return;
    	// if pixel is out of bounds, do not draw
    	// NOTE: im assuming that draw_line and draw_circle can accept
    	// arguments that are out of bounds, but those will not be drawn.
    	// Thus, if the center of the circle is out of bounds, the edges
    	// may still appear on screen.
    	if ((x < 0) || (y < 0) || (x > vinfo.xres_virtual - 1) || (y >
    	vinfo.yres_virtual - 1)) return;
    	color_t* img2 = (color_t*)img;
    	int coordinate = (x + y * vinfo.xres_virtual);
    	img2[coordinate] = color;
    	return;
}

//helper function for draw_line()
static void dll(void *img, int x1, int y1, int x2, int y2, color_t c){
    	int dx = x2 - x1;
    	int dy = y2 - y1;
    	int yi = 1;
    	if (dy < 0){
		yi = -1;
		dy = -dy;
    	}
    	int D = (2 * dy) - dx;
    	int y = y1;
    	int x;
    	for (x = x1; x < x2; x++){
    		draw_pixel(img, x, y, c);
    		if (D > 0){
    	    		y = y + yi;
    	    		D = D + (2 * (dy - dx));
    		} else {
    	    		D = D + 2 * dy;
    		}
    	}
}

//helper function for draw_line
static void dlh(void *img, int x1, int y1, int x2, int y2, color_t c){
    	int dx = x2 - x1;
    	int dy = y2 - y1;
    	int xi = 1;
    	if (dx < 0){
    		xi = -1;
    		dx = -dx;
    	}
    	int D = (2 * dx) - dy;
    	int x = x1;
    	int y;
    	for (y = y1; y < y2; y++){
    		draw_pixel(img, x, y, c);
    		if (D > 0){
    	    		x = x + xi;
    	    		D = D + (2 * (dx - dy));
    		} else {
    	    		D = D + 2*dx;
    		}
    	}
}

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c){
    	if (img == NULL) return;
    	//algorithm from:
    	//https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
    	int dy = y2 - y1;
    	int dx = x2 - x1;
    	if ((dy * ((dy > 0) - (dy < 0))) < (dx * ((dx > 0) - (dx < 0)))){
    		if (x1 > x2){
	    		dll(img, x2, y2, x1, y1, c);
    		} else {
    	    		dll(img, x1, y1, x2, y2, c);
    		}
    	} else {
    		if (y1 > y2){
    	    		dlh(img, x2, y2, x1, y1, c);
    		} else {
    	    		dlh(img, x1, y1, x2, y2, c);
    		}
    	}
    	return;
}

void draw_circle(void *img, int x, int y, int r, color_t c){
    	if(img == NULL || r <= 0) return;
    	//algorithm from:
    	//www.geeksforgeeks.org/mid-point-cirlce-drawing-algorithm/
    	int x2 = r, y2 = 0;
    	draw_pixel(img, x2 + x, y2 + y, c);
    	if (r > 0) {
    		draw_pixel(img, x2 + x, -y2 + y, c);
    		draw_pixel(img, y2 + x, x2 + y, c);
    		draw_pixel(img, -x2 + x, y, c);
    		draw_pixel(img, x, -x2 + y, c);
    	}

    	int P = 1 - r;
    	while (x2 > y2){
    		y2++;

    		if (P <= 0) P = P + 2 * y2 + 1;
    		else {
    	    		x2--;
    	    		P = P + 2 * y2 - 2 * x2 + 1;
    		}

    		if (x2 < y2) break;

    		draw_pixel(img, x2 + x, y2 + y, c);
    		draw_pixel(img, -x2 + x, y2 + y, c);
    		draw_pixel(img, x2 + x, -y2 + y, c);
    		draw_pixel(img, -x2 + x, -y2 + y, c);

    		if (x2 != y){
    	    		draw_pixel(img, y2 + x, x2 + y, c);
    	    		draw_pixel(img, -y2 + x, x2 + y, c);
    	    		draw_pixel(img, y2 + x, -x2 + y, c);
    	    		draw_pixel(img, -y2 + x, -x2 + y, c);
    		}
    	}
    	return;
}

void *new_offscreen_buffer() {
    	//mmap
    	void* m = mmap(NULL, vinfo.yres_virtual * finfo.line_length,
    	PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,(off_t)0);
    	return m;
}

void blit(void *src){
    	if (src == NULL) return;
    	color_t* src2 = (color_t*)src;
    	int y, x;
    	//loop through src and copy every pixel to fb
    	for (y = 0; y < vinfo.yres_virtual; y++){
    		for(x = 0; x < vinfo.xres_virtual; x++){
    		fb[(x + y * vinfo.xres_virtual)] = src2[(x + y *
    		vinfo.xres_virtual)];
    		}
    	}
    	return;
}
