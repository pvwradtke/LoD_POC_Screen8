#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE   8192
#define STAGE_SIZE  819200

uint8_t buffer[PAGE_SIZE];
uint8_t packedstage[STAGE_SIZE];

int main(int argc, char *argv[]){
    unsigned char *current;
    int totalBytes=0;
    int currentPage=0;
    int totalBlocks;
    char name[256];
    int16_t offset=0;
    uint8_t blockSize;
    if(argc != 3){
        printf("Usage: pack_stage basename_ blockcount\n");
        return 1;
    }
    totalBlocks=atoi(argv[2]);
    printf("There are %d blocks to process.\n", totalBlocks);
    for(int i=1;i<=totalBlocks;i++){
        // opens the current block
        sprintf(name, "%s%04d.lz48", argv[1], totalBlocks-i);        
        FILE *block=fopen(name, "rb");
        if(block==0){
            printf("Panic at the disco!\n");
            exit(1);
        }
        // What is the block size
        fseek(block, 0, SEEK_END);
        blockSize = ftell(block);
        if(blockSize>=255){
            printf("Invalid block size, %d bytes (maximum is 254 bytes)\n", blockSize);
            return(1);
        }
        fseek(block, 0, SEEK_SET);
        // Reads the block
        fread(buffer, 1, blockSize, block);
        fclose(block);
        
        // Do we still fit inside the curren page?
        // Need 2 bytes to jump to next page (-1)
        if(offset+blockSize+2>=PAGE_SIZE-2){
            //((int16_t*)
            packedstage[PAGE_SIZE*currentPage+offset]=255;
            currentPage++;
            offset=0;
            printf("Moved to page %d.\n", currentPage);
        }
        
        // Writes the offset           
        printf("Processing %s - %04d bytes - offset: %04d\n", name, blockSize, offset);
        //((int16_t*)
        packedstage[PAGE_SIZE*currentPage+offset]=blockSize;
        // Advances two bytes
        offset++;
        // Writes the data block
        memcpy(&packedstage[PAGE_SIZE*currentPage+offset], buffer, blockSize);
        offset+=blockSize;
    }
    // Finally, we need to end the block with a 0 offset
    //((int16_t*)
    packedstage[PAGE_SIZE*currentPage+offset]=0;
    // Now, write the packed file
    sprintf(name, "%spacked", argv[1]);
    FILE *packed = fopen(name, "wb");
    fwrite(packedstage, 1, PAGE_SIZE*(currentPage+1), packed);
    fclose(packed);
    exit(0);
}
