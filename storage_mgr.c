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



RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//if numPages is correct
	int numPages = fHandle->totalNumPages;
	if(!(pageNum > (numPages - 1) || pageNum < 0))
	{
		//seek to page number specified
		void * info = fHandle->mgmtInfo;
		fseek(info,(pageNum+1)*4096,SEEK_SET);
		//write the block
		fwrite(memPage,PAGE_SIZE,sizeof(char),info);
		int Pagepos = fHandle->curPagePos;
		//updating pages to the presen position
		Pagepos = pageNum;
		fHandle->curPagePos = pageNum;

		return RC_OK;
		
	}
	else
	{
		return RC_WRITE_FAILED;
	}

}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	RC currentPosition = getBlockPos(fHandle);
//current position of block
    RC writeResult = writeBlock(currentPosition, fHandle, memPage);
//write onto the currently pointed block
        return writeResult == 0 ? RC_OK : RC_WRITE_FAILED;
//tenary operator is used to check write status, returns RC_OK if block is present otherwise gives error
}

//ensureCapacity method is used to maintain capacity of the file and it should be equal to numberOfPages.
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
	int temp  = fHandle->totalNumPages;
	FILE *intpoint;
	intpoint = fopen(fHandle->fileName,"a");

	if(temp >= numberOfPages)
	{
		return RC_OK;
	}
	else	//if capacity i.e number of pages is not maintained
	{
		int i, numOfPagesToAdd;
		//find the number of pages to be added
		numOfPagesToAdd = numberOfPages - temp;

		int temp1 = numberOfPages;
		
//append empty blocks to get required capacity through iteration
		i = 0;

		while(i < numOfPagesToAdd)
		{
			//invoke appendEmptyBlock
			appendEmptyBlock(fHandle);
			i++;
		}
		return RC_OK;
	}

	if (intpoint == NULL){
		return RC_FILE_NOT_FOUND;
	}
	fclose(intpoint);
}

// appendEmptyBlock method is used to add empty block to pageFile

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
	int noPage; //allocate to count pages
	char * emptyPge;// allocates memory to emptypge
	emptyPge = (char*)calloc(4096, 1);
	int noOfPages = fHandle->totalNumPages;


	fseek(fHandle->mgmtInfo,(noOfPages+1)*PAGE_SIZE,SEEK_SET);
	void * info = fHandle->mgmtInfo;

	//seeking the position of pointer if success then write an empty page to file
	if(!(fwrite(emptyPge, PAGE_SIZE, 1, info)))
	{
		free(emptyPge);
		return RC_WRITE_FAILED;
	}
	else
	{
		//update the fHandle 
		fHandle->totalNumPages +=1;	
	//increment total number of pages
		int pageCt = fHandle->totalNumPages;
		fHandle->curPagePos = pageCt - 1;
	//update the present page position

		//seek to the start of the headerPage
		fseek(info,0L,SEEK_SET);
		fprintf(info, "%d", pageCt);

		//pointer is positioned back to the previous position 
		fseek(info,(pageCt+1)*4096,0);
		free(emptyPge);// delloactes the memory

		return RC_OK;
		
	}
}



