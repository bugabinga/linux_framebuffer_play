/* used as stated by the nanosleep documentation. */
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <linux/fb.h>

void print_variable_screeninfo(struct fb_var_screeninfo *info);
void print_variable_screeninfo(struct fb_var_screeninfo *info)
{
	printf("visible resolution: [x=%u, y=%u]\n", info->xres, info->yres);
	printf("bits per pixel: %u\n", info->bits_per_pixel);
	printf("grayscale: %u\n", info->grayscale);
	printf("virtual resolution: [x=%u, y=%u]\n", info->xres_virtual, info->yres_virtual);
	printf("offset: [x=%u, y=%u]\n", info->xoffset, info->yoffset);
}

void print_fix_screeninfo(struct fb_fix_screeninfo *info);
void print_fix_screeninfo(struct fb_fix_screeninfo *info)
{
	printf("screen info id: %s\n", info->id);
	printf("line length: %u\n", info->line_length);
}

uint32_t pixel_color(uint8_t red, uint8_t green, uint8_t blue, struct fb_var_screeninfo *info);
uint32_t pixel_color(uint8_t red, uint8_t green, uint8_t blue, struct fb_var_screeninfo *info)
{
	return (red << info->red.offset) | (green << info->green.offset) | (blue << info->blue.offset);
}

/*
 * This programs depends on your user having the correct permissions to
 * "/dev/fb0". On Ubuntu 16.04 LTS and Fedora 26 this means you either have to be root, or part of the "video" group.
 * "sudo usermod -aG video ${USER}"
 * Re-login for changes to user to take effect.
 */

int main()
{
	int framebuffer_file_descriptor;
	long x,y;
	long screen_size;
	uint8_t* framebuffer;
	uint32_t pixel;

	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo variable_info;

	struct timespec spec;

	printf("START\n");

	framebuffer_file_descriptor = open("/dev/fb0", O_RDWR);

	if (framebuffer_file_descriptor < 0)
	{
		printf("ERROR opening the framebuffer device fb0. Error code = %i.\n", framebuffer_file_descriptor);
		exit(framebuffer_file_descriptor);
	}

	printf("framebuffer file descriptor: %i\n", framebuffer_file_descriptor);

	/*Get changeable screen information from the kernel and fill variable_info with it.*/
	ioctl(framebuffer_file_descriptor, FBIOGET_VSCREENINFO, &variable_info);

	/*Get immutable screen information from kernel into fix_info.*/
	ioctl(framebuffer_file_descriptor, FBIOGET_FSCREENINFO, &fix_info);

	print_variable_screeninfo(&variable_info);
	print_fix_screeninfo(&fix_info);

	if (variable_info.grayscale != 0)
	{
		variable_info.grayscale = 0;
	}

	if (variable_info.bits_per_pixel < 32)
	{
		variable_info.bits_per_pixel = 32;
	}

	/* setting the screeninfo we potentially set. hopefully these calls are implemented such that they are no-op, if the structs did not change. otherwise we have wasted some cycles here. the screeninfo is immediatly fetched again to verify the setings have been written. */
	ioctl(framebuffer_file_descriptor, FBIOPUT_VSCREENINFO, &variable_info);
	ioctl(framebuffer_file_descriptor, FBIOGET_VSCREENINFO, &variable_info);

	print_variable_screeninfo(&variable_info);

	screen_size = variable_info.yres_virtual * fix_info.line_length;

	framebuffer =  mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_file_descriptor, (off_t)0);

	pixel = pixel_color(0xAB, 0xFF, 0xCC, &variable_info);

	spec.tv_sec = 0;
	spec.tv_nsec = 0;

	for (x = 0; x < variable_info.xres; x++)
	{
		for (y = 0; y < variable_info.yres; y++)
		{
			/* variable_info.bits_per_pixel/8 seems to assume that bpp is 32 ?*/
			long location_of_pixel_in_memory = (x+variable_info.xoffset) * (variable_info.bits_per_pixel/8) + (y+variable_info.yoffset) * fix_info.line_length;
			*((uint32_t*) (framebuffer + location_of_pixel_in_memory)) = pixel;
		}
		nanosleep(&spec, NULL);
	}

	printf("END\n");
	return 0;
}
