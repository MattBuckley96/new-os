void kernel_main(void)
{
    unsigned char* videoMemory = (unsigned char*)0xA0000;
    
    int videoWidth = 320;
    int videoHeight = 200;
    unsigned char color = 0;
    
    for (;;)
    {
        for (int i = 0; i < videoWidth * videoHeight; i++)
        {
            videoMemory[i] = color;
        }
        color++;
    }
}
