#include "FreeImage.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char** argv)
{
    bool hasErrored = false;
    if (argc < 2) // No files provided
    {
        std::cerr << "Usage: Drag and drop .tiff files onto this .exe to convert them to .png and ensure they are in 8-bit/channel (24 bit)" << std::endl;
        std::cout << "Press Enter to close...";
        std::cin.get();
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
            hasErrored = true;
            std::cerr << "Error: Could not load image " << inputFile << std::endl;
            continue; // Skip to the next file
        }

        // Check the bit depth and color type
        unsigned int bpp = FreeImage_GetBPP(image);
        FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(image);

        // Handle grayscale images
        if (colorType == FIC_MINISBLACK || colorType == FIC_MINISWHITE)
        {
            std::cerr << "Image is grayscale. Converting to 8-bit grayscale." << std::endl;
            FIBITMAP* tempImage = FreeImage_ConvertTo8Bits(image);
            FreeImage_Unload(image); // Unload the original image
            image = tempImage;
            if (!image)
            {
                hasErrored = true;
                std::cerr << "Error: Could not convert grayscale image " << inputFile << " to 8-bit." << std::endl;
                continue;
            }
        }

        // Convert to 24-bit RGB if necessary
        if (bpp != 24 || FreeImage_GetColorType(image) != FIC_RGB)
        {
            std::cerr << "Converting image to 24-bit RGB format." << std::endl;
            FIBITMAP* tempImage = FreeImage_ConvertTo24Bits(image);
            FreeImage_Unload(image);
            image = tempImage;
            if (!image)
            {
                hasErrored = true;
                std::cerr << "Error: Conversion to 24-bit RGB failed for " << inputFile << std::endl;
                continue;
            }
        }

        // Save as PNG
        if (!FreeImage_Save(FIF_PNG, image, outputFile.c_str()))
        {
            hasErrored = true;
            std::cerr << "Error: Could not save the output image " << outputFile << std::endl;
            FreeImage_Unload(image);
            continue;
        }

        // Clean up
        FreeImage_Unload(image);
        std::cout << "Conversion successful: " << outputFile << std::endl;
    }

    FreeImage_DeInitialise();

    if (hasErrored)
    {
        std::cout << "Some errors occurred during processing. Press Enter to close...";
    }
    else
    {
        std::cout << "All files processed successfully. Press Enter to close...";
    }
    std::cin.get();

    return 0;
}
