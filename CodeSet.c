#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "SmartAlloc.h"
#include "CodeSet.h"
typedef struct CodeEntry {                                         
   unsigned char code;                                             
   void *prev;                                                     
   unsigned char *freePos;                                         
   short size;                                                     
   unsigned char getCalls;                                         
} CodeEntry;                                                       
                                                                   
typedef struct CodeSet {                                           
   struct CodeEntry *entries;                                      
   short size;                                                     
} CodeSet;                                                         
                                                                   
void *CreateCodeSet(int numCodes) {                                // Base: 3, Surcharge: 0
   CodeSet *newCodeSet = malloc(sizeof(CodeSet));                  // Base: 6, Surcharge: 0
   newCodeSet->size = 0;                                           // Base: 3, Surcharge: 0
   newCodeSet->entries = malloc(numCodes * sizeof(CodeEntry));     // Base: 8, Surcharge: 0
   return newCodeSet;                                              // Base: 1, Surcharge: 0
}                                                                  
                                                                   
int NewCode(void *codeSet, char val) {                             // Base: 4, Surcharge: 0
   CodeSet *set = (CodeSet *)codeSet;                              // Base: 4, Surcharge: 0
   CodeEntry *tmpEntries = set->entries + set->size;               // Base: 5, Surcharge: 0
   tmpEntries->freePos = NULL;                                     // Base: 3, Surcharge: 0
   tmpEntries->prev = NULL;                                        // Base: 3, Surcharge: 0
   tmpEntries->code = val;                                         // Base: 3, Surcharge: 0
   tmpEntries->size = 1;                                           // Base: 3, Surcharge: 0
   tmpEntries->getCalls = 0;                                       // Base: 3, Surcharge: 0
   set->size++;                                                    // Base: 2, Surcharge: 0
   return set->size - 1;                                           // Base: 3, Surcharge: 0
                                                                   
}                                                                  
                                                                   
Code GetCode(void *codeSet, int code) {                            // Base: 4, Surcharge: 0
   int size, tmpsize = 0;                                          // Base: 3, Surcharge: 0
   Code output;                                                    
   CodeEntry *tmpEntries, *oldEntries;                             
   oldEntries = tmpEntries = ((CodeSet *)codeSet)->entries + code; // Base: 11, Surcharge: 0
   tmpsize = size = tmpEntries->size;                              // Base: 8, Surcharge: 0
   if (oldEntries->freePos) {                                      // Base: 3, Surcharge: 0
      output.data = oldEntries->freePos;                           // Base: 3, Surcharge: 0
   }                                                               
   else {                                                          // Base: 1, Surcharge: 0
      output.data = malloc(size);                                  // Base: 5, Surcharge: 0
   }                                                               
   while (tmpEntries && !tmpEntries->freePos) {                    // Base: 6, Surcharge: 3
      output.data[tmpsize-- - 1] = tmpEntries->code;               // Base: 8, Surcharge: 0
      tmpEntries = tmpEntries->prev;                               // Base: 3, Surcharge: 0
   }                                                               
   output.size = size;                                             // Base: 3, Surcharge: 0
   if (tmpEntries) {                                               // Base: 3, Surcharge: 0
      memcpy(output.data, tmpEntries->freePos, tmpsize);           // Base: 5, Surcharge: 0
   }                                                               
   oldEntries->freePos = output.data;                              // Base: 3, Surcharge: 0
   oldEntries->getCalls++;                                         // Base: 2, Surcharge: 0
   return output;                                                  // Base: 1, Surcharge: 0
}                                                                  
                                                                   
int ExtendCode(void *codeSet, int oldCode) {                       // Base: 4, Surcharge: 0
   int extend;                                                     
   CodeEntry *tmpEntries = ((CodeSet *)codeSet)->entries;          // Base: 4, Surcharge: 0
   CodeEntry *oldEntry = tmpEntries + oldCode;                     // Base: 5, Surcharge: 0
   tmpEntries += extend = NewCode(codeSet, '\0');                  // Base: 11, Surcharge: 0
   tmpEntries->size = oldEntry->size + 1;                          // Base: 5, Surcharge: 0
   tmpEntries->prev = oldEntry;                                    // Base: 3, Surcharge: 0
   return extend;                                                  // Base: 1, Surcharge: 0
                                                                   
}                                                                  
                                                                   
void FreeCode(void *codeSet, int code) {                           // Base: 4, Surcharge: 0
   CodeEntry *tmpEntries = ((CodeSet *)codeSet)->entries + code;   // Base: 6, Surcharge: 0
   tmpEntries->getCalls--;                                         // Base: 2, Surcharge: 0
   if (tmpEntries->getCalls == 0) {                                // Base: 5, Surcharge: 0
      free(tmpEntries->freePos);                                   // Base: 3, Surcharge: 0
      tmpEntries->freePos = NULL;                                  // Base: 3, Surcharge: 0
   }                                                               
}                                                                  
                                                                   
void DestroyCodeSet(void *codeSet) {                               // Base: 3, Surcharge: 0
   CodeSet *set = (CodeSet *)codeSet;                              // Base: 4, Surcharge: 0
   CodeEntry *tmpEntries = set->entries;                           // Base: 3, Surcharge: 0
   while (set->size--) {                                           // Base: 4, Surcharge: 2
      if (tmpEntries->freePos) {                                   // Base: 3, Surcharge: 0
         free(tmpEntries->freePos);                                // Base: 3, Surcharge: 0
      }                                                            
      tmpEntries++;                                                // Base: 2, Surcharge: 0
   }                                                               
   free(set->entries);                                             // Base: 3, Surcharge: 0
   free(set);                                                      // Base: 3, Surcharge: 0
}                                                                  
                                                                   
void SetSuffix(void *codeSet, int code, char suffix) {             // Base: 5, Surcharge: 0
   CodeEntry *tmpEntries = ((CodeSet *)codeSet)->entries + code;   // Base: 6, Surcharge: 0
   if (tmpEntries->freePos) {                                      // Base: 3, Surcharge: 0
      *(tmpEntries->freePos + tmpEntries->size - 1) = suffix;      // Base: 8, Surcharge: 0
   }                                                               
   tmpEntries->code = suffix;                                      // Base: 3, Surcharge: 0
}                                                                  
