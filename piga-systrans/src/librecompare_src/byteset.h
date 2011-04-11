#ifndef BYTESET_H
#define BYTESET_H

#include <QByteArray>

typedef QByteArray ByteSet;

/// Returns the bytes contained in either array a or b
ByteSet operator |(const ByteSet &a, const ByteSet &b);

/// Returns the bytes contained in both array a and b
ByteSet operator &(const ByteSet &a, const ByteSet &b);

/// Returns the bytes contained in array a but not in b
ByteSet operator -(const ByteSet &a, const ByteSet &b);

/// Adds the bytes contained in b to a
ByteSet &operator |=(ByteSet &a, const ByteSet &b);

/// Only keeps in a the bytes also contained in b
ByteSet &operator &=(ByteSet &a, const ByteSet &b);

/// Removes from a the bytes contained in b
ByteSet &operator -=(ByteSet &a, const ByteSet &b);


#endif // BYTESET_H
