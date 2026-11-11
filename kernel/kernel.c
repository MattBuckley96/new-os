#include "kernel_image.h"
#include "kernel_font.h"

typedef _Bool bool;
#define true 1
#define false 0

typedef struct {
    int width;
    int height;
    void* memory;
} VideoBuffer;

// NOTE: needed for clang
void* memset(void* address, int value, unsigned int size)
{
    unsigned char* memory = (unsigned char*)address;
    
    for (unsigned int i = 0; i < size; i++)
    {
        memory[i] = (unsigned char)value;
    }
    
    return address;
}

static unsigned char inb(unsigned short port)
{
    unsigned char result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static void outb(unsigned short port, unsigned char value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void ata_wait_bsy_rdy(void)
{
    while (inb(0x1F7) & 0x80); // wait for BSY
    while (!(inb(0x1F7) & 0x40)); // wait for RDY
}

static void read_sectors(unsigned int address, void* buffer, unsigned int count)
{
    // select the drive
    ata_wait_bsy_rdy();
    outb(0x1F6, 0xE0 | ((address >> 24) & 0x0F));
    
    ata_wait_bsy_rdy();
    outb(0x1F2, count);
    outb(0x1F3, (unsigned char)address);
    outb(0x1F4, (unsigned char)(address >> 8));
    outb(0x1F5, (unsigned char)(address >> 16));
    outb(0x1F7, 0x20);
    
    unsigned short* out = (unsigned short*)buffer;
    
    for (int i = 0; i < count; i++)
    {
        // wait for DRQ to set
        while (!(inb(0x1F7) & 0x08));
        
        int wordsToCopy = 256;
        asm volatile("rep insw" : "+D"(out), "+c"(wordsToCopy) : "d"(0x1F0) : "memory");
        
        out += wordsToCopy;
    }
}

static void render_glyph(VideoBuffer* framebuffer, char c,
                         int x, int y, int scale, unsigned int color)
{
    if (c == ',')
    {
        y += 4*scale;
    }
    
    if (c == 'Q')
    {
        y++;
    }
    
    int glyphWidth = GLYPH_WIDTH;
    int glyphHeight = GLYPH_HEIGHT;
    int scaledGlyphWidth = glyphWidth*scale;
    int scaledGlyphHeight = glyphHeight*scale;
    
    unsigned int* pixels = (unsigned int*)framebuffer->memory;
    for (int row = 0; row < scaledGlyphHeight; row++)
    {
        int glyphY = row / scale;
        unsigned char byte = GLYPH_DATA[c][glyphY];
        
        for (int bit = 0; bit < scaledGlyphWidth; bit++)
        {
            int glpyhX = bit / scale;
            int bitPos = (glyphWidth - (glpyhX));
            bool bitIsOne = (byte >> bitPos) & 1;
            
            if (bitIsOne)
            {
                int pixelIndex = (bit + x) + ((row + y)*framebuffer->width);
                pixels[pixelIndex] = color;
            }
        }
    }
}

static void render_text(VideoBuffer* framebuffer, const char* text, 
                        int x, int y, int scale, unsigned int color)
{
    int xoffset = 0;
    
    while (*text)
    {
        
        render_glyph(framebuffer, *text++, x, y, scale, color);
        x += 8*scale;
    }
}

static void render_rect(VideoBuffer* buffer, int x, int y, 
                        int width, int height, unsigned int color)
{
    unsigned int* pixels = (unsigned int*)buffer->memory;
    
    for (int rectY = 0; rectY < height; rectY++)
    {
        for (int rectX = 0; rectX < width; rectX++)
        {
            int pixelIndex = (x + rectX) + ((y + rectY)*buffer->width);
            pixels[pixelIndex] = color;
        }
    }
}

static void render_image(VideoBuffer* buffer, VideoBuffer* image, 
                         int x, int y, int scale)
{
    int scaledImageWidth = image->width*scale;
    int scaledImageHeight = image->height*scale;
    unsigned int* videoBufferPixels = (unsigned int*)buffer->memory;
    unsigned char* imagePixel = (unsigned char*)image->memory;
    
    for (int scaledY = 0; scaledY < scaledImageHeight; scaledY++)
    {
        for (int scaledX = 0; scaledX < scaledImageWidth; scaledX++)
        {
            int videoIndex = (x + scaledX) + ((y + scaledY)*buffer->width);
            
            int imageX = (scaledX*image->width) / scaledImageWidth;
            int imageY = (scaledY*image->height) / scaledImageHeight;
            int imageIndex = imageX + (imageY*image->width);
            imagePixel = image->memory + (imageIndex*4);
            
            unsigned char r = *imagePixel++;
            unsigned char g = *imagePixel++;
            unsigned char b = *imagePixel++;
            unsigned char a = *imagePixel++;
            
            if (a == 0)
            {
                continue;
            }
            
            unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;
            videoBufferPixels[videoIndex] = color;
        }
    }
    
}

static void render_weird_gradient(VideoBuffer* buffer, 
                                  int blueOffset, int greenOffset)
{
    unsigned int* pixels = (unsigned int*)buffer->memory;
    for (int y = 0; y < buffer->height; y++)
    {
        for (int x = 0; x < buffer->width; x++)
        {
            int pixelIndex = x + (y*buffer->width);
            
            unsigned char b = (unsigned char)x + blueOffset;
            unsigned char g = (unsigned char)y + greenOffset;
            
            unsigned int color = 0xFF000000 | (g << 8) | b;
            pixels[pixelIndex] = color;
        }
    }
    
}

void kernel_main(VideoBuffer frontbuffer)
{
    unsigned int* frontbufferPixels = (unsigned int*)frontbuffer.memory;
    
    VideoBuffer backbuffer = {0};
    backbuffer.width = frontbuffer.width;
    backbuffer.height = frontbuffer.height;
    backbuffer.memory = (void*)0x200000;
    unsigned int* backbufferPixels = (unsigned int*)backbuffer.memory;
    
    VideoBuffer image = {0};
    image.width = IMAGE_WIDTH;
    image.height = IMAGE_HEIGHT;
    image.memory = (void*)IMAGE_DATA;
    
    int xoffset = 0;
    int yoffset = 0;
    
    while (true)
    {
        render_weird_gradient(&backbuffer, xoffset, yoffset);
        render_rect(&backbuffer, 0, 0, 800, 400, 0x00111111);
        render_image(&backbuffer, &image, 128, 64, 8);
        render_text(&backbuffer, "THAT IS SO MUCH EASIER TO READ", 0, 0, 3, 0x00FFFF00);
        
        xoffset++;
        
        for (int i = 0; i < frontbuffer.width*frontbuffer.height; i++)
        {
            frontbufferPixels[i] = backbufferPixels[i];
        }
    }
}
