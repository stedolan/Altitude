//This file is included from a couple of places
//It's a definition of each primitive type
//The macro "prim" is expected to be defined by whatever file 
//includes this one. The syntax is:
// prim(name,
//      representation type,
//      ID number,
//      C name)
//The representation types are available from memtypes.h

//Void is given a reptype of char to avoid breaking things
//(variables can't be declared void)
//Uninitialised memory has type void
prim(PT_VOID,        char,     0,  "void")
//in Altitude, plain "char" is signed
prim(PS_CHAR,        int8_t,   1,  "signed char")
prim(PU_CHAR,        uint8_t,  2,  "unsigned char")
prim(PS_SHORT,       int16_t,  3,  "signed short")
prim(PU_SHORT,       uint16_t, 4,  "unsigned short")
//int and long are both 32-bit types
prim(PS_INT,         int32_t,  5,  "signed int")
prim(PU_INT,         uint32_t, 6,  "unsigned int")
prim(PS_LONG_LONG,   int64_t,  7,  "signed long long")
prim(PU_LONG_LONG,   uint64_t, 8,  "unsigned long long")

//The number of non-pointer primitive types
#ifndef N_PRIMITIVE_DATA_TYPES
#define N_PRIMITIVE_DATA_TYPES 9
#endif

//Can't give a proper C name because we don't know the type 
//it points to.
prim(PT_PTR,         userptr_t,100,"<ptr>")
