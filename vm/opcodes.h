//This file is included from a couple of places
//It's a definition of each opcode
//The macro "op" is expected to be defined by whatever file includes
//this one. The syntax is:
// op(name,
//    #of immediate args,
//    #of args taken on stack,
//    #of results pushed onto stack)
//For the CALL and CALL_NONE operations, the # of args taken on stack
//is specified as -1 since they take a variable number of args.
//For the RETURN and RETURN_NONE operations, the # of args pushed onto
//the stack is specified as -1 since they destroy the stack by
//returning from the current function.

/////// Pure data opcodes (manipulate stack, no side-effects)
//arithmetic
op(ARITH_PLUS,   0,2,1)
op(ARITH_MINUS,  0,2,1)
op(ARITH_TIMES,  0,2,1)
op(ARITH_DIV,    0,2,1)
op(ARITH_MOD,    0,2,1)
op(ARITH_NEGATE, 0,1,1)

  
//bitwise operators
op(BW_LSHIFT,    0,2,1)
op(BW_RSHIFT,    0,2,1)
op(BW_AND,       0,2,1)
op(BW_OR,        0,2,1)
op(BW_XOR,       0,2,1)
op(BW_NOT,       0,1,1)

  
//comparisons, only for arithmetic types (not pointers)
op(REL_EQ,       0,2,1)
op(REL_LEQ,      0,2,1)
op(REL_GEQ,      0,2,1)
op(REL_LT,       0,2,1)
op(REL_GT,       0,2,1)
op(REL_NE,       0,2,1)


//casts *TO* the specified type
op(CAST_S_CHAR,  0,1,1)
op(CAST_U_CHAR,  0,1,1)
op(CAST_S_SHORT, 0,1,1)
op(CAST_U_SHORT, 0,1,1)
op(CAST_S_INT,   0,1,1)
op(CAST_U_INT,   0,1,1)
op(CAST_S_LONG_LONG,0,1,1)
op(CAST_U_LONG_LONG,0,1,1)


//logical operators, these all operate only on ints and return ints
op(LOG_AND,      0,2,1)
op(LOG_OR,       0,2,1)
op(LOG_NOT,      0,1,1)


//no-op, pop something off the stack and ignore it
op(NOOP_POP,     0,1,0)


  
/////// Pointer manipulation
//dereference, store value pointed at on the stack
op(PTR_DEREF,    0,1,1)

//assign, store value through pointer
op(PTR_ASSIGN,   0,2,0)

//index into an array (i.e. add an integer to a pointer)
op(PTR_INDEX,    0,2,1)

//offset into a struct or union (i.e. point to a field)
op(PTR_OFFSET,   1,1,1)

//memory allocation
op(PTR_MALLOC,   0,1,1)
op(PTR_FREE,     0,1,0)

//arithmetic
op(PTR_DIFF,     0,2,1)
op(PTR_TO_LL,    0,1,1)
op(PTR_FROM_LL,  0,1,1)

//pointer->pointer casts. Takes 2 immediates, which make up a usertype_t in big-endian order
op(PTR_CAST,     2,1,1)


/////// Variables
//All of these operations take an immediate operand, an offset into
//the specified table (local, global, function or constant)
//load address of local or global or function onto the stack
op(VAR_LOAD_LOCAL,1,0,1)
op(VAR_LOAD_GLOBAL,1,0,1)
op(VAR_LOAD_FUNC,1,0,1)

//load value of a constant (from the literals table). In the case of
//literal strings, this is loaded as a pointer.
op(VAR_LOAD_CONSTANT,1,0,1)


/////// Control flow
//function call/return
//CALL_NONE is "call and ignore return value"
//RETURN_NONE is "return;" (as opposed to "return 4;")
op(FUNC_CALL,    1,-1,1)
op(FUNC_CALL_NONE,1,-1,0)
op(FUNC_RETURN,  0,1,-1)
op(FUNC_RETURN_NONE,0,1,-1)

//control flow within a function - both take a int16_t immediate
//jump offset (offset from the start of the goto instruction)
op(GOTO_ALWAYS,  1,0,0)
op(GOTO_COND,    1,0,0)

