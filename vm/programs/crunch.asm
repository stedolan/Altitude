Global variables:
Function <main>:
 Arguments:
 Local variables:
  signed int ret declared in @"crunch.c":1:68
  signed int i declared in @"crunch.c":1:68
  signed int j declared in @"crunch.c":1:68
  signed int j___0 declared in @"crunch.c":1:68
 Constants:
  0: signed int =  0
  1: signed int =  0
  2: signed int =  1000
  3: signed int =  0
  4: signed int =  30
  5: signed int =  1
  6: signed int =  1
  7: signed int =  30
  8: signed int =  1
  9: signed int =  1
 Code:
                                  0000: VAR_LOAD_LOCAL 0
                                  0002: VAR_LOAD_CONSTANT 0
  @"crunch.c":2:82                0004: PTR_ASSIGN
                                  0005: VAR_LOAD_LOCAL 1
                                  0007: VAR_LOAD_CONSTANT 1
  @"crunch.c":4:103               0009: PTR_ASSIGN
                                  0010: VAR_LOAD_LOCAL 1
                                  0012: PTR_DEREF
                                  0013: VAR_LOAD_CONSTANT 2
                                  0015: REL_LT[PS_INT]
                                  0016: GOTO_COND 6 (->22)
  @"crunch.c":4:98                0018: GOTO_ALWAYS 91 (->109)
                                  0020: GOTO_ALWAYS 2 (->22)
                                  0022: VAR_LOAD_LOCAL 2
                                  0024: VAR_LOAD_CONSTANT 3
  @"crunch.c":5:133               0026: PTR_ASSIGN
                                  0027: VAR_LOAD_LOCAL 2
                                  0029: PTR_DEREF
                                  0030: VAR_LOAD_LOCAL 1
                                  0032: PTR_DEREF
                                  0033: REL_LT[PS_INT]
                                  0034: GOTO_COND 6 (->40)
  @"crunch.c":5:128               0036: GOTO_ALWAYS 24 (->60)
                                  0038: GOTO_ALWAYS 2 (->40)
                                  0040: VAR_LOAD_LOCAL 0
                                  0042: VAR_LOAD_LOCAL 0
                                  0044: PTR_DEREF
                                  0045: VAR_LOAD_CONSTANT 4
                                  0047: ARITH_PLUS[PS_INT]
  @"crunch.c":6:157               0048: PTR_ASSIGN
                                  0049: VAR_LOAD_LOCAL 2
                                  0051: VAR_LOAD_LOCAL 2
                                  0053: PTR_DEREF
                                  0054: VAR_LOAD_CONSTANT 5
                                  0056: ARITH_PLUS[PS_INT]
  @"crunch.c":5:133               0057: PTR_ASSIGN
                                  0058: GOTO_ALWAYS 65505 (->27)
                                  0060: VAR_LOAD_LOCAL 3
                                  0062: VAR_LOAD_CONSTANT 6
  @"crunch.c":8:183               0064: PTR_ASSIGN
                                  0065: VAR_LOAD_LOCAL 3
                                  0067: PTR_DEREF
                                  0068: VAR_LOAD_LOCAL 1
                                  0070: PTR_DEREF
                                  0071: REL_LT[PS_INT]
                                  0072: GOTO_COND 6 (->78)
  @"crunch.c":8:178               0074: GOTO_ALWAYS 24 (->98)
                                  0076: GOTO_ALWAYS 2 (->78)
                                  0078: VAR_LOAD_LOCAL 0
                                  0080: VAR_LOAD_LOCAL 0
                                  0082: PTR_DEREF
                                  0083: VAR_LOAD_CONSTANT 7
                                  0085: ARITH_MINUS[PS_INT]
  @"crunch.c":9:207               0086: PTR_ASSIGN
                                  0087: VAR_LOAD_LOCAL 3
                                  0089: VAR_LOAD_LOCAL 3
                                  0091: PTR_DEREF
                                  0092: VAR_LOAD_CONSTANT 8
                                  0094: ARITH_PLUS[PS_INT]
  @"crunch.c":8:183               0095: PTR_ASSIGN
                                  0096: GOTO_ALWAYS 65505 (->65)
                                  0098: VAR_LOAD_LOCAL 1
                                  0100: VAR_LOAD_LOCAL 1
                                  0102: PTR_DEREF
                                  0103: VAR_LOAD_CONSTANT 9
                                  0105: ARITH_PLUS[PS_INT]
  @"crunch.c":4:103               0106: PTR_ASSIGN
                                  0107: GOTO_ALWAYS 65439 (->10)
                                  0109: VAR_LOAD_LOCAL 0
                                  0111: PTR_DEREF
  @"crunch.c":12:230              0112: FUNC_RETURN
                                  0113: FUNC_RETURN_NONE
