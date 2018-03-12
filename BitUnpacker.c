/*
 * Copyright Software Innovations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UINT_SIZE 32
#define UINT_MASK 0xFFFFFFFF

typedef unsigned int UInt;

typedef struct {
   UInt curData;
   UInt nextData;
   int bitsLeft;
   int validNext;
} BitUnpacker;

void buInit(BitUnpacker *bup) { // Base: 3, Surcharge: 0
   bup->curData = 0x00000000;   // Base: 3, Surcharge: 0
   bup->bitsLeft = 0;           // Base: 3, Surcharge: 0
   bup->nextData = 0x00000000;  // Base: 3, Surcharge: 0
   bup->validNext = 1;          // Base: 3, Surcharge: 0
}

void BuTakeData(BitUnpacker *bup, UInt data) { // Base: 4, Surcharge: 0
   bup->nextData = data;                      // Base: 3, Surcharge: 0
}

int BuUnpack(BitUnpacker *bup, int size, UInt *out) {
   int ret = 0;
   int nsize = UINT_SIZE - size;
   if (bup->validNext == 1) {                                            
      bup->curData = bup->nextData;                                 
      bup->nextData <<= bup->bitsLeft;
      bup->validNext = 0;
   }
                                                             
   if (bup->validNext == 2) {
      *out = (bup->curData) | (bup->nextData >> (UINT_SIZE - bup->bitsLeft));
      bup->validNext = 1;
      ret = 1;
   }                                                                                                                             
   else if (bup->bitsLeft + size < UINT_SIZE) {
      *out = bup->nextData >> nsize;    
      bup->bitsLeft += size;                                        
      bup->nextData <<= size;                        
      bup->validNext = 0;  
      ret = 1;                                                                    
   }                                                               
   else if (bup->bitsLeft + size > UINT_SIZE) {                             
      bup->validNext = 2;
      bup->bitsLeft = bup->bitsLeft + size - UINT_SIZE;
      bup->curData = bup->nextData >> nsize;
      ret = 0;
      bup->validNext = 2;                                           
   }                                                               
   else {                                                            
      *out = bup->curData = bup->nextData >> nsize;                                         
      bup->nextData = 0;
      ret = 1;                                                       
      bup->validNext = 1;                                     
      bup->bitsLeft = 32;                                    
   }
    
   return ret;                                                      
}                                                                  
                                                                  