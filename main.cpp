/*******************************************************************
 Project main function template for MicroZed based MZ_APO board
designed by Petr Porazil at PiKRON

change_me.c      - main file

include your name there and license for distribution.

Remove next text: This line should not appear in submitted
work and project name should be change to match real application.
If this text is there I want 10 points subtracted from final
evaluation.

*******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "font_types.h"

#include "DisplayDriver.hpp"
#include "Color.hpp"

unsigned short *fb;

void draw_pixel(int x, int y, unsigned short color) {
	if (x>=0 && x<480 && y>=0 && y<320) {
		fb[x+480*y] = color;
	}
}

void draw_char(int x, int y, font_descriptor_t* fdes, char ch) {
}

int char_width(font_descriptor_t* fdes, int ch) {
	int width = 0;
	if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
		ch -= fdes->firstchar;
		if (!fdes->width) {
		width = fdes->maxwidth;
		} else {
		width = fdes->width[ch];
		}
	}
	return width;
}

int main(int argc, char *argv[]) {
	unsigned char *mem_base;
	unsigned char *parlcd_mem_base;
	uint32_t val_line=5;
	int i,j,k;
	int ptr;

	printf("Hello world\n");

	sleep(1);

	/*
		* Setup memory mapping which provides access to the peripheral
		* registers region of RGB LEDs, knobs and line of yellow LEDs.
		*/
	mem_base = static_cast<uint8_t*>(map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0));

	/* If mapping fails exit with error code */
	if (mem_base == NULL)
		exit(1);

	struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
	for (i=0; i<30; i++) {
		*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
		val_line<<=1;
		printf("LED val 0x%x\n", val_line);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
	}


	DisplayDriver screen = DisplayDriver(DisplayOrientation::Landscape);

	for (int x = 0; x < 300; ++x) {
		screen.fill_screen(Color::Black);
		screen.draw_rectangle(x, x, 100, 50, Color::White);
		screen.flush();
		clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
	}

	return 0;
}
