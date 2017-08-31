#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

/*
* This programs depends on your user having the correct permissions to 
* "/dev/fb0". On Ubuntu 16.04 LTS this means you either have to be root, or part of the "video" group.
* "sudo usermod -aG video ${USER}"
* Re-login for changes to user to take effect.
*/

int main(){
	int framebuffer_file_descriptor;

	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo variable_info;

	printf("START\n");

	framebuffer_file_descriptor = open("/dev/fb0", O_RDWR);

	if(framebuffer_file_descriptor < 0 ){
		printf("ERROR opening the framebuffer device fb0. Error code = %i.\n", framebuffer_file_descriptor);
		exit(framebuffer_file_descriptor);
	}

	printf("framebuffer file descriptor: %i\n", framebuffer_file_descriptor);

	/*Get changeable screen information from the kernel and fill variable_info with it.*/
	ioctl(framebuffer_file_descriptor, FBIOGET_VSCREENINFO, &variable_info);

	/*Get immutable screen information from kernel into fix_info.*/
	ioctl(framebuffer_file_descriptor, FBIOGET_FSCREENINFO, &fix_info);

	printf("visible resolution: [x=%u, y=%u]\n", variable_info.xres, variable_info.yres);
	printf("bits per pixel: %u\n", variable_info.bits_per_pixel);
	printf("grayscale: %u\n", variable_info.grayscale);
	printf("virtual resolution: [x=%u, y=%u]\n", variable_info.xres_virtual, variable_info.yres_virtual);	
	printf("offset: [x=%u, y=%u]\n", variable_info.xoffset, variable_info.yoffset);

	printf("screen info id: %s\n", fix_info.id);
	printf("line length: %u\n", fix_info.line_length);

	printf("END\n");
	return 0;
}
