#include "byteset.h"

ByteSet operator |(const ByteSet &a, const ByteSet &b)
{
	ByteSet inter(a);

	for(int i=0; i<b.size(); ++i)
		if(!a.contains(b.at(i)))
			inter.append(b.at(i));

	return inter;
}

ByteSet operator &(const ByteSet &a, const ByteSet &b)
{
	ByteSet inter;

	for(int i=0; i<b.size(); ++i)
		if(a.contains(b.at(i)))
			inter.append(b.at(i));

	return inter;
}

ByteSet operator -(const ByteSet &a, const ByteSet &b)
{
	ByteSet inter;

	for(int i=0; i<a.size(); ++i)
		if(!b.contains(a.at(i)))
			inter.append(a.at(i));

	return inter;
}


/// Adds the bytes contained in b to a
ByteSet &operator |=(ByteSet &a, const ByteSet &b)
{
	for(int i=0; i<b.size(); ++i)
		if(!a.contains(b.at(i)))
			a.append(b.at(i));

	return a;
}

/// Only keeps in a the bytes also contained in b
ByteSet &operator &=(ByteSet &a, const ByteSet &b)
{
	for(int i=0; i<b.size(); ++i)
		if(a.contains(b.at(i)))
			a.append(b.at(i));

	return a;
}

/// Removes from a the bytes contained in b
ByteSet &operator -=(ByteSet &a, const ByteSet &b)
{
	int index;

	for(int i=0; i<b.size(); ++i)
	{
		index=a.indexOf(b.at(i));
		if(index!=-1)
			a.remove(index, 1);
	}

	return a;
}
