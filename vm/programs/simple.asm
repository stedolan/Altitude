Global variables:
Function <sum>:
 Arguments:
  signed int x declared in @"simple.c":10:91
 Local variables:
  struct somestruct s declared in @"simple.c":10:91
  signed int i declared in @"simple.c":10:91
 Constants:
  0: signed int =  0
  1: signed int =  0
  2: signed int =  1
 Code:
                                  0000: VAR_LOAD_LOCAL 1
                                  0002: PTR_OFFSET 2
                                  0004: PTR_OFFSET 1
                                  0006: VAR_LOAD_CONSTANT 0
  @"simple.c":12:132              0008: PTR_ASSIGN
                                  0009: VAR_LOAD_LOCAL 2
                                  0011: VAR_LOAD_CONSTANT 1
  @"simple.c":13:158              0013: PTR_ASSIGN
                                  0014: VAR_LOAD_LOCAL 2
                                  0016: PTR_DEREF
                                  0017: VAR_LOAD_LOCAL 0
                                  0019: PTR_DEREF
                                  0020: REL_LT[PS_INT]
                                  0021: GOTO_COND 6 (->27)
  @"simple.c":13:153              0023: GOTO_ALWAYS 33 (->56)
                                  0025: GOTO_ALWAYS 2 (->27)
                                  0027: VAR_LOAD_LOCAL 1
                                  0029: PTR_OFFSET 2
                                  0031: PTR_OFFSET 1
                                  0033: VAR_LOAD_LOCAL 1
                                  0035: PTR_OFFSET 2
                                  0037: PTR_OFFSET 1
                                  0039: PTR_DEREF
                                  0040: VAR_LOAD_LOCAL 2
                                  0042: PTR_DEREF
                                  0043: ARITH_PLUS[PS_INT]
  @"simple.c":14:180              0044: PTR_ASSIGN
                                  0045: VAR_LOAD_LOCAL 2
                                  0047: VAR_LOAD_LOCAL 2
                                  0049: PTR_DEREF
                                  0050: VAR_LOAD_CONSTANT 2
                                  0052: ARITH_PLUS[PS_INT]
  @"simple.c":13:158              0053: PTR_ASSIGN
                                  0054: GOTO_ALWAYS 65496 (->14)
                                  0056: VAR_LOAD_LOCAL 1
                                  0058: PTR_OFFSET 2
                                  0060: PTR_OFFSET 1
                                  0062: PTR_DEREF
  @"simple.c":16:206              0063: FUNC_RETURN
                                  0064: FUNC_RETURN_NONE
Function <main>:
 Arguments:
 Local variables:
  signed int tmp declared in @"simple.c":19:231
 Constants:
  0: signed int =  40
 Code:
                                  0000: VAR_LOAD_LOCAL 0
                                  0002: VAR_LOAD_CONSTANT 0
                                  0004: VAR_LOAD_FUNC 0
                                  0006: PTR_DEREF
  @"simple.c":20:245              0007: FUNC_CALL 1
                                  0009: VAR_LOAD_LOCAL 0
                                  0011: PTR_DEREF
  @"simple.c":20:245              0012: FUNC_RETURN
                                  0013: FUNC_RETURN_NONE
