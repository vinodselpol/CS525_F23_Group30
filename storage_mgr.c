#include "dberror.h"
#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//Declaring file pointer
FILE *filptr; 

/* Initializing the file pointer with NULL*/
void initStorageManager(void){
    filptr = NULL;   
    SM_PageHandle pghndl = NULL;
}

RC closePageFile (SM_FileHandle *fHandle)
{
	//if closing the file is success
	RC PageFileClosed = fclose(fHandle->mgmtInfo);// closing the file
    return PageFileClosed == 0? RC_OK : RC_FILE_NOT_FOUND; 
}

RC createPageFile (char *fileName){
    FILE *fil_p;
	
    fil_p = fopen(fileName,"w+");
    if(fil_p == NULL){
        return RC_FILE_NOT_FOUND;
    }
    else{
		char *newpage;
        // This page is for zeroes
		newpage = (char *)calloc(PAGE_SIZE, sizeof(char));
		char *twopage;
        // This page is for writng value 1
		twopage = (char*)malloc(PAGE_SIZE * sizeof(char));
        fputs("1",fil_p);
        //This writes value 1 in the second page
        fwrite(newpage, PAGE_SIZE, 1, fil_p);
		free(newpage);
		fwrite(twopage, PAGE_SIZE, 1, fil_p);
		free(twopage);
        return RC_OK;
    }
	fclose(fil_p);
}

int getBlockPos (SM_FileHandle *fHandle)
{
	FILE * pt;
	return fHandle->curPagePos;
}

RC destroyPageFile (char *fileName)
{
	int  desFile = remove(fileName);
    return desFile==0? RC_OK: RC_FILE_NOT_FOUND;
}

RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
    int fil_pt;
	FILE *fil_p;		//file pointer
    
	fil_p = fopen(fileName, "r+");	//open the pageFile
    fHandle->mgmtInfo;
	if(fil_p!=NULL)	//if file exists
	{
		/*update the fileHandle attributes*/
        
		fHandle->fileName = fileName;
        fHandle->fileName;	//store the file name

		/*read headerPage to get the Total Number of Pages*/
		char *header;
		char* pageCounter = (char*)calloc(PAGE_SIZE,sizeof(char));
        char* totalPage;
		fgets(pageCounter,PAGE_SIZE,fil_p);
		totalPage = pageCounter;
        fHandle->curPagePos;
		fHandle->totalNumPages = atoi(totalPage); //convert to integer
		fHandle->curPagePos = 0;	//store the current page position

		//store the File pointer information in the Management info of Page Handle
		fHandle->mgmtInfo = fil_p;
        fHandle->totalNumPages;

		free(pageCounter);	
        fHandle->mgmtInfo;	//free memory to avoid memory leaks

		return RC_OK;
	}
	else	//if file does not exists
	{
		return RC_FILE_NOT_FOUND;
	}
}



RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{

	//call to readBlock with page No. 0 i.e first block
	return 0 == readBlock(RC_OK,fHandle,memPage)?0:RC_READ_NON_EXISTING_PAGE;
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//call to readBlock with page No. (currentPage-1) i.e previous block
	int n = -2;
	RC posi = 1+getBlockPos(fHandle)+ n;
	if(0)
		return RC_OK;
	else
		return 0 == readBlock(posi,fHandle,memPage)?RC_OK:RC_READ_NON_EXISTING_PAGE;
}

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//check if PageNum is Valid
	void * ptr;
	int pageNu;
	ptr = fHandle->mgmtInfo;
	do{
		int ptr1;
		ptr1 = fHandle->totalNumPages;
		if(pageNum<0 || pageNum>ptr1 - 1)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
	}while(0);

	int seek = fseek(ptr,(pageNum+1)*4096,SEEK_SET);
	
	int ptr2 = fHandle->totalNumPages + fHandle->curPagePos;
	//if seeking the file ptr is successful then read the page into memPage
	if( seek == 0)
	{
		//read the block into memPage
		fread(memPage,1,PAGE_SIZE,ptr);
		void * info = fHandle->mgmtInfo;
		fHandle->curPagePos = pageNum;
		pageNu = pageNum;	//update the curr page pos to most recently read page
		return RC_OK;
	}
	else
		return RC_READ_NON_EXISTING_PAGE;
}

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//readCurrentBlock is used to get current position of read  block
	RC blockPos = getBlockPos(fHandle);//gets block position
	return 0 == readBlock(blockPos,fHandle,memPage)?RC_OK:RC_READ_NON_EXISTING_PAGE;
//If there are no pages in the pageFile, it returns an error.
}

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//call to readBlock with page No. (currentPage+1) i.e next block
	RC posi = getBlockPos(fHandle)+1; //to get position
	return 0 == readBlock(posi,fHandle,memPage)?RC_OK:RC_READ_NON_EXISTING_PAGE;
}

/*
 * This method is used to read the Last Block from the pageFile into memPage.
 * It returns an error if there are no pages in the page
 */
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	//call to readBlock with page as Last block
	RC posi = fHandle->totalNumPages - 1;
	return 0 == readBlock(posi,fHandle,memPage)?RC_OK:RC_READ_NON_EXISTING_PAGE;
}


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



