//author Griffin McCool

#include "library.c"

int main(){
	init_graphics();

	//create new offscreen buffer
	void* buf = new_offscreen_buffer();
	//draw a bunch of stuff
	color_t col = RGB(31, 0, 0);
	draw_pixel(buf, 0, 0, col);
	draw_circle(buf, 340, 240, 50, col);
	col = RGB(0, 31, 0);
	draw_line(buf, 50, 50, 200, 200, col);
	col = RGB(0, 31, 31);
	draw_circle(buf, 0, 0, 100, col);
	col = RGB(31, 63, 31);
	draw_line(buf, 0, 0, 639, 479, col);
	col = RGB(0, 0, 31);
	draw_pixel(buf, 350, 0, col);
	draw_circle(buf, 639, 0, 200, col);
	col = RGB(0, 60, 31);
	draw_circle(buf, 690, 499, 150, col);
	draw_circle(buf, 200, 400, 250, col);
	draw_circle(buf, 100, 100, -10, col);
	draw_circle(buf, 100, 100, 500, col);
	col = RGB (31, 31, 0);
	draw_line(buf, 0, 490, 700, 0, col);
	draw_line(buf, 100, 50, 100, 300, col);
	draw_line(buf, 0, 250, 681, 250, col);
	blit(buf);
	//sleeps for about 3 seconds, then clears the screen
	sleep_ms(999);
	sleep_ms(999);
	sleep_ms(999);
	clear_screen(buf);
	blit(buf);
	//get user input and change color of printed cirlces
	//based on input. default (not '1', '2', or '3' is white)
	char h = getkey();
	switch(h){
	case '1':
		col = RGB(31, 63, 0);
		break;
	case '2':
		col = RGB(0, 63, 31);
		break;
	case '3':
		col = RGB(31, 0, 31);
		break;
	default:
		col = RGB(31, 63, 31);
	}
	int i, j;
	for(i = 0; i < 10; i++){
		for (j = 0; j < 10; j++){
			draw_circle(buf, 64 * i, 48 * j, 100, col);
			blit(buf);
		}
	}
	sleep_ms(999);
	sleep_ms(999);
	sleep_ms(999);
	//wait about 3 seconds then clear screen
	clear_screen(buf);
	blit(buf);
	exit_graphics();
	return 0;
}
