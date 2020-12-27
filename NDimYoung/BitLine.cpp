#include "BitLine.h";

int BitBool::GetBit(int index)
{
	return val;
}

void BitBool::SetBit(int bit)
{
	val = bit;
}

BitLine::BitLine(int length)
{
	size = length;
	line = (int*)malloc(length);
}

BitLine::~BitLine()
{
	free(line);
}

void BitLine::Resize(int newSize)
{
	size = newSize;
	line = (int*)realloc(line, newSize);
}

void BitLine::CopyLineFrom(BitLine* other)
{
	memcpy(line, other->line, other->size);
}

int BitLine::GetBit(int index)
{
	return (*line >> index) & 1;
}

void BitLine::SetBit(int index, int bit)
{
	*line ^= (-bit ^ *line) & (1ULL << index);
}