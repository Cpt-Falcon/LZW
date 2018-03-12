#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "LZWExp.h"
#include "BitUnpacker.h"
#include "SmartAlloc.h"
#include "MyLib.h"
#include "CodeSet.h"
#define BITS_INIT 9
#define MAX_INIT 512
#define DICT_SIZE 256
#define CODE_SET_SIZE 4096
/* Initialize a LZWExp the DataSink to which to send uncompressed symbol 
 * sequences and an anonymous state object for the DataSink.
 */
/*
 typedef struct LZWExp {
   void *dict;        // Ptr to CodeSet dictionary of codes
   DataSink sink;     // Data sink to send char sequences to
   void *sinkState;   // Unknown object to send to sink for state
   int lastCode;      // Last code that was created
   int numBits;       // Number of bits per code currently
   int maxCode;       // Max code that fits in numBits bits.
   int recycleCode;   // Code that triggers recycling of dictionary
   BitUnpacker bitUnpacker; // Used to uncompress data
   int EODSeen;       // Indicates that an EOD code has been seen
} LZWExp;
*/
void OutCode(LZWExp *exp, UInt ndx) {
	Code code = GetCode(exp->dict, ndx);
	int val = code.size;                                  // Base: 3, Surcharge: 0
	while (val--) {                                   // Base: 4, Surcharge: 1.8
	   printf("%c", *code.data++);                   // Base: 7, Surcharge: 0
	}                                                   
}

UInt firstCode(LZWExp *exp, UInt ndx) {
	Code code = GetCode(exp->dict, ndx);
	//printf("fc:--------- %c\n", *code.data);
	return *code.data;                                                  
}

void LZWExpInit(LZWExp *exp, DataSink sink, void *sinkState, int recycleCode) {
	UInt initDic = 0;
	BitUnpacker unpack;
	buInit(&unpack);
	exp->dict = CreateCodeSet(CODE_SET_SIZE);
	exp->lastCode = 0;
	while (initDic <= DICT_SIZE) {
		NewCode(exp->dict, initDic);
		initDic++;
	}
	exp->EODSeen = 0;
	exp->numBits = BITS_INIT;
	//exp->maxCode = pow(2, exp->numBits)  - 1;
	exp->maxCode = MAX_INIT;
	exp->recycleCode = recycleCode;
	exp->sink = sink;
	exp->sinkState = sinkState;
	exp->bitUnpacker = unpack;
}

/* Break apart compressed data in "bits" into one or more codes and send 
 * the corresponding symbol sequences to the DataSink.  Save any leftover 
 * compressed bits to combine with the bits from the next call of 
 * LZWExpEncode.  Return 0 on success or BAD_CODE if you receive a code not
 * in the dictionary.
 *
 * For this and all other methods, a code is "invalid" if it either could not
 * have been sent (e.g. is too high) or if it is a nonzero code following
 * the detection of an EOD code.
 */


int LZWExpDecode(LZWExp *exp, UInt bits) {
	UInt out = 0, test = 0, end = 1, prev = -1;
	int shift = 0;
	UInt mask = 0xFFFFFFFF;
	BuTakeData(&(exp->bitUnpacker), bits);	
	while (BuUnpack(&(exp->bitUnpacker), exp->numBits, &out)) {
		if (exp->lastCode) {
			SetSuffix(exp->dict, exp->lastCode, firstCode(exp, out));
		}
		OutCode(exp, out);
		exp->lastCode = ExtendCode(exp->dict, out);
		if (exp->lastCode == exp->maxCode) {
			exp->maxCode = exp->maxCode * 2;
			exp->numBits++;
		}
	}
	//out = BAD_CODE;
	return 0;
}

/* Called after EOD has been seen to peform any error checking and/or
 * housekeeping that should be performed at the end of decoding. Returns 0 if
 * all is OK, NO_EOD if no terminating EOD code has been found, or BAD_CODE
 * if either non-zero bits follow the EOD or if one or more extra intextra an int-block(s) 
 */
int LZWExpStop(LZWExp *exp) {
	return 0;
}

/* Free all storage associated with LZWExp (not the sinkState, though,
 * which is "owned" by the caller.  Must be called even if LZWExpInit
 * returned an error.
 */
void LZWExpDestruct(LZWExp *exp) {
	DestroyCodeSet(exp->dict);
	free(exp->sink);
	free(exp->sinkState);
	free(exp);
}
