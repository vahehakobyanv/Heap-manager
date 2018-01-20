#include "myheader.h"

int segmentnumber = 0;



bool _isMemoryEnough(int requiredMem)
{
    return (requiredMem < MAX_MEM_SIZE ? true : false);
}

_SBLOCK *allocateMemBlock(size_t size)
{
    _SBLOCK *block = sbrk(0);
    if(sbrk(BLOCK_SIZE + size) == (void*)-1){
        return NULL;
    }else{
        block->next = NULL;
        block->prev = NULL;
        block->isfree = false;
        block->size = size;
        return block;
    }
}


//allocate next memory block
void allocateNextMemBlock(size_t size, _SBLOCK **head)
{
    _SBLOCK *current = *head;
    void *allocate_mem = NULL;
    if(current==NULL){
        *head = allocateMemBlock(size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        _SBLOCK *newblock = sbrk(0);

        allocate_mem = sbrk(BLOCK_SIZE + size);      
        if(allocate_mem == (void*) - 1){ }
        else{
            sbrk(BLOCK_SIZE + size);
            newblock->next = NULL;
            newblock->prev = current;
            newblock->isfree = false;
            newblock->size = size;
            current->next = newblock;
      }
    }
}



//allocate memory block for Paging
_MEMPAGEBLOCKS *allocateMemPageBlock(size_t size)
{
    _MEMPAGEBLOCKS *block = sbrk(0);
    if(sbrk(MEM_PAGE_BLOCK_SIZE + size) == (void*) - 1){ return NULL;}
    else{
        sbrk(MEM_PAGE_BLOCK_SIZE + size);
        block->next = NULL;
        block->isfree = true;
        block->memoryaddress = sbrk(0);
        return block;
    }
}


//allocate next memory block for Paging
void allocateNextMemPageBlock(size_t size, _MEMPAGEBLOCKS **head)
{
    _MEMPAGEBLOCKS *current = *head;
    void *allocate_mem = NULL;
    if(current == NULL){
        *head = allocateMemPageBlock(size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        _MEMPAGEBLOCKS *newblock = sbrk(0);

        allocate_mem = sbrk(MEM_PAGE_BLOCK_SIZE + size);      
        if(allocate_mem == (void*) - 1){ }
        else{
            sbrk(MEM_PAGE_BLOCK_SIZE + size);
            newblock->next = NULL;
            newblock->isfree = true;
            newblock->memoryaddress = sbrk(0);
            current->next = newblock;
      }
    }
}


//allocate memory block for Segmentation
_MEMSEGMENTBLOCKS *allocateMemSegmentBlock(size_t size)
{
    _MEMSEGMENTBLOCKS *block = sbrk(0);
    if(sbrk(MEM_SEGMENT_BLOCK_SIZE + size) == (void*) - 1){ return NULL;    }
    else{
        sbrk(MEM_SEGMENT_BLOCK_SIZE + size);
        block->next = NULL;
        block->isfree = false;
        block->size = size;
        block->memoryaddress = sbrk(0);
        return block;
    }
}


//allocate next memory block for Segmentation
void allocateNextMemSegmentBlock(size_t size, _MEMSEGMENTBLOCKS **head)
{
    _MEMSEGMENTBLOCKS *current = *head;
    void *allocate_mem = NULL;
    if(current == NULL){
        *head = allocateMemSegmentBlock(size);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        _MEMSEGMENTBLOCKS *newblock = sbrk(0);

        allocate_mem = sbrk(MEM_SEGMENT_BLOCK_SIZE + size);      
        if(allocate_mem == (void*) - 1){ }
        else{
            sbrk(MEM_SEGMENT_BLOCK_SIZE + size);
            newblock->next = NULL;
            newblock->isfree = false;
            newblock->size = size;
            newblock->memoryaddress = sbrk(0);
            current->next = newblock;
      }
    }
}



void allocate_allMemory(_SBLOCK **blockHead,_PROCINTNODE *procinthead)
{
    _PROCINTNODE *current = procinthead;

    printf("\n\t\t[ All Memory allocated to processes]\n");
    printf("-------------------------------------------------------------------------");
    printf("\n|  Process   |   Start Address   |   End Address    |      Size       |\n");
    printf("-------------------------------------------------------------------------\n");

    while(current != NULL){
        void *start_address = sbrk(0) + 1;
        allocateNextMemBlock(current->size + 1, &(*blockHead));
        void *end_address = sbrk(0);
        float size_kb = (current->size)/1024.0;

        printf("|     P%d     |     %p     |    %p     |     %.3fKB     |\n",
                current->process, start_address, end_address, size_kb);

        current = current->next;
    }
    printf("-------------------------------------------------------------------------\n");
    printf("\n\n");
    printf("\n\tCurrent brk pointer is here (sbrk(0)) = %p\n", sbrk(0));
    
    printf("\n\t\t\t[ Memory released ]\n\n");
    _SBLOCK *lastblock, *blockcurrent = *blockHead;
    while(blockcurrent != NULL){
        blockcurrent->isfree = true;
        if(blockcurrent->next == NULL){
                lastblock = blockcurrent;
        }
        blockcurrent = blockcurrent->next;
    }

    blockcurrent = lastblock;
    int proc_length = _countPROCINTNodes(procinthead);

    while(blockcurrent->prev != NULL){
        if(blockcurrent->isfree){
            (blockcurrent->prev)->next = NULL;
            if(blockcurrent->prev == NULL){
                sbrk(0-(BLOCK_SIZE + blockcurrent->size + 1));
                printf("\nProcess P%d : sbrk(0) is %p\n", proc_length, sbrk(0));        
            }
            sbrk(0-(BLOCK_SIZE + blockcurrent->size + 1));
            printf("\nProcess P%d : sbrk(0) is %p\n", proc_length, sbrk(0));
        }
        blockcurrent = blockcurrent->prev;
        proc_length--;
    }

    sbrk(0 - (BLOCK_SIZE + blockcurrent->size + 1));
    printf("\nProcess P%d : sbrk(0) is %p\n",proc_length,sbrk(0));
    *blockHead=NULL; 

}




void allocate_using_FCFS(_SBLOCK **blockHead, _PROCINTNODE *procinthead)
{
    _PROCINTNODE *current = procinthead;

    printf("\n\t\t[ Memory allocated using FCFS ]\n");
    printf("\nsbrk(0) = %p\n", sbrk(0));

    int count = 0;
    
    while(current != NULL){
        if(current->size > (MAX_MEM_SIZE - 4096)){
            printf("\n\n*******************************************************************************************");
            printf("\n** Size of given memory(%.3fKB) for process P%d is too large, Cannot allocate memory.....\n", 
                            (current->size)/1024.0, current->process);
            printf("*******************************************************************************************\n\n");
            current = current->next;
        }else{
            void *start_address = sbrk(0) + 1;
            allocateNextMemBlock(current->size + 1, &(*blockHead));
            void *end_address = sbrk(0);
            float size_kb = (current->size)/1024.0;

            printf("\n\n[ Memory allocated    :    Process  =  P%d ]", current->process);

            printf("\nStart-memory-address = %p, End-memory-address = %p, Size = %.3fKB, sbrk(0) = %p\n",
                    start_address, end_address, size_kb, sbrk(0));

            sbrk(0 - (current->size + BLOCK_SIZE - 3));

            printf("[ Memory released    :    Process  =  P%d ]\n", current->process);
            printf("\nsbrk(0) = %p\n", sbrk(0));
     
            count++;
            current = current->next;
        }
    }
}



void divideProc_Mem_IntoPageBlocks(_PROCINTNODE *procinthead, _VIRTMEMPAGEBLOCKS **virtmempageblockshead,
                                    _MEMPAGEBLOCKS **mempageblocksHead)
{
    _PROCINTNODE *current = procinthead;
    unsigned int pagenumber = 0;
    void *address = (void*)0x12345678;


    while(current != NULL){
        if(current->size < PAGE_SIZE){
            _virtmempageblocksinsertAtEnd(current->process, PAGE_SIZE, pagenumber, address, &(*virtmempageblockshead));
            current = current->next;
        }else{
            _virtmempageblocksinsertAtEnd(current->process, PAGE_SIZE, pagenumber, address, &(*virtmempageblockshead));
            current->size = (current->size - PAGE_SIZE);
            pagenumber++;
            address += PAGE_MEM_SIZE;
        }
    }


   
    for(int i = 0; i < MAX_PAGES; i++){
        allocateNextMemPageBlock(PAGE_MEM_SIZE, &(*mempageblocksHead));
    }
}

void mapVirtPhyAddressPageTable(_VIRTMEMPAGEBLOCKS **virtmempageblockshead, _MEMPAGEBLOCKS **mempageblocksHead)
{
    _MEMPAGEBLOCKS *currentmem = *mempageblocksHead;
    _MEMPAGEBLOCKS *currentmem2 = *mempageblocksHead;
    _VIRTMEMPAGEBLOCKS *currentvirt = *virtmempageblockshead;
    _MEMPAGEBLOCKS *currtemp = NULL;
    int count = 0;


    printf("\n[ Memory mapped Virtual/Physical Page Table ]\n");
    printf("\n-----------------------------------------------------------------------------------");
    printf("\n|    Process    |    PageNumber    |    VirtualAddress    |    PhysicalAddress    |\n");
    printf("-----------------------------------------------------------------------------------\n");

    while(currentmem != NULL){
            if(currentmem->isfree){
                    printf("|       P%d      |        %d         |     %p       |        %p      |\n",
                        currentvirt->process, currentvirt->pagenumber, currentvirt->memoryaddress, currentmem->memoryaddress);
                    
                    currentmem->isfree = false;
                    count++;

                    

                    if(count == 256){
                        printf("***- 1MB block of pages loaded -***\n");
                        count = 0;
                    }
            }

            currentvirt = currentvirt->next;
            currentmem = currentmem->next;

            if(currentvirt == NULL){
                goto exitLoop;
            }
    }

    printf("-----------------------------------------------------------------------------------\n");


    currentmem = *mempageblocksHead;
    while(currentmem != NULL){
        if(!(currentmem->isfree)){
            currentmem->isfree = true;
            printf("Memory Frame freed, Address = %p\n", currentmem->memoryaddress);
        }
        currentmem = currentmem->next;
    }

    mapVirtPhyAddressPageTable(&currentvirt, &(*mempageblocksHead));

exitLoop:

    currentmem = *mempageblocksHead;
    while(currentmem != NULL){
        if(!(currentmem->isfree)){
            currentmem->isfree = true;
            printf("Memory Frame freed, Address = %p\n", currentmem->memoryaddress);
        }
        currentmem = currentmem->next;
    }
}




void allocateMemory_using_Segmentation(_PROCINTNODE **procinthead, _MEMSEGMENTBLOCKS **memSegBlockshead)
{
    _PROCINTNODE *current = *procinthead;
    int total_size = 0;

    printf("\nsbrk(0) = %p\n", sbrk(0));

    while(current != NULL){
        if(current->size <= MAX_SEGMENT_SIZE){
            if(total_size + current->size < MAX_MEM_SIZE){
                void *start_address = sbrk(0) + 1;
                allocateNextMemSegmentBlock(current->size + 1, &(*memSegBlockshead));
                void *end_address = sbrk(0);

                total_size += current->size;

                printf("\nProcess = P%d, start_address=%p, end_address=%p, segmentnumber=%d\n",
                        current->process, start_address, end_address, segmentnumber);

               

                printf("\ntotal_size=%d\n", total_size);

                current->size = 0;
                current = current->next;
                segmentnumber++;
            }else{
                total_size = 0;
                break;
            }
        }else{
                if(total_size + MAX_SEGMENT_SIZE < MAX_MEM_SIZE){
                    void *start_address = sbrk(0) + 1;
                    allocateNextMemSegmentBlock(MAX_SEGMENT_SIZE, &(*memSegBlockshead));
                    void *end_address = sbrk(0);
                    total_size += MAX_SEGMENT_SIZE;

                    printf("\nProcess = P%d, start_address=%p, end_address=%p, segmentnumber=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                    printf("\ntotal_size=%d\n", total_size); 

                    current->size = current->size - MAX_SEGMENT_SIZE;
                    segmentnumber++;
                }else{
                    total_size = 0;
                    break;
                }
            }
    }

    current = *procinthead;
    _deleteProcINTNode_ZeroData(&current);
    current = *procinthead;

    if(_countPROCINTNodes(current) == 1){
        if(current->size == 0){ *procinthead = NULL;}
    }else{
            *procinthead = (*procinthead)->next;
    }

    allocateMemory_using_Segment_remain(&(*procinthead), &(*memSegBlockshead));
}




void getFreeMemoryAddress(_MEMSEGMENTBLOCKS *memSegBlockshead, unsigned int size, void **start_address)
{
    _MEMSEGMENTBLOCKS *current = memSegBlockshead;

    int size_temp = current->size;

    while(current != NULL){
        if(current->isfree){
            if(size_temp >= size){
                *start_address=current->memoryaddress;
                size_temp = 0;
                break;
            }
            size_temp += current->size;
            current = current->next;
        }else{ current = current->next; }
    }
}



void allocateMemory_using_Segment_remain(_PROCINTNODE **procinthead, _MEMSEGMENTBLOCKS **memSegBlockshead)
{
    _PROCINTNODE *current = *procinthead;
    int total_size = 0;
    _MEMSEGMENTBLOCKS *segcurrent = *memSegBlockshead;

    while(segcurrent != NULL){
        segcurrent->isfree = true;
        segcurrent = segcurrent->next;
    }
    
    segcurrent = *memSegBlockshead;
    void *start_address;
    
    while(current != NULL){
        if(current->size <= MAX_SEGMENT_SIZE){
            if(total_size + current->size < MAX_MEM_SIZE){
                
                getFreeMemoryAddress(*memSegBlockshead, current->size, &start_address);

                void *end_address = start_address + current->size;

                total_size += current->size;

                printf("\nProcess = P%d, start_address=%p, end_address=%p, segmentnumber=%d\n",
                        current->process, start_address, end_address, segmentnumber);


                printf("\ntotal_size=%d\n", total_size);

                current->size = 0;
                current = current->next;
                segmentnumber++;
            }else{
                total_size = 0;
                break;
            }
        }else{
                if(total_size + MAX_SEGMENT_SIZE < MAX_MEM_SIZE){

                    if(segcurrent->next == NULL)
                        segcurrent = *memSegBlockshead;
                    else
                        segcurrent = segcurrent->next;

                    getFreeMemoryAddress(segcurrent, MAX_SEGMENT_SIZE, &start_address);

                    void *end_address = start_address + MAX_SEGMENT_SIZE;

                    printf("\nProcess = P%d, start_address=%p, end_address=%p, segmentnumber=%d\n",
                        current->process, start_address, end_address, segmentnumber);

                  

                    total_size += MAX_SEGMENT_SIZE;

                    printf("\ntotal_size=%d\n", total_size);

                    current->size = current->size - MAX_SEGMENT_SIZE;
                    segmentnumber++;
                }else{
                    total_size = 0;
                    break;
                }
            }
    }    
}




void AllocateALLMemory(_SBLOCK *s_blockHead, const char *inputFile)
{
  
    _INTNODE *dataList = getProcessData(inputFile);
    _PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    printf("\n\n\t* Reading data from file \"%s\"......\n", inputFile);

   
    if(_isMemoryEnough(getProcessMemSizeSum(procintHead))){
            printf("\n\n\t -- Memory is enough to fulfill all processes --\n");
            allocate_allMemory(&s_blockHead, procintHead);
    }else{
            printf("\nError: Memory is not enough to fulfill all processes\nExiting...\n\n");
            exit(0);
        }
}



void AllocateFCFS(_SBLOCK *s_blockHead, const char *inputFile)
{
    _INTNODE *dataList = getProcessData(inputFile);
    _PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    printf("\n\n\t* Reading data from file \"%s\"......\n", inputFile);

    allocate_using_FCFS(&s_blockHead, procintHead);
}


void AllocatePAGING(_SBLOCK *s_blockHead,_VIRTMEMPAGEBLOCKS *virtmempageBlocks,
                    _MEMPAGEBLOCKS *mempageBlocks, const char *inputFile)
{
    _INTNODE *dataList = getProcessData(inputFile);
    _PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    printf("\n\n\t* Reading data from file \"%s\"......\n", inputFile);

    divideProc_Mem_IntoPageBlocks(procintHead, &virtmempageBlocks, &mempageBlocks);

    printf("\n\n\t\t[ Allocate memory using Paging ]\n\n");

    mapVirtPhyAddressPageTable(&virtmempageBlocks, &mempageBlocks);
}



void AllocateSEGMENTATION(_SBLOCK *s_blockHead, _MEMSEGMENTBLOCKS *memSegBlocksHead, const char *inputFile)
{
    //get data from input file
    _INTNODE *dataList = getProcessData(inputFile);
    _PROCINTNODE *procintHead = NULL;
    procintHead = getDataFromINTNODEList(dataList, &procintHead);
    
    printf("\n\n\t[ Allocate memory using Segmentation ]\n");
    do{
        if(procintHead->next == NULL){
            if(procintHead->size == 0)
                break;
        }else{
            _deleteProcINTNode_ZeroData(&procintHead);
            allocateMemory_using_Segmentation(&procintHead, &memSegBlocksHead);
        }
    }while(procintHead != NULL);

}

