#include "data.hpp"

const int bytesPerPixel = 3; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

unsigned char *createBitmapFileHeader(unsigned int height, unsigned int width, int paddingSize);

unsigned char *createBitmapInfoHeader(unsigned int height, unsigned int width);

void generateBitmapImage(unsigned char *image, int height, int width, const char *const imageFileName) {

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (width * bytesPerPixel) % 4) % 4;

    unsigned char *fileHeader = createBitmapFileHeader(height, width, paddingSize);
    unsigned char *infoHeader = createBitmapInfoHeader(height, width);

    FILE *imageFile = fopen(imageFileName, "wb");

    fwrite(fileHeader, 1, fileHeaderSize, imageFile);
    fwrite(infoHeader, 1, infoHeaderSize, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i * width * bytesPerPixel), bytesPerPixel, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char *createBitmapFileHeader(unsigned int height, unsigned int width, int paddingSize) {
    unsigned int fileSize = fileHeaderSize + infoHeaderSize + (bytesPerPixel * width + paddingSize) * height;

    static unsigned char fileHeader[] = {
            0, 0, /// signature
            0, 0, 0, 0, /// image file size in bytes
            0, 0, 0, 0, /// reserved
            0, 0, 0, 0, /// start of pixel array
    };

    fileHeader[0] = (unsigned char) ('B');
    fileHeader[1] = (unsigned char) ('M');
    fileHeader[2] = (unsigned char) (fileSize);
    fileHeader[3] = (unsigned char) (fileSize >> 8);
    fileHeader[4] = (unsigned char) (fileSize >> 16);
    fileHeader[5] = (unsigned char) (fileSize >> 24);
    fileHeader[10] = (unsigned char) (fileHeaderSize + infoHeaderSize);

    return fileHeader;
}

unsigned char *createBitmapInfoHeader(unsigned int height, unsigned int width) {
    static unsigned char infoHeader[] = {
            0, 0, 0, 0, /// header size
            0, 0, 0, 0, /// image width
            0, 0, 0, 0, /// image height
            0, 0, /// number of color planes
            0, 0, /// bits per pixel
            0, 0, 0, 0, /// compression
            0, 0, 0, 0, /// image size
            0, 0, 0, 0, /// horizontal resolution
            0, 0, 0, 0, /// vertical resolution
            0, 0, 0, 0, /// colors in color table
            0, 0, 0, 0, /// important color count
    };

    infoHeader[0] = (unsigned char) (infoHeaderSize);
    infoHeader[4] = (unsigned char) (width);
    infoHeader[5] = (unsigned char) (width >> 8);
    infoHeader[6] = (unsigned char) (width >> 16);
    infoHeader[7] = (unsigned char) (width >> 24);
    infoHeader[8] = (unsigned char) (height);
    infoHeader[9] = (unsigned char) (height >> 8);
    infoHeader[10] = (unsigned char) (height >> 16);
    infoHeader[11] = (unsigned char) (height >> 24);
    infoHeader[12] = (unsigned char) (1);
    infoHeader[14] = (unsigned char) (bytesPerPixel * 8);

    return infoHeader;
}