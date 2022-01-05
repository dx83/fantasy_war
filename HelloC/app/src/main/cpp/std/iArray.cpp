#include "iArray.h"

#include "iStd.h"

iArray::iArray(ARRAY_METHOD m)
{
	node = NULL;
	count = 0;
	method = m;
}

iArray::~iArray()
{
	removeAllObject();
}

void iArray::addObject(void* data)
{
	iNode* n = new iNode;
	n->data = data;
	n->prev = node;

	node = n;
	count++;
}
void iArray::addObject(int index, void* data)
{
	if (index < 0)
		index = 0;
	else
	{
		addObject(data);
		return;
	}

	iNode* n = node;
	for (int i = 0; i < count; i++)
	{
		if (count - 1 - i == index)
		{
			iNode* a = new iNode;
			a->data = data;
			a->prev = n->prev;

			n->prev = a;
			count++;
			return;
		}
		n = n->prev;
	}
}

void iArray::removeObject(int index)
{
	iNode* prevN = NULL;
	iNode* n = node;
	for (int i = 0; i < count; i++)
	{
		if (count - 1 - i == index)
		{
			prevN->prev = n->prev;
			if (method)
				method(n->data);
			delete n;
			count--;
			return;
		}
		prevN = n;
		n = n->prev;
	}
}

void iArray::removeData(void* data)
{
	iNode* prevN = NULL;
	iNode* n = node;
	for (int i = 0; i < count; i++)
	{
		if (n->data == data)//if (count - 1 - i == index)
		{
			prevN->prev = n->prev;
			if (method)
				method(n->data);
			delete n;
			count--;
#if 0// 한개만 지울때
			return;
#else// 모두 지울때
#endif
		}
		prevN = n;
		n = n->prev;
	}
}

void iArray::removeAllObject()
{
	iNode* n = node;
	for (; n;)
	{
		iNode* p = n->prev;
		if (method)
			method(n->data);
		delete n;

		n = p;
	}
}

void* iArray::objectAtIndex(int index)
{
	iNode* n = node;
	int i = count - 1;
	for (; n;)
	{
		if (i == index)
			return n->data;
		n = n->prev;
		i--;
	}

	return NULL;
}

void iArray::replaceAtIndex(int index, void* data, bool del)
{
	iNode* n = node;
	int i = count - 1;
	for (; n;)
	{
		if (i == index)
		{
			if (del)
			{
				if (method)
					method(n->data);
			}
			n->data = data;
			return;
		}
		n = n->prev;
		i--;
	}
}


iNode* iArray::nodeAtIndex(int index)
{
	iNode* n = node;
	int i = count - 1;
	for (; n;)
	{
		if (i == index)
			return n;
		n = n->prev;
		i--;
	}

	return NULL;
}

