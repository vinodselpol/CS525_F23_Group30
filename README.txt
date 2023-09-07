Group Members: 
1. Vinod Krishna Selpol - A20511584
2. Gagan Beerappa - A
3. Pavan Kodihalli Jagadeesh - A

How to run: 

1. Run 'make clean' in the assign1 directory to remove any previous object files. 
2. Run 'make'.
3. Run 'make run_test' to run test cases. 

Description:

There are 16 functions in storage manager.
Each function helps in doing a specific task associated with Read/write of blocks to and from a file.

1. initStorageManager:
    This function initializes the file pointer and makes it available to use in next blocks

2. createPageFile:
    This function created a pagefile and put the null character to reserve the firstpage.

3. openPageFile:
    It takes in file name as argument and opens the pagefile for further use.

4. closePageFile:
    This function closes an existing pagefile. It throws an error if the given file argument is not found.

5. destroyPageFile:
    It destroys the page file by using remove file function.

6. readBlock:
    This function allows to read and writes the data from the page position to the memory pointed by pageHandle.

7. getBlockPos:
    This function reads data of currentblockPosition and returns the value.

8. readFirstBlock:
    By calling the already existing readBlock function this function updates the argument and runs the readblock function.

9. readCurrentBlock:
    This block also follows the above readBlock and manipulates the argument to read the current block.
    The value of current block can be taken from the fHandle stucture curPagePos.

10. readNextBlock:
    We use the getBlockPos fucntion to read the current block poition and updates the argument by +1 and runs the readBlock function.

11. readLastBlock:
    We get the last block position by subtracting 1 with totalNumPage. We can pass that argument into readblock and read the last block.

12. readPreviousBlock:
    We can pass the argument to readBlock. The argument is current Page position subtracted by 1.

13. writeBlock:
    We write a page to disk using fwrite file function using the absolutePosition.

14. write currentblockPosition:
    e write a page to disk using fwrite file function using the current block Position.

15. ensureCapacity:
    