#include <stdlib.h>
#include <algorithm>
using namespace std;
#pragma once

//profit: ~7.5% memory
#pragma pack(1)
typedef struct _bitBool
{
	unsigned int val : 1;
	int GetBit(int index);
	void SetBit(int bit);
} BitBool;

/*----------------------------------------------------------------
|	this struct is used instead of vector<bool/int>, is going to save
|	~1,5 * 8 * 32 - 1 = ~383 bits = ~48 bytes per 2D layer (32 maxsize calculations)
|*/
class BitLine {
public:
	int* line;
	int size;

	BitLine(int length);
	~BitLine();

	void Resize(int newSize);
	void CopyLineFrom(BitLine* other);

	int GetBit(int index);
	void SetBit(int index, int bit);
};