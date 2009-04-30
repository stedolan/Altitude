(program 

(@"programs/mem.c":7:143 function "main"
                                  (formals)
                                  (locals (@"programs/mem.c":7:143 var (type (ptr
                                                                             (type
                                                                             (int))))
                                                                       "array")
                                          (@"programs/mem.c":7:143 var (type (ptr
                                                                             (type
                                                                             (void))))
                                                                       "memory"))
                                  (body (constant.int 10)
                                        (load_f "__altitude_malloc")
                                        (deref)
                                        (@"programs/mem.c":10:187 callnone 1)
                                        (load_l "memory")
                                        (constant.int 20)
                                        (load_f "__altitude_malloc")
                                        (deref)
                                        (@"programs/mem.c":11:212 callassign 1)
                                        (load_l "array")
                                        (load_l "memory")
                                        (deref)
                                        (ptrcast (type (ptr (type (int)))) (type (ptr
                                                                                 (type
                                                                                 (void)))))
                                        (@"programs/mem.c":12:246 assign)
                                        (load_l "array")
                                        (deref)
                                        (constant.int 0)
                                        (index)
                                        (constant.int 15)
                                        (@"programs/mem.c":13:270 assign)
                                        (load_l "array")
                                        (deref)
                                        (constant.int 0)
                                        (index)
                                        (deref)
                                        (@"programs/mem.c":14:287 return)))
)
