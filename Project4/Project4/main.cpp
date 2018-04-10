//
//  main.cpp
//  Project4
//
//  Created by Hendrik Oosenbrug on 2018-04-04.
//  Copyright © 2018 Hendrik Oosenbrug. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, const char * argv[])
{
    // Initialize the page table
    int pageTable[256] = {0};
    memset(pageTable, -1, sizeof(pageTable[0]) * 256);
    
    // Initialize the physical memory
    int8_t physicalMemory[256][256] = {-1};
    memset(physicalMemory, -1, sizeof(physicalMemory[0][0]) * 256 * 256);
    
    // Open the logical addresses file addresses.txt
    ifstream logicalAddressesInputFile;
    logicalAddressesInputFile.open("addresses.txt");
    
    if (!logicalAddressesInputFile)
    {
        cerr << "Unable to open the input file.";
        exit(1);
    }
    
    string testString;
    int counter = 0;
    // Read from file
    while (getline(logicalAddressesInputFile, testString))
    {
        int logicalAddress = stoi(testString);
        
        // Get page number and offset
        int offset = logicalAddress & 255;
        int page = logicalAddress & 65280;
        int pageNumber = page >> 8;
        
        int physicalMemoryPageNumber = -1;
        int outputValue = -1;
        // Consult page table
        if (pageTable[pageNumber] == -1)
        {
            // This means there is no entry in our page table
            
            int i = 0;
            bool isPageAvailable = true;
            
            // Find page in physical memory that hasn't been assigned
            for (; i < 256; ++i)
            {
                isPageAvailable = true;
                for (int j = 0; j < 256; ++j)
                {
                    if (physicalMemory[i][j] != -1)
                    {
                        isPageAvailable = false;
                    }
                }
                if (isPageAvailable)
                {
                    break;
                }
            }
            
            if (!isPageAvailable)
            {
                cerr << "No available page table found." << " " << counter << "\n";
                exit(1);
                // Need to swap out a page from physical memory
            }
            
            // i is now the physical memory page number
            physicalMemoryPageNumber = i;
            
            // Get the frames from disk of whole page
            ifstream binFile;
            binFile.open("BACKING_STORE.bin", ios::in | ios::binary);
            
            if (!binFile)
            {
                cerr << "Unable to open the input file.";
                exit(1);
            }
            
            // Copy page to physical memory
            char binFileValue;
            binFile.seekg(page);
            for (int i = 0; i < 256; ++i)
            {
                binFile.read(reinterpret_cast<char *>(&binFileValue), 1);
                physicalMemory[physicalMemoryPageNumber][i] = binFileValue;
            }
            
            outputValue = physicalMemory[physicalMemoryPageNumber][offset];
            
            // Update page table
            pageTable[pageNumber] = physicalMemoryPageNumber;
        }
        else
        {
            // This means we do have a mapping
            physicalMemoryPageNumber = pageTable[pageNumber];
            
            outputValue = physicalMemory[physicalMemoryPageNumber][offset];
        }
        
        counter++;
        printf("Virtual address: %d ", logicalAddress);
        printf("Physical address: %d ", (physicalMemoryPageNumber * 256) + offset);
        printf("Value: %d\n", outputValue);
    }
    printf("Counter: %d\n", counter);
    return 0;
}
