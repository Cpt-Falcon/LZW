#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LZWExp.h"
#include "SmartAlloc.h"
#include "MyLib.h"



int main() {
   UInt hexVal;
   DataSink tmp = malloc(sizeof(DataSink));
   void *sinkState = malloc(1);
   LZWExp *prim = malloc(sizeof(LZWExp));
   LZWExpInit(prim, tmp, sinkState, 4096);
   while (scanf("%08X", &hexVal) != EOF) {
      LZWExpDecode(prim, hexVal);
      //printf("%08X ", hexVal);   
   }
   LZWExpDestruct(prim);
   printf("\n");
   printf("Space: %lu\n", report_space());
   printf("\n");
   return 0;
}