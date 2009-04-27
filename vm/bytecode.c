#include <stdlib.h>
#include "memtypes.h"
#include "bytecode.h"
#include "error.h"
#include "sexp.h"
#include "hashtable.h"
#include "error.h"


void build_var_decls(int size, struct sexp_element* decllist, int* outsize, struct var_decl** outvars){
  struct var_decl* vars = malloc(size * sizeof(struct var_decl));
  for (int i=0;i<size;i++){
    struct sexp* var = decllist[i].data.sexp;
    if (!(is_sexp_with_tag(decllist[i], S_VAR) &&
	  var->nelems == 2 && 
	  var->elems[0].type == ST_SEXP &&
	  var->elems[1].type == ST_STRING)){
      say(COMPILE, "invalid variable declaration");
      *outvars = NULL;
      free(vars);
      return;
    }
    vars[i].name = var->elems[1].data.string;
    vars[i].loc = var->location;
    vars[i].type = build_type(var->elems[0].data.sexp);
  }
  *outvars = vars;
  *outsize = size;
}


static int find_local_var(struct function* f, atom name){
  for (int i=0;i<f->nformals;i++){
    if (name == f->formals[i].name)return i;
  }
  for (int i=0;i<f->nlocals;i++){
    if (name == f->locals[i].name)return i + f->nformals;
  }
  sayf(COMPILE, "unknown local variable %s of function %s", name->string, f->name->string);
  return -1;
}

static void* new_int(int x){
  int* p = malloc(sizeof(int));
  *p = x;
  return p;
}

static void compile_function(struct sexp* f, struct function* ret, ht_atom_t funcnames){
  if (!(f->elems[0].type == ST_STRING &&
	f->nelems == 3 && 
	is_sexp_with_tag(f->elems[1], S_FORMALS) &&
	is_sexp_with_tag(f->elems[2], S_LOCALS) &&
	is_sexp_with_tag(f->elems[3], S_BODY))){
    say(COMPILE, "Invalid function");
    return;
  }
  ret->name = f->elems[0].data.string;

  build_var_decls(f->elems[1].data.sexp->nelems, f->elems[1].data.sexp->elems,
		  &ret->nformals, &ret->formals);
  build_var_decls(f->elems[2].data.sexp->nelems, f->elems[2].data.sexp->elems,
		  &ret->nlocals, &ret->locals);
  
  ret->nvars = ret->nformals + ret->nlocals;
  ret->vars = malloc(sizeof(struct var_decl*) * ret->nvars);


  memcpy(ret->vars,
	 ret->locals, sizeof(struct var_decl*) * ret->nlocals);
  memcpy(ret->vars + ret->nlocals,
	 ret->formals, sizeof(struct var_decl*) * ret->nformals);
  
  ret->codelen = 20;
  int codepos = 0;
  ret->code = malloc(sizeof(instruction) * ret->codelen);

  int ninstructions = f->elems[3].data.sexp->nelems;

  /* maps labels to their position in the code */
  ht_atom_t labelpos = ht_atom_alloc();
  
  atom* labelrefs = malloc(sizeof(atom) * ninstructions);
  int curr_label = 0;
  
  struct sexp* body = f->elems[3].data.sexp;
  //FIXME: doesn't do much error-checking, will crash or silently corrupt on bad input
  for (int i=0;i<ninstructions;i++){
    struct sexp* instr = body->elems[i].data.sexp;
    switch (instr->tag){
    case S_PLUS_INT:   ret->code[codepos++] = build_instr_typed(ARITH_PLUS, PS_INT);break;
    case S_PLUS_SHORT: ret->code[codepos++] = build_instr_typed(ARITH_PLUS, PS_SHORT);break;

    case S_MINUS_INT:  ret->code[codepos++] = build_instr_typed(ARITH_MINUS, PS_INT);break;
    
    case S_TIMES_INT:  ret->code[codepos++] = build_instr_typed(ARITH_TIMES, PS_INT);break;
    
    case S_DIV_INT:    ret->code[codepos++] = build_instr_typed(ARITH_DIV, PS_INT);break;

    case S_MOD_INT:    ret->code[codepos++] = build_instr_typed(ARITH_MOD, PS_INT);break;
    
    case S_NEGATE_INT: ret->code[codepos++] = build_instr_typed(ARITH_NEGATE, PS_INT);break;

    case S_LT_INT:     ret->code[codepos++] = build_instr_typed(REL_LT, PS_INT);break;

    case S_DEREF:      ret->code[codepos++] = build_instr_untyped(PTR_DEREF);break;
    case S_ASSIGN:     ret->code[codepos++] = build_instr_untyped(PTR_ASSIGN);break;
    case S_INDEX:      ret->code[codepos++] = build_instr_untyped(PTR_INDEX);break;
    case S_OFFSET:
      ret->code[codepos++] = build_instr_untyped(PTR_OFFSET);
      ret->code[codepos++] = get_type_offset(instr->elems[0].data.string,
					     instr->elems[1].data.string);
      break;
    
    case S_CALL:       ret->code[codepos++] = build_instr_untyped(FUNC_CALL);break;


    //more interesting ones
      
    case S_LOAD_L:
      ret->code[codepos++] = build_instr_untyped(VAR_LOAD_LOCAL);
      ret->code[codepos++] = find_local_var(ret, instr->elems[0].data.string);
      break;
    case S_LOAD_F:
      ret->code[codepos++] = build_instr_untyped(VAR_LOAD_FUNC);
      ret->code[codepos++] = (instruction)(((struct function*)ht_atom_get(funcnames, instr->elems[0].data.string))->ID);
      break;
    case S_LOAD_G:
      //FIXME
      break;
    case S_CONSTANT_INT://FIXME: types
      break;


      /* Control flow:
	 On the first pass, a mapping from labels to positions in code is built.
	 Also, a list of the instructions that reference labels is built.
	 Then, when we know where each label ended up, we can go back and insert them */
    case S_LABEL:
      ht_atom_set(labelpos, instr->elems[0].data.string, new_int(codepos));
      break;
    case S_GOTO:
      ret->code[codepos++] = build_instr_untyped(GOTO_ALWAYS);
      ret->code[codepos++] = (instruction)curr_label;
      labelrefs[curr_label++] = instr->elems[0].data.string;
      break;
    case S_CONDGOTO:
      ret->code[codepos++] = build_instr_untyped(GOTO_COND);
      ret->code[codepos++] = (instruction)curr_label;
      labelrefs[curr_label++] = instr->elems[0].data.string;
      break;
    default:
      sayf(COMPILE, "Invalid sexp in function - %s", sexp_tag_to_string(instr->tag));
      break;
    }


    if (ret->codelen - codepos < 10)
      ret->code = realloc(ret->code, sizeof(instruction) * (ret->codelen *= 2));
  }
  
  //We add a sentinel instruction at the end of the stream
  //so that all instructions end in a "return"
  ret->code[codepos++] = build_instr_untyped(FUNC_RETURN_NONE);

  ret->codelen = codepos;

  //Now, we make a second pass to fix up the labels
  for (int i=0;i<ret->codelen;i++){
    opcode op = instr_opcode(ret->code[i]);
    if (op == GOTO_COND || op == GOTO_ALWAYS){
      ret->code[i] = *(int*)ht_atom_get(labelpos, labelrefs[ret->code[i+1]]);
    }
    if (takes_immediate_arg(op)){
      i++; //skip the immediate operand
    }
  }
  //free temporaries
  for (ht_atom_iter i = ht_atom_begin(labelpos); ht_atom_hasmore(i); i=ht_atom_next(i))
    free(ht_atom_getvalue(i));
  ht_atom_free(labelpos);
  free(labelrefs);
}


//FIXME: lots of memory leaks, especially in error conditions. Not enough error-checking for malformed sexps.
struct program* compile(struct sexp* code){
  if (!code->tag == S_PROGRAM){
    say(COMPILE, "sexp does not appear to be a full program");
    return NULL;
  }
  struct program* program = malloc(sizeof(struct program));
  struct sexp_element* globalvars = malloc(code->nelems * sizeof(struct sexp_element));
  struct sexp** functions = malloc(code->nelems * sizeof(struct sexp*));
  struct sexp** types = malloc(code->nelems * sizeof(struct sexp*));
  int nglobals = 0, nfunctions = 0, ntypes = 0;

  //Separate the global objects into types, global variables, and functions
  for (int i=0;i<code->nelems;i++){
    if (code->elems[i].type != ST_SEXP){
      say(COMPILE, "invalid global");
      return NULL;
    }
    struct sexp* global = code->elems[i].data.sexp;
    switch (global->tag){
    case S_STRUCT:
    case S_UNION:
      types[ntypes++] = global;
      break;
    case S_VAR:
      globalvars[nglobals].type = ST_SEXP;
      globalvars[nglobals].data.sexp = global;
      nglobals++;
      break;
    case S_FUNCTION:
      functions[nfunctions++] = global;
      break;
    default:
      sayf(COMPILE, "Invalid global '%s'", sexp_tag_to_string(global->tag));
      return NULL;
    }
  }
  
  program->typemap = build_typemap(ntypes, types);
  free(types);


  build_var_decls(nglobals, globalvars, &program->nglobals, &program->globals);


  program->nfunctions = nfunctions;
  program->functions = malloc(nfunctions * sizeof(struct function));
 
  ht_atom_t funcnames = ht_atom_alloc();
  for (int i=0;i<nfunctions;i++){
    program->functions[i].ID = i;
    ht_atom_set(funcnames, functions[i].elems[0].data.string, &program->functions[i]);
  }
  program->main_function = ht_atom_get(funcnames, atom_get("main"));
  
  for (int i=0;i<nfunctions;i++){
    compile_function(functions[i], &program->functions[i], funcnames);
  }
  ht_atom_free(funcnames);
  free(functions);
  
  return program;
}


void var_decl_dump(int nvars, struct var_decl* d){
  for (int i=0;i<nvars;i++){
    printf("  %s", get_base_type_name(d[i].type)->string);
    for (int j=0;j<get_ptr_levels(d[i].type);j++){
      printf("*");
    }
    printf(" %s declared in " LOC_FMT "\n",
	   d[i].name->string, LOC_ARGS(d[i].loc));
  }
}


void function_dump(struct function* f){
  printf("Function <%s>:\n", f->name->string);
  printf(" Arguments:\n");
  var_decl_dump(f->nformals, f->formals);
  printf(" Local variables:\n");
  var_decl_dump(f->nlocals, f->locals);
  printf(" Code:\n");
  for (int i=0;i<f->codelen;i++){
    printf("  %04d: ", i);
    switch (instr_opcode(f->code[i])){
    case GOTO_ALWAYS:printf("GOTO_ALWAYS");break;
    case GOTO_COND:printf("GOTO_COND");break;
    default:
      printf("%04x",f->code[i]);
    }
    if (takes_immediate_arg(f->code[i])){
      printf(" %d", f->code[i+1]);
      i++;
    }
    printf("\n");
  }
}

void program_dump(struct program* p){
  printf("Global variables:\n");
  var_decl_dump(p->nglobals, p->globals);
  for (int i=0;i<p->nfunctions;i++)
    function_dump(&p->functions[i]);
}
