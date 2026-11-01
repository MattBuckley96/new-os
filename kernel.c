#define true 1
#define false 0

void kernel_main(unsigned int videoAddress,
                 unsigned short videoWidth,
                 unsigned short videoHeight)
{
    unsigned int* videoMemory = (unsigned int*)videoAddress;
    
    int xoffset = 0;
    int yoffset = 0;
    while (true)
    {
        for (int y = 0; y < videoHeight; y++)
        {
            for (int x = 0; x < videoWidth; x++)
            {
                int pixelIndex = x + (y * videoWidth);
                
                unsigned char b = (unsigned char)x + xoffset;
                unsigned char g = (unsigned char)y;
                
                unsigned int pixel = (g << 8) | b;
                videoMemory[pixelIndex] = pixel;
            }
        }
        
        xoffset++;
    }
}
