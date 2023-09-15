Group Members: 
1. Vinod Krishna Selpol - A20511584
2. Gagan Beerappa - A20514760
3. Pavan Kodihalli Jagadeesh - A20519967

How to run: 

1. Run 'make clean' in the assign1 directory to remove any previous object files. 
2. Run 'make'.
3. Run 'make run_test' to run test cases. 

Description:

There are 16 functions in storage manager.
Each function helps in doing a specific task associated with Read/write of blocks to and from a file.

1. initStorageManager:
    This function initializes file pointer and it sets the global file pointer to null and it makes it available to use in next blocks

2. createPageFile:
    This function creates a pagefile and put the null character to reserve the firstpage. It creates a new file and initializes the first two pages of the file. If the file is successfully created and initialized, it returns RC_OK. Otherwise, it returns RC_FILE_NOT_FOUND
    
3. openPageFile:
    This function is used to open an existing page file with the given fileName and initialize the fHandle structure. It sets various attributes of the file handle, such as the file name, total number of pages, and the current page position. If the file is successfully opened, it returns RC_OK. If the file is not found, it returns RC_FILE_NOT_FOUND

4. closePageFile:
    This function closes an existing pagefile. It throws an error if the given file argument is not found. It attempts to close the file associated with fHandle. If the file is successfully closed, it returns RC_OK, indicating success; otherwise, it returns RC_FILE_NOT_FOUND to indicate an error.


5. destroyPageFile:
    This function is used to delete (destroy) a page file with the given fileName.

6. readBlock:
    This function allows to read and writes the data from the page position to the memory pointed by pageHandle.

7. getBlockPos:
    This function reads data of currentblockPosition and returns the value.

8. readFirstBlock:
    By calling the already existing readBlock function this function updates the argument and runs the readblock function.

9. readCurrentBlock:
    This function reads the current block (page) specified by the current position in fHandle into the memPage buffer.

10. readNextBlock:
    This function reads the block immediately after the current block into the memPage buffer.

11. readLastBlock:
    This function reads the last block (page) of the file specified by fHandle into the memPage buffer.

12. readPreviousBlock:
    We can pass the argument to readBlock. The argument is current Page position subtracted by 1.

13. writeBlock:
    This function writes the contents of the memPage buffer to the specified block (page) with the given pageNum in the file specified by fHandle

14. write currentblockPosition:
    This function writes the contents of the memPage buffer to the current block (page) specified by the current position in fHandle.

15. appendEmptyBlock: 
    This function appends an empty block (page) to the end of the file specified by fHandle. It returns RC_OK if successful and RC_WRITE_FAILED if there is an error in appending the block.

15. ensureCapacity:
    This function ensures that the file specified by fHandle has a capacity of at least numberOfPages. If the file already has the required capacity or more, it returns RC_OK. If the capacity needs to be increased by adding empty pages, it does so and returns RC_OK. If there's an error in the process, it returns RC_FILE_NOT_FOUND
    