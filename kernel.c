#define false 0
#define true 1

void kernel_main(void)
{
    int videoWidth = 320;
    int videoHeight = 200;
    unsigned char* videoMemory = (unsigned char*)0xA0000;
    
    unsigned char color = 0;
    while (true)
    {
        for (int i = 0; i < videoWidth * videoHeight; i++)
        {
            videoMemory[i] = color;
        }
        color++;
    }
}
