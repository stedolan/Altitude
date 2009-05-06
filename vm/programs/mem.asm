Global variables:
Function <main>:
 Arguments:
 Local variables:
  signed int* array declared in @"programs/mem.c":7:143
  void* memory declared in @"programs/mem.c":7:143
 Constants:
  0: signed int =  10
  1: signed int =  20
  2: signed int =  0
  3: signed int =  15
  4: signed int =  0
 Code:
                        0000: VAR_LOAD_CONSTANT 0
                        0002: VAR_LOAD_FUNC 65535
                        0004: PTR_DEREF
  @"programs/mem.c":10:187  0005: FUNC_CALL_NONE 1
                        0007: VAR_LOAD_LOCAL 1
                        0009: VAR_LOAD_CONSTANT 1
                        0011: VAR_LOAD_FUNC 65535
                        0013: PTR_DEREF
  @"programs/mem.c":11:212  0014: FUNC_CALL 1
                        0016: VAR_LOAD_LOCAL 0
                        0018: VAR_LOAD_LOCAL 1
                        0020: PTR_DEREF
                        0021: PTR_CAST 256 5
  @"programs/mem.c":12:246  0024: PTR_ASSIGN
                        0025: VAR_LOAD_LOCAL 0
                        0027: PTR_DEREF
                        0028: VAR_LOAD_CONSTANT 2
                        0030: PTR_INDEX
                        0031: VAR_LOAD_CONSTANT 3
  @"programs/mem.c":13:270  0033: PTR_ASSIGN
                        0034: VAR_LOAD_LOCAL 0
                        0036: PTR_DEREF
                        0037: VAR_LOAD_CONSTANT 4
                        0039: PTR_INDEX
                        0040: PTR_DEREF
  @"programs/mem.c":14:287  0041: FUNC_RETURN
                        0042: FUNC_RETURN_NONE
