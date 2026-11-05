#include "image.h"

#define true 1
#define false 0

void kernel_main(unsigned int videoAddress,
                 int videoWidth,
                 int videoHeight)
{
    unsigned int* videoMemory = (unsigned int*)videoAddress;
    unsigned int* framebuffer = (unsigned int*)0x200000;
    
    int xoffset = 0;
    int yoffset = 0;
    
    while (true)
    {
        for (int y = 0; y < videoHeight; y++)
        {
            for (int x = 0; x < videoWidth; x++)
            {
                int pixelIndex = x + (y*videoWidth);
                
                unsigned char b = (unsigned char)x + xoffset;
                unsigned char g = (unsigned char)y + yoffset;
                
                unsigned int color = (g << 8) | b;
                framebuffer[pixelIndex] = color;
            }
        }
        
        xoffset++;
        
        // render image 
        unsigned char* pixel = IMAGE_DATA;
        int imageWidth = IMAGE_WIDTH;
        int imageHeight = IMAGE_HEIGHT;
        int imageScale = 12;
        int scaledImageWidth = IMAGE_WIDTH*imageScale;
        int scaledImageHeight = IMAGE_HEIGHT*imageScale;
        
        for (int y = 0; y < scaledImageHeight; y++)
        {
            for (int x = 0; x < scaledImageWidth; x++)
            {
                int videoIndex = x + (y*videoWidth);
                
                int imageX = (x*imageWidth) / scaledImageWidth;
                int imageY = (y*imageHeight) / scaledImageHeight;
                int imageIndex = imageX + (imageY*imageWidth);
                pixel = IMAGE_DATA + (imageIndex*4);
                
                unsigned char r = *pixel++;
                unsigned char g = *pixel++;
                unsigned char b = *pixel++;
                unsigned char a = *pixel++;
                
                unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;
                framebuffer[videoIndex] = color;
            }
        }
        
        for (int i = 0; i < videoWidth*videoHeight; i++)
        {
            videoMemory[i] = framebuffer[i];
        }
    }
}
