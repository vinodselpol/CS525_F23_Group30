#include "dberror.h"
#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


/* Manipulating the page files */

//Declaring file pointer
FILE *filepointer; 

/* Initializing the file pointer with NULL*/
void initStorageManager(void){
    filepointer = NULL;   
    SM_PageHandle pghndl = NULL;
}
//Create Page File
RC createPageFile (char *fileName){
    FILE *fil_p = fopen(fileName,"w+");
	
    if(fil_p == NULL){
        return RC_FILE_NOT_FOUND;
    }
    else{
        // This page is for zeroes
		char *zeroPage = (char *)calloc(PAGE_SIZE, sizeof(char));
		// This page is for writng value 1
		char *pageWithValueOne = (char *)malloc(PAGE_SIZE * sizeof(char));
        fputs("1",fil_p);
        //This writes value 1 in the second page
        fwrite(zeroPage, PAGE_SIZE, 1, fil_p);
		free(zeroPage);
		fwrite(pageWithValueOne, PAGE_SIZE, 1, fil_p);
		free(pageWithValueOne);
        return RC_OK;
    }
	fclose(fil_p);
}

//Open page file
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    FILE *fil_p = fopen(fileName, "r+");
    
    if (fil_p == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    // Initialize fileHandle attributes
    fHandle->fileName = strdup(fileName);
    fHandle->totalNumPages = 0;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fil_p;

    // Read header page to get the total number of pages
    char pageCounter[PAGE_SIZE];
    if (fgets(pageCounter, PAGE_SIZE, fil_p) != NULL) {
        fHandle->totalNumPages = atoi(pageCounter);
        return RC_OK;
    } else {
        fclose(fil_p);
        free(fHandle->fileName);
        return RC_FILE_NOT_FOUND; // Handle both file not found and read errors
    }
}



//Close the page file
RC closePageFile (SM_FileHandle *fHandle)
{
	//Attempt to close the file handle
	RC result = fclose(fHandle->mgmtInfo);// closing the file
    return result == 0? RC_OK : RC_FILE_NOT_FOUND; 
}

//Destroy the page file
RC destroyPageFile (char *fileName)
{
	int  destroyedFile = remove(fileName);
    return destroyedFile==0? RC_OK: RC_FILE_NOT_FOUND;
}


/* reading blocks from disc */

//Read Block
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Check if pageNum is valid
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    FILE *file = fHandle->mgmtInfo;

    // Calculate the byte offset to the beginning of the page
    long offset = (long)(pageNum + 1) * PAGE_SIZE;

    // Seek to the beginning of the page
    if (fseek(file, offset, SEEK_SET) == 0) {
        // Read the page into memPage
        size_t bytesRead = fread(memPage, 1, PAGE_SIZE, file);

        if (bytesRead == PAGE_SIZE) {
            // Update the current page position
            fHandle->curPagePos = pageNum;
            return RC_OK;
        }
    }

    return RC_READ_NON_EXISTING_PAGE;
}

//Get Block Position
int getBlockPos (SM_FileHandle *fHandle)
{
	FILE * pt;
	return fHandle->curPagePos;
}



//Read first block


RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Call readBlock with page number 0 (the first block)
    return readBlock(0, fHandle, memPage);
}


//Read previous block

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Calculate the page number of the previous block (currentPage - 1)
    int currentPage = getBlockPos(fHandle);
    int previousPage = currentPage - 1;

    if (previousPage >= 0) {
        // Read the previous block using readBlock
        return readBlock(previousPage, fHandle, memPage);
    } else {
        return RC_READ_NON_EXISTING_PAGE;
    }
}


//Read current block
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Get the current position of the read block
    int blockPos = getBlockPos(fHandle);

    if (blockPos >= 0 && blockPos < fHandle->totalNumPages) {
        // Read the current block using readBlock
        return readBlock(blockPos, fHandle, memPage);
    } else {
        return RC_READ_NON_EXISTING_PAGE;
    }
}


//Read next block
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Get the position of the next block (currentPage + 1)
    int nextBlockPos = getBlockPos(fHandle) + 1;

    if (nextBlockPos < fHandle->totalNumPages) {
        // Read the next block using readBlock
        return readBlock(nextBlockPos, fHandle, memPage);
    } else {
        return RC_READ_NON_EXISTING_PAGE;
    }
}



//Read last block
// Function to read the last block in a file
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Calculate the position of the last block
    int lastBlockPos = fHandle->totalNumPages - 1;

    // Check if the calculated position is valid (non-negative)
    if (lastBlockPos >= 0) {
        // Call the readBlock function to read the last block
        return readBlock(lastBlockPos, fHandle, memPage);
    } else {
        // If the calculated position is not valid, return an error
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* writing blocks to a page file */

//Write Block to a page file
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    int numPages = fHandle->totalNumPages;

    // Check if pageNum is within the valid range (0 to numPages-1)
    if (pageNum >= 0 && pageNum < numPages) {
        // Calculate the byte offset to the specified page
        long offset = (long)(pageNum + 1) * PAGE_SIZE;

        // Seek to the beginning of the specified page
        FILE *file = fHandle->mgmtInfo;
        if (fseek(file, offset, SEEK_SET) == 0) {
            // Write the block (memPage) to the file
            size_t bytesWritten = fwrite(memPage, sizeof(char), PAGE_SIZE, file);

            if (bytesWritten == PAGE_SIZE) {
                // Update the current page position
                fHandle->curPagePos = pageNum;
                return RC_OK;
            }
        }
    }

    // If pageNum is out of range or there's a write error, return an error code
    return RC_WRITE_FAILED;
}





//write current block
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    // Get the current position of the block
    int currentPosition = getBlockPos(fHandle);

    // Write the current block
    RC writeResult = writeBlock(currentPosition, fHandle, memPage);

    // Check if the write operation was successful
    if (writeResult == RC_OK) {
        return RC_OK;
    } else {
        return RC_WRITE_FAILED;
    }
}


//append empty block
RC appendEmptyBlock(SM_FileHandle *fHandle) {
    // Allocate memory to count pages and an empty page
    int noPage;
    char *emptyPage = (char *)calloc(PAGE_SIZE, sizeof(char));

    // Get the current number of pages
    int numPages = fHandle->totalNumPages;

    // Seek to the end of the file to append an empty page
    fseek(fHandle->mgmtInfo, (numPages + 1) * PAGE_SIZE, SEEK_SET);

    // Write the empty page to the file
    if (!(fwrite(emptyPage, PAGE_SIZE, 1, fHandle->mgmtInfo))) {
        free(emptyPage);
        return RC_WRITE_FAILED;
    } else {
        // Update the total number of pages in fHandle
        fHandle->totalNumPages += 1;

        // Update the current page position
        fHandle->curPagePos = fHandle->totalNumPages - 1;

        // Seek to the start of the headerPage and update the page count
        fseek(fHandle->mgmtInfo, 0L, SEEK_SET);
        fprintf(fHandle->mgmtInfo, "%d", fHandle->totalNumPages);

        // Position the pointer back to the previous position
        fseek(fHandle->mgmtInfo, (fHandle->totalNumPages + 1) * PAGE_SIZE, 0);

        // Deallocate the memory
        free(emptyPage);

        return RC_OK;
    }
}


//ensure capacity
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    int currentNumPages = fHandle->totalNumPages;
    FILE *filePointer;
    filePointer = fopen(fHandle->fileName, "a");

    if (filePointer == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    if (currentNumPages >= numberOfPages) {
        // If the current capacity is greater or equal to the required capacity, no action needed.
        fclose(filePointer);
        return RC_OK;
    } else {
        int numOfPagesToAdd = numberOfPages - currentNumPages;
        int i;

        // Append empty blocks to reach the required capacity
        for (i = 0; i < numOfPagesToAdd; i++) {
            RC appendResult = appendEmptyBlock(fHandle);

            if (appendResult != RC_OK) {
                fclose(filePointer);
                return appendResult;
            }
        }

        fclose(filePointer);
        return RC_OK;
    }
}





