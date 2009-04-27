#ifndef _ALTITUDE_BYTECODE_H
#define _ALTITUDE_BYTECODE_H
#include <stdint.h>
#include "atom.h"
#include "memtypes.h"



/* location information */
/* If there is no location information for this sexp, filename is
 * NULL and line and bytepos are -1.
 */
struct location{
  atom filename;
  int line, bytepos;
};

/* printing locations
   Usage: printf("%s happened here: " LOC_FMT ", %d times.",
                 whathappened, LOC_ARGS(location), ntimes);
*/

#define LOC_FMT "@\"%s\":%d:%d"
#define LOC_ARGS(l) l.filename->string, l.line, l.bytepos




/* Instruction format
 *
 * Many instructions require a type. For instance, addition of two
 * "int"s is subtly different from addition of two "char"s
 * (e.g. detecting overflow and the like). So, many operations carry
 * both and opcode and a type. There are no mixed-mode operations in
 * Altitude (e.g. in C you can do long x = (int)y +
 * (short)z). Instead, explicit casts are inserted to make all of the
 * operand types the same (there are subtle issues for shift
 * operators, however, as their operands do not have the same type).
 *
 * The "types" in question are just the primitive types (see primtype
 * in memtypes.h), not the full machinery of the Altitude type system
 * (as you can't, say, add two structs in C).
 * 
 * Instructions are stored as 16-bit unsigned integers.
 * The top byte holds the opcode, the bottom byte holds the type of
 * the operands, if specified. Otherwise it is 0.
 *
 * Some instructions take immediate, fixed operands. These are
 * inserted inline into the instruction stream. For instance, the
 * "instruction" following a JUMP_ALWAYS is not an instruction but a
 * 16-bit signed offset into the code array.
 */

typedef uint16_t instruction;

struct var_decl{
  atom name;
  usertype_t type;
  struct location loc;
};

struct function{
  int ID;
  atom name;

  usertype_t return_type;
  
  /* Declared arguments to the function */
  int nformals;
  struct var_decl* formals;

  /* Local variables */
  int nlocals;
  struct var_decl* locals;

  /* All function-scope variables (global & local) 
     First few are the formals, rest are the locals
     nvars == nformals + nlocals */
  int nvars;
  struct var_decl* vars;
  
  /* Actual code */
  int codelen;
  instruction* code;
};


struct program{
  struct typemap* typemap;
  int nfunctions;
  struct function* functions;
  int nglobals;
  struct var_decl* globals;

  struct function* main_function;
};

void var_decl_dump(int nvars, struct var_decl*);
void function_dump(struct function*);
void program_dump(struct program*);

typedef enum{
  /////// Pure data opcodes (manipulate stack, no side-effects)
  //arithmetic
  ARITH_PLUS, ARITH_MINUS, ARITH_TIMES, ARITH_DIV, ARITH_MOD, ARITH_NEGATE,

  //bitwise operators
  BW_LSHIFT, BW_RSHIFT, BW_AND, BW_OR, BW_XOR, BW_NOT,

  //comparisons, only for arithmetic types (not pointers)
  REL_EQ, REL_LEQ, REL_GEQ, REL_LT, REL_GT, REL_NE,

  //casts *TO* the specified type
  CAST_S_CHAR, CAST_U_CHAR, CAST_S_SHORT, CAST_U_SHORT,
  CAST_S_INT, CAST_U_INT, CAST_S_LONG_LONG, CAST_U_LONG_LONG,

  //logical operators, these all operate only on ints and return ints
  LOG_AND, LOG_OR, LOG_NOT,

  //no-op, pop something off the stack and ignore it
  NOOP_POP,

  
  /////// Pointer manipulation
  //dereference, store value pointed at on the stack
  PTR_DEREF,
  //assign, store value through pointer
  PTR_ASSIGN,
  //index into an array (i.e. add an integer to a pointer)
  PTR_INDEX,
  //offset into a struct or union (i.e. point to a field)
  PTR_OFFSET,
  //memory allocation
  PTR_MALLOC, PTR_FREE,
  //arithmetic
  PTR_DIFF, PTR_TO_LL, PTR_FROM_LL,
  //pointer->pointer casts. Takes 2 immediates, which make up a usertype_t in big-endian order
  PTR_CAST,

  /////// Variables
  //All of these operations take an immediate operand, an offset into
  //the specified table (local, global, function or constant)
  //load address of local or global or function onto the stack
  VAR_LOAD_LOCAL, VAR_LOAD_GLOBAL, VAR_LOAD_FUNC,
  //load value of local or global onto the stack (above followed by PTR_DEREF)
  VAR_LOADV_LOCAL, VAR_LOADV_GLOBAL,
  //load value of a constant (from the literals table). In the case of
  //literal strings, this is loaded as a pointer.
  VAR_LOAD_CONSTANT,

  /////// Control flow
  //function call/return
  FUNC_CALL, FUNC_RETURN,
  //control flow within a function - both take a int16_t immediate
  //jump offset
  GOTO_ALWAYS, GOTO_COND,
} opcode;



static inline primtype instr_type(instruction t){
  return t & 0xff;
}
static inline opcode instr_opcode(instruction t){
  return (t >> 8) & 0xff;
}
static inline instruction build_instr_typed(opcode a, primtype b){
  return ((a & 0xff) << 8) | (b & 0xff);
}
static inline instruction build_instr_untyped(opcode a){
  return build_instr_typed(a,0);
}

static inline int takes_immediate_arg(opcode op){
  return 
    op == GOTO_COND ||
    op == GOTO_ALWAYS || 
    op == VAR_LOAD_LOCAL ||
    op == VAR_LOAD_GLOBAL || 
    op == VAR_LOAD_FUNC ||
    op == PTR_OFFSET;
}
#endif
