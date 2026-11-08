#include "image.h"

typedef _Bool bool;
#define true 1
#define false 0

// font
static unsigned char glpyhs[255][10] = {
    [' '] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    ['!'] = {
        0b00000000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00000000,
        0b00100000,
    },
    ['A'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    ['B'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
    },
    ['C'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000010,
        0b00111100,
    },
    ['D'] = {
        0b00000000,
        0b01111000,
        0b01000100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000100,
        0b01111000,
    },
    ['E'] = {
        0b00000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111110,
    },
    ['F'] = {
        0b00000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
    },
    ['G'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01000000,
        0b01001110,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    ['H'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    ['I'] = {
        0b00000000,
        0b01111100,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b01111100,
    },
    ['J'] = {
        0b00000000,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    ['K'] = {
        0b00000000,
        0b01000010,
        0b01000100,
        0b01001000,
        0b01010000,
        0b01100000,
        0b01010000,
        0b01001000,
        0b01000100,
        0b01000010,
    },
    ['L'] = {
        0b00000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111110,
    },
    ['M'] = {
        0b00000000,
        0b10000010,
        0b11000110,
        0b10101010,
        0b10010010,
        0b10010010,
        0b10000010,
        0b10000010,
        0b10000010,
        0b10000010,
    },
    ['N'] = {
        0b00000000,
        0b01000010,
        0b01100010,
        0b01100010,
        0b01010010,
        0b01010010,
        0b01001010,
        0b01001010,
        0b01000110,
        0b01000110,
    },
    ['O'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    ['P'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
    },
    ['R'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01001000,
        0b01000100,
        0b01000010,
        0b01000010,
    },
    ['Q'] = {
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01001010,
        0b01000100,
        0b00111010,
        0b00000010,
    },
    ['S'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01000000,
        0b00111100,
        0b00000010,
        0b00000010,
        0b01000010,
        0b00111100
    },
    ['T'] = {
        0b00000000,
        0b11111110,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
    },
    ['U'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    ['V'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00011000,
        0b00011000,
    },
    ['W'] = {
        0b00000000,
        0b10000010,
        0b10000010,
        0b10010010,
        0b10010010,
        0b10010010,
        0b10101010,
        0b01000100,
        0b01000100,
        0b01000100,
    },
    ['X'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b00100100,
        0b00100100,
        0b00011000,
        0b00100100,
        0b00100100,
        0b01000010,
        0b01000010,
    },
    ['Y'] = {
        0b00000000,
        0b10000010,
        0b10000010,
        0b01000100,
        0b01000100,
        0b00101000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
    },
    ['Z'] = {
        0b00000000,
        0b01111110,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000,
        0b01111110,
    },
    [','] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00110000,
        0b00110000,
        0b00010000,
        0b00100000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
};

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

void render_glyph(unsigned int* framebuffer, int framebufferWidth,
                  char c, int x, int y, int scale)
{
    if (c == ',')
    {
        y += 4*scale;
    }
    
    if (c == 'Q')
    {
        y++;
    }
    
    int glyphWidth = 8;
    int glyphHeight = 10;
    int scaledGlyphWidth = glyphWidth*scale;
    int scaledGlyphHeight = glyphHeight*scale;
    
    for (int row = 0; row < scaledGlyphHeight; row++)
    {
        int glyphY = row / scale;
        unsigned char byte = glpyhs[c][glyphY];
        
        for (int bit = 0; bit < scaledGlyphWidth; bit++)
        {
            int glpyhX = bit / scale;
            int bitPos = (glyphWidth - (glpyhX));
            bool bitIsOne = (byte >> bitPos) & 1;
            
            if (bitIsOne)
            {
                int pixelIndex = (bit + x) + ((row + y)*framebufferWidth);
                framebuffer[pixelIndex] = 0x00FFFFFF;
            }
        }
    }
}

void render_text(unsigned int* framebuffer, int framebufferWidth,
                 const char* text, int x, int y, int scale)
{
    int xoffset = 0;
    
    while (*text)
    {
        
        render_glyph(framebuffer, framebufferWidth, *text++, x, y, scale);
        x += 8*scale;
    }
}

void kernel_main(void* videoAddress,
                 int videoWidth,
                 int videoHeight)
{
    unsigned int* videoMemory = (unsigned int*)videoAddress;
    unsigned int* framebuffer = (unsigned int*)0x200000;
    
    int xoffset = 0;
    int yoffset = 0;
    
    while (true)
    {
        // render weird gradient
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
        unsigned char* imageData = (unsigned char*)IMAGE_DATA;
        unsigned char* pixel = imageData;
        int imageWidth = IMAGE_WIDTH;
        int imageHeight = IMAGE_HEIGHT;
        int imageScale = 8;
        int scaledImageWidth = imageWidth*imageScale;
        int scaledImageHeight = imageHeight*imageScale;
        
        for (int y = 0; y < scaledImageHeight; y++)
        {
            for (int x = 0; x < scaledImageWidth; x++)
            {
                int videoIndex = x + (y*videoWidth);
                
                int imageX = (x*imageWidth) / scaledImageWidth;
                int imageY = (y*imageHeight) / scaledImageHeight;
                int imageIndex = imageX + (imageY*imageWidth);
                pixel = imageData + (imageIndex*4);
                
                unsigned char r = *pixel++;
                unsigned char g = *pixel++;
                unsigned char b = *pixel++;
                unsigned char a = *pixel++;
                
                if (a == 0)
                {
                    continue;
                }
                
                unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;
                framebuffer[videoIndex] = color;
            }
        }
        
        render_text(framebuffer, videoWidth, "THAT IS SO MUCH EASIER TO READ", 0, 0, 3);
        
        for (int i = 0; i < videoWidth*videoHeight; i++)
        {
            videoMemory[i] = framebuffer[i];
        }
    }
}
