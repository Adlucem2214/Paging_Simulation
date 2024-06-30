#include <stdio.h>
#include <stdlib.h>

#define VIRTUAL_MEMORY_PAGES 64
#define PHYSICAL_MEMORY_FRAMES 50
#define PAGE_SIZE 1024

typedef struct {
    int pageNumber;
    int frameNumber;
    int valid;
    int lastUsed;  
} PageTableEntry;

typedef struct {
    int frameNumber;
    int data[PAGE_SIZE];  
} Frame;

typedef struct {
    int pageNumber;
    int data[PAGE_SIZE];  
} VirtualPage;

PageTableEntry pageTable[VIRTUAL_MEMORY_PAGES];
Frame physicalMemory[PHYSICAL_MEMORY_FRAMES];
VirtualPage virtualMemory[VIRTUAL_MEMORY_PAGES];
int currentTime = 0;


void initializeVirtualMemory() {
    for (int i = 0; i < VIRTUAL_MEMORY_PAGES; i++) {
        virtualMemory[i].pageNumber = i;
        for (int j = 0; j < PAGE_SIZE; j++) {
            virtualMemory[i].data[j] = i * PAGE_SIZE + j;  // Fill with some data
        }
    }
}


void initializePageTable() {
    for (int i = 0; i < VIRTUAL_MEMORY_PAGES; i++) {
        pageTable[i].pageNumber = i;
        pageTable[i].frameNumber = -1;
        pageTable[i].valid = 0;
        pageTable[i].lastUsed = -1;
    }
}


void initializePhysicalMemory() {
    for (int i = 0; i < PHYSICAL_MEMORY_FRAMES; i++) {
        physicalMemory[i].frameNumber = i;
        for (int j = 0; j < PAGE_SIZE; j++) {
            physicalMemory[i].data[j] = -1; 
        }
    }
}


void printVirtualMemory() {
    printf("Virtual Memory:\n");
    printf("Page Number\tData (First 10 entries)\n");
    for (int i = 0; i < VIRTUAL_MEMORY_PAGES; i++) {
        printf("%d\t\t", virtualMemory[i].pageNumber);
        for (int j = 0; j < 10; j++) {
            printf("%d ", virtualMemory[i].data[j]);
        }
        printf("\n");
    }
}


void printPageTable() {
    printf("Page Table:\n");
    printf("Page Number\tFrame Number\tValid\tLast Used\n");
    for (int i = 0; i < VIRTUAL_MEMORY_PAGES; i++) {
        printf("%d\t\t%d\t\t%d\t%d\n", pageTable[i].pageNumber, pageTable[i].frameNumber, pageTable[i].valid, pageTable[i].lastUsed);
    }
}


void printPhysicalMemory() {
    printf("Physical Memory:\n");
    printf("Frame Number\tData (First 10 entries)\n");
    for (int i = 0; i < PHYSICAL_MEMORY_FRAMES; i++) {
        printf("%d\t\t", physicalMemory[i].frameNumber);
        for (int j = 0; j < 10; j++) {
            printf("%d ", physicalMemory[i].data[j]);
        }
        printf("\n");
    }
}


int findFreeFrame() {
    for (int i = 0; i < PHYSICAL_MEMORY_FRAMES; i++) {
        if (physicalMemory[i].data[0] == -1) {  // Check if frame is empty
            return i;
        }
    }
    return -1;
}


int findLRUPage() {
    int lruIndex = -1;
    int minLastUsed = currentTime + 1;
    for (int i = 0; i < VIRTUAL_MEMORY_PAGES; i++) {
        if (pageTable[i].valid && pageTable[i].lastUsed < minLastUsed) {
            minLastUsed = pageTable[i].lastUsed;
            lruIndex = i;
        }
    }
    return lruIndex;
}


int requestPage(int entryNumber) {
    currentTime++;
    
    if (entryNumber < 0 || entryNumber >= VIRTUAL_MEMORY_PAGES * PAGE_SIZE) {
        printf("Invalid entry number.\n");
        return -1;
    }

    int pageNumber = entryNumber / PAGE_SIZE;
    int offset = entryNumber % PAGE_SIZE;
    

    
    if (pageTable[pageNumber].valid) {
        pageTable[pageNumber].lastUsed = currentTime;
        int frame = pageTable[pageNumber].frameNumber;
        int physicalAddress = frame * PAGE_SIZE + offset;
        printf("Entry %d (Page %d, Offset %d) is already in frame %d.\n", entryNumber, pageNumber, offset, frame);
        return physicalAddress;
    }

    
    int freeFrame = findFreeFrame();
    if (freeFrame == -1) {
        int lruPage = findLRUPage();
        if (lruPage == -1) {
            printf("Error: No LRU page found, but no free frame either.\n");
            return -1;
        }
        freeFrame = pageTable[lruPage].frameNumber;

        printf("Replacing page %d in frame %d with page %d.\n", lruPage, freeFrame, pageNumber);

       
        pageTable[lruPage].frameNumber = -1;
        pageTable[lruPage].valid = 0;
        pageTable[lruPage].lastUsed = -1;
    } else {
        printf("Loading page %d into free frame %d.\n", pageNumber, freeFrame);
    }

   
    pageTable[pageNumber].frameNumber = freeFrame;
    pageTable[pageNumber].valid = 1;
    pageTable[pageNumber].lastUsed = currentTime;
    for (int i = 0; i < PAGE_SIZE; i++) {
        physicalMemory[freeFrame].data[i] = virtualMemory[pageNumber].data[i];
    }

    int physicalAddress = freeFrame * PAGE_SIZE + offset;
    return physicalAddress;
}

int main() {
    initializeVirtualMemory();
    initializePageTable();
    initializePhysicalMemory();

    int entryNumber;

    while (1) {
        printf("Enter entry number (0 to %d), or -1 to exit: ", VIRTUAL_MEMORY_PAGES * PAGE_SIZE - 1);
        if (scanf("%d", &entryNumber) != 1 || entryNumber == -1) {
            break;
        }

        int physicalAddress = requestPage(entryNumber);
        if (physicalAddress != -1) {
            int pageNumber = entryNumber / PAGE_SIZE;
            int offset = entryNumber % PAGE_SIZE;
            printf("Entry Number: %d, Page: %d, Offset: %d, Physical Address: %d, Frame: %d\n", entryNumber, pageNumber, offset, physicalAddress, pageTable[pageNumber].frameNumber);
        }
        printPageTable();
        printPhysicalMemory();
        printVirtualMemory();
        printf("\n");
    }

    return 0;
}

