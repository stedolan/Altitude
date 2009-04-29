#ifndef _ALTITUDE_BYTECODE_H
#define _ALTITUDE_BYTECODE_H
#include <stdint.h>
#include "atom.h"
#include "memtypes.h"
#include "sexp.h"

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
typedef int16_t signed_immediate;
typedef uint16_t unsigned_immediate;

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
  
  /* Constants used in the function */
  int nconsts;
  primitive_val* consts;
  
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
  int nfunctions;
  struct function* functions;
  int nglobals;
  struct var_decl* globals;

  struct function* main_function;
};

struct program* compile(struct sexp*);

void var_decl_dump(int nvars, struct var_decl*);
void function_dump(struct function*);
void program_dump(struct program*);

typedef enum{
#define op(name, imm, in, out) name,
#include "opcodes.h"
#undef op
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
int count_immediates(opcode);

#endif
