#include <FreeImage.h>
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char** argv)
{
    if (argc < 2) // No files provided
    {
        std::cerr << "Usage: Drag and drop .tiff files onto this .exe to convert them to .png and ensure they are in 8-bit/channel (24 bit)" << std::endl;
        return -1;
    }

    // Initialize FreeImage
    FreeImage_Initialise();

    // Process each file
    for (int i = 1; i < argc; ++i)
    {
        std::string inputFile = argv[i];
        std::string outputFile = std::filesystem::path(inputFile).replace_extension(".png").string();

        // Load the image
        FIBITMAP* image = FreeImage_Load(FIF_TIFF, inputFile.c_str());
        if (!image)
        {
            std::cerr << "Error: Could not load image " << inputFile << std::endl;
            continue; // Skip to the next file
        }

        // Convert to 8-bit/channel if necessary
        FIBITMAP* image24bit = FreeImage_ConvertTo24Bits(image);
        FreeImage_Unload(image); // Unload the original image

        if (!image24bit)
        {
            std::cerr << "Error: Could not convert " << inputFile << " to 8-bit /channel" << std::endl;
            continue;
        }

        // Save as PNG
        if (!FreeImage_Save(FIF_PNG, image24bit, outputFile.c_str()))
        {
            std::cerr << "Error: Could not save the output image " << outputFile << std::endl;
            FreeImage_Unload(image24bit);
            continue;
        }

        // Clean up
        FreeImage_Unload(image24bit);
        std::cout << "Conversion successful: " << outputFile << std::endl;
    }

    FreeImage_DeInitialise();
    return 0;
}
