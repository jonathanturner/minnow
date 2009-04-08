// This file is distributed under the BSD license.
// See LICENSE.TXT for details.

#ifndef COMMON_H_
#define COMMON_H_

/** A bool for compatibility with older C */
#define CBOOL char
/** A true for compatibility with older C */
#define CTRUE 1
/** A false for compatibility with older C */
#define CFALSE 0

/**
 * The union that allows us to collapse all the datatypes into one value type
 */
typedef union {
    signed char Int8;
    unsigned char UInt8;
    signed short Int16;
    unsigned short UInt16;
    signed int Int32;
    unsigned int UInt32;
    signed long long Int64;
    unsigned long long UInt64;
    float Float;
    double Double;
    CBOOL Bool;
    void(*Function)(void *);
    void *VoidPtr;
}  Type_Union;

#endif /* COMMON_H_ */
