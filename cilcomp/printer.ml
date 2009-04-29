open Cil
open Pretty
open Escape
module E = Errormsg
module H = Hashtbl

let freshNameID = ref Int64.zero

let rec pSexpLoc ?(sep=break) (loc: location) (tag: string) (children: doc list) = 
  let locdata =
    if loc.line == -1 then
      text "" 
    else
      text "@" ++ pStr loc.file ++ text ":" ++ num loc.line ++ text ":" ++ num loc.byte ++ break
  in
  text "(" ++ locdata ++ text tag
    ++ (match children with
      [] -> text ")"
    | _ -> sep ++ align ++ seq sep (fun x -> x) children ++ text ")" ++ unalign) 
    
and pStr (s: string) = chr '"' ++ text (escape_string s) ++ chr '"'

and pSexp ?(sep=break) (tag: string) (children: doc list) = 
  pSexpLoc ~sep:sep {line = -1; file = ""; byte = -1;} tag children

and freshName () : string = begin
  let str = Int64.to_string !freshNameID in
  freshNameID := Int64.succ !freshNameID;
  "$ALT$" ^ str
end
    


      (*** VARIABLES ***)
    (* variable declaration *)
and pVDecl () (v:varinfo) =
  let stom, rest = separateStorageModifiers v.vattr in
  (* First the storage modifiers *)
  text (if v.vinline then "__inline " else "")
    ++ d_storage () v.vstorage
    ++ (pAttrs () stom)
    ++ (pType (Some (text v.vname)) () v.vtype)
    ++ text " "
    ++ pAttrs () rest

    (*** L-VALUES ***)
and pLval () (lv:lval) =  (* lval (base is 1st field)  *)
  let rec pOffset (base:doc) = function
    | NoOffset -> base
    | Field (fi, o) -> 
	pOffset (base ++ text "." ++ text fi.fname) o
    | Index (e, o) ->
	pOffset (base ++ text "[" ++ pExp e ++ text "]") o in

  match lv with
    Var vi, o -> pOffset (text vi.vname) o
  | Mem e, Field(fi, o) ->
      pOffset
        ((pExp e) ++ text ("->" ^ fi.fname)) o
  | Mem e, NoOffset -> 
      text "*" ++ pExp e
  | Mem e, o ->
      pOffset
        (text "(*" ++ pExp e ++ text ")") o

    (*** EXPRESSIONS ***)

and psLval (lv: lval) = 
  let rec psOffset = function
    | NoOffset -> nil
    | Field (fi, o) -> 
	line ++ pSexp "offset" [pStr (compFullName fi.fcomp); pStr (fi.fname)]
	  ++ psOffset o
    | Index (exp, o) -> 
	line ++ psExp exp ++ line ++ pSexp "index" [] ++ psOffset o in
  match lv with
    Var vi, o -> 
      (if vi.vglob then
	if isFunctionType vi.vtype then
	  pSexp "load_f" [pStr vi.vname]
	else
	  pSexp "load_g" [pStr vi.vname]
      else
	pSexp "load_l" [pStr vi.vname]
	) ++ psOffset o
  | Mem e, o ->
      psExp e ++ psOffset o

and psConstExp (e: exp) : doc = 
  let err () = E.s (error "invalid integer constant expression") in
  match e with
    Const(c) -> 
      (match c with 
	CInt64 (i, kind, _) -> text (Int64.to_string i)
      | _ -> err())
  | UnOp (op, e1, typ) -> 
      pSexp (match op with
	Neg -> "negate"
      | _ -> err ())
	[psConstExp e1]
  | BinOp (op, e1, e2, typ) ->
      pSexp (match op with
	PlusA -> "plus"
      | _ -> err ())
	[psConstExp e1; psConstExp e2]
  | CastE (t,e) -> psConstExp e
  | SizeOf (t) -> pSexp "sizeof" [psType t]
  | SizeOfE (e) -> pSexp "sizeof" [psType (typeOf e)]
  | SizeOfStr (s) -> pSexp "sizeof" [pStr s]
  | AlignOf _ -> err ()
  | AlignOfE _ -> err ()
  | Lval _ -> err()
  | AddrOf _ -> err()
  | StartOf _ -> err()

and psExp (e: exp) : doc = 
  match e with
    Const(c) ->
      (match c with
      | CInt64 (i, kind, _) -> let t = [text (Int64.to_string i)] in
	(match kind with
	  IInt -> pSexp "constant.int" t
	| _ -> E.s (unimp "Unsupported constant integer type"))
      | _ -> E.s (unimp "Unsupported constant type"))
  | UnOp (op, e1, typ) -> 
      psExp (e1) ++ line ++
	pSexp (match (op,typ) with
	  (Neg, intType) -> "negate.int"
	| _ -> E.s (unimp "Unsupported unary operation")
	      ) []
  | BinOp (op, e1, e2, typ) -> 
      psExp (e1) ++ line ++ psExp (e2) ++ line ++
	pSexp (match (op, typ) with
	  (PlusA, intType) -> "plus.int"
	| (Lt, intType) -> "lt.int"
	| _ -> E.s (unimp ("Unsupported binary operation"))
	      ) []
  | CastE (typ, e1) -> 
      psExp (e1) ++ pSexp (match typ, (typeOf e1) with
	_ -> E.s (unimp "Unsupported cast")) []

  | SizeOf (t) -> E.s (unimp "sizeof")
  | SizeOfE (e) -> E.s (unimp "sizeof")
  | SizeOfStr(s) -> E.s (unimp "sizeof")
  | AlignOf (t) -> E.s (unimp "alignof")
  | AlignOfE (e) -> E.s (unimp "alignof")

  | Lval(l) -> psLval l ++ line ++ pSexp "deref" []
  | AddrOf(l) -> psLval l
  | StartOf(l) -> psLval l
	
	     
and pExp (e: exp) : doc = chr '(' ++ 
    (
     match e with
       Const(c) -> d_const () c
     | Lval(l) -> pLval () l
     | UnOp(u,e1,typ) -> 
	 
	 (d_unop () u) ++ chr ' ' ++ (pExp e1)
           
     | BinOp(b,e1,e2,typ) -> 
	 align 
           ++ (pExp e1)
           ++ chr ' ' 
           ++ (d_binop () b)
           ++ chr ' '
           ++ (pExp e2)
           ++ unalign

     | CastE(t,e) -> 
	 text "(" 
           ++ pType None () t
           ++ text ")"
           ++ pExp e

     | SizeOf (t) -> 
	 text "sizeof(" ++ pType None () t ++ chr ')'
     | SizeOfE (e) ->  
	 text "sizeof(" ++ pExp e ++ chr ')'

     | SizeOfStr s -> 
	 text "sizeof(" ++ d_const () (CStr s) ++ chr ')'

     | AlignOf (t) -> 
	 text "__alignof__(" ++ pType None () t ++ chr ')'
     | AlignOfE (e) -> 
	 text "__alignof__(" ++ pExp e ++ chr ')'
     | AddrOf(lv) -> 
	 text "&(" ++ (pLval () lv) ++ text ")"
           
     | StartOf(lv) -> pLval () lv
    ) ++ chr ')'


and pInit () = function 
    SingleInit e -> pExp e
  | CompoundInit (t, initl) -> 
      (* We do not print the type of the Compound *)
      let printDesignator = 
        if not !msvcMode then begin
          (* Print only for union when we do not initialize the first field *)
          match unrollType t, initl with
            TComp(ci, _), [(Field(f, NoOffset), _)] -> 
              if not (ci.cstruct) && ci.cfields != [] && 
                (List.hd ci.cfields) != f then
                true
              else
                false
          | _ -> false
        end else 
          false 
      in
      let d_oneInit = function
          Field(f, NoOffset), i -> 
            (if printDesignator then 
              text ("." ^ f.fname ^ " = ") 
            else nil) ++ pInit () i
        | Index(e, NoOffset), i -> 
            (if printDesignator then 
              text "[" ++ pExp e ++ text "] = " else nil) ++ 
              pInit () i
        | _ -> E.s (unimp "Trying to print malformed initializer")
      in
      chr '{' ++ (align 
                    ++ ((docList ~sep:(chr ',' ++ break) d_oneInit) () initl) 
                    ++ unalign)
        ++ chr '}'


    (*** INSTRUCTIONS ****)
and pInstr () (i:instr) =       (* imperative instruction *)
  match i with
  | Set(lv,e,l) -> 
      (pLval () lv
        ++ text " = "
        ++ pExp e
        ++ text ";")
  | Call(dest,e,args,l) ->
      ((match dest with
          None -> nil
        | Some lv -> 
            pLval () lv ++ text " = " ++ text "(" ++ pType None () (typeOfLval lv) ++ text ")")
        (* Now the function name *)
        ++ (let ed = pExp e in
        match e with 
          Lval(Var _, _) -> ed
        | _ -> text "(" ++ ed ++ text ")")
        ++ text "(" ++ 
        (align
           (* Now the arguments *)
           ++ (docList ~sep:(chr ',' ++ break) 
                 (pExp) () args)
           ++ unalign)
        ++ text (");"))

  | Asm(attrs, tmpls, outs, ins, clobs, l) ->
      E.s (E.error "Inline assembly is not supported")
        

and psLabel = function
    Label (s, loc, true) -> pSexpLoc loc "label" [pStr s]
  | Label (s, loc, false) -> pSexpLoc loc "label" [pStr ("$CIL$" ^ s)]
  | Case (e, _) -> E.s (unimp "switch statements")
  | Default _ -> E.s (unimp "switch statements")

and psBlock () (loopstart: doc option) (loopend: doc option) (blk: block) = docList ~sep:line (fun x -> psStmt () loopstart loopend x) () blk.bstmts

and psStmt () (loopstart: doc option) (loopend: doc option) (s: stmt) = 
  (docList ~sep:line psLabel () s.labels)++
  match s.skind with
    Return (None, l) -> pSexpLoc l "retnone" []
  | Return (Some e, l) -> psExp e ++ line ++  pSexpLoc l "return" []
  | Goto (sref, l) -> begin
      (* Grab one of the labels *)
      let rec pickLabel = function
          [] -> None
        | Label (l, a, b) :: _ -> Some (Label (l,a,b))
        | _ :: rest -> pickLabel rest
      in
      match pickLabel !sref.labels with
        Some l -> pSexp "goto" [psLabel l]
      | None -> 
          ignore (error "Cannot find label for target of goto\n");
          text "goto __invalid_label;"
  end
  | Break l -> (match loopend with
      None -> E.s (bug "Break outside loop")
    | Some lbl -> pSexpLoc l "goto" [lbl])
  | Continue l -> (match loopstart with
      None -> E.s (bug "Continue outside loop")
    | Some lbl -> pSexpLoc l "goto" [lbl])
  | Instr il -> docList ~sep:line (function i -> match i with
      Set (lv, exp, loc) -> 
	psLval lv ++ line ++ psExp exp ++ line ++ pSexpLoc loc "assign" []
    | Asm _ -> E.s (unimp "inline assembly")
    | Call (None, fn, args, loc) -> 
	docList ~sep:line psExp () args ++ line
	  ++ psExp fn ++ line
	  ++ pSexpLoc loc "callnone" [text (string_of_int (List.length args))]
    | Call (Some lv, fn, args, loc) -> 
	psLval lv ++ line
	  ++ docList ~sep:line psExp () args ++ line
	  ++ psExp fn ++ line
	  ++ pSexpLoc loc "callassign" [text (string_of_int (List.length args))]
      ) () il
  | If (cond, tb, fb, loc) -> 
      let truemark = pSexp "label" [pStr (freshName ())] in 
      let endmark = pSexp "label" [pStr (freshName ())] in
      psExp cond ++ line
	++ pSexp "condgoto" [truemark] ++ line
	++ psBlock () loopstart loopend fb ++ line ++ pSexp "goto" [endmark] ++ line
	++ truemark ++ line ++ psBlock () loopstart loopend tb ++ line
	++ endmark
  | Switch _ -> E.s (unimp "switch statements")
  | Loop (blk, loc, _, _) -> 
      let newloopstart = pSexp "label" [pStr ("$LOOPSTART" ^ freshName ())] in 
      let newloopend = pSexp "label" [pStr ("$LOOPEND" ^ freshName ())] in 
      newloopstart ++ line 
	++ psBlock () (Some newloopstart) (Some newloopend) blk ++ line 
	++ pSexp "goto" [newloopstart] ++ line 
	++ newloopend
  | Block (blk) -> psBlock () loopstart loopend blk
  | TryFinally _ -> E.s (unimp "try-finally")
  | TryExcept _ -> E.s (unimp "try-except")
      

	(**** STATEMENTS ****)
and pStmt () (s: stmt) = (* control-flow statement *)
  let pLabel = function
      Label (s, _, true) -> text (s ^ ": ")
    | Label (s, _, false) -> text (s ^ ": /* CIL Label */ ")
    | Case (e, _) -> text "case " ++ pExp e ++ text ": "
    | Default _ -> text "default: " in

  (* print the labels *)
  ((docList ~sep:line pLabel) () s.labels)
    (* print the statement itself. If the labels are non-empty and the
     * statement is empty, print a semicolon  *)
    ++ 
    (if s.skind = Instr [] && s.labels <> [] then
      text ";"
    else
      (if s.labels <> [] then line else nil) 
        ++ pStmtKind () s.skind)


	(* The pBlock will put the unalign itself *)
and pBlock () (blk: block) = 
  (* Let the host of the block decide on the alignment. The d_block will 
   * pop the alignment as well  *)
  text "{" 
    ++ line
    ++ docList ~sep:line (pStmt ()) () blk.bstmts
    ++ unalign ++ line ++ text "}"

and pStmtKind () = function
    Return(None, l) ->
        text "return;"

  | Return(Some e, l) ->
        text "return ("
        ++ pExp e
        ++ text ");"
        
  | Goto (sref, l) -> begin
      (* Grab one of the labels *)
      let rec pickLabel = function
          [] -> None
        | Label (l, _, _) :: _ -> Some l
        | _ :: rest -> pickLabel rest
      in
      match pickLabel !sref.labels with
        Some l -> text ("goto " ^ l ^ ";")
      | None -> 
          ignore (error "Cannot find label for target of goto\n");
          text "goto __invalid_label;"
  end

  | Break l ->
        text "break;"

  | Continue l -> 
        text "continue;"

  | Instr il ->
      align
        ++ (docList ~sep:line (fun i -> pInstr () i) () il)
        ++ unalign

  | If(be,t,e,l) ->
      align
              ++ text "if"
              ++ (align
                    ++ text " ("
                    ++ pExp be
                    ++ text ") "
                    ++ pBlock () t)
              ++ text " "   (* sm: indent next code 2 spaces (was 4) *)
              ++ (align
                    ++ text "else "
                    ++ pBlock () e)
              ++ unalign
        
  | Switch(e,b,_,l) ->
        (align
              ++ text "switch ("
              ++ pExp e
              ++ text ") "
              ++ pBlock () b)
  | Loop(b, l, _, _) -> begin
        text "wh" ++ align ++ text "ile (1) " ++ pBlock () b
  end
  | Block b -> align ++ pBlock () b
	
  | TryFinally (b, h, l) -> E.s (E.error "TryFinally is not supported")
  | TryExcept (b, (il, e), h, l) -> E.s (E.error "TryExcept is not supported")



and dsGlobal (out: out_channel) (g: global) : unit =
  let psVarDecl l v =
    pSexpLoc l "var" [psType v.vtype; pStr v.vname] in
  fprint out !lineLength ((match g with
    GFun (fdec, l) -> pSexpLoc ~sep:line l "function" [
      pStr fdec.svar.vname;
      pSexp "formals" (List.map (psVarDecl l) fdec.sformals);
      pSexp "locals" (List.map (psVarDecl l) fdec.slocals);
      pSexp "body" [psBlock () None None fdec.sbody]
    ]
  | GVar (vi, io, l) -> 
      (* FIXME: initialisers for globals *)
      psVarDecl l vi


  | GCompTag (comp, loc) -> pSexpLoc loc 
	(if comp.cstruct then "struct" else "union")
	[pStr (Cil.compFullName comp);
	 pSexp ~sep:line "fields" (List.map 
	   (fun f -> pSexp "field" 
	       [psType f.ftype; pStr f.fname]) comp.cfields)]

  | GEnumTag (_,_) -> E.s (unimp "enums")

  | GType (_,_) -> E.s (unimp "typedef")

  | GCompTagDecl (_,_) -> nil
  | GEnumTagDecl (_,_) -> nil

  | GVarDecl (_,_) -> nil (* FIXME: linkage??? E.s (unimp "external var linkage") *)

  | GAsm (_,_) -> E.s (unimp "inline assembly")
  | GPragma (_,_) -> E.s (unimp "pragmas")
  | GText _ -> E.s (unimp "GText")
	) ++ line)
	
	
	(*** GLOBALS ***)

and dGlobal (out: out_channel) (g: global) : unit = 
  fprint out !lineLength (match g with 
    GFun (fdec, l) -> 
      pVDecl () fdec.svar
	++  line
	++ text "{ "
	++ (align
              (* locals. *)
              ++ (docList ~sep:line (fun vi -> pVDecl () vi ++ text ";") 
                    () fdec.slocals)
              ++ line ++ line
              (* the body *)
              ++ pBlock () fdec.sbody)
	++ line
	++ text "}\n"

  | GVar (vi, io, l) ->
        pVDecl () vi
        ++ chr ' '
        ++ (match io.init with
          None -> nil
        | Some i -> text " = " ++ 
              (let islong = 
                match i with
                  CompoundInit (_, il) when List.length il >= 8 -> true
                | _ -> false 
              in
              if islong then 
                line ++ text "  " 
              else nil) ++
              (pInit () i))
        ++ text ";\n"


  | GType (typ, l) ->
        text "typedef "
        ++ (pType (Some (text typ.tname)) () typ.ttype)
        ++ text ";\n"

  | GEnumTag (enum, l) ->
        text "enum" ++ align ++ text (" " ^ enum.ename) ++
        text " {" ++ line
        ++ (docList ~sep:(chr ',' ++ line)
              (fun (n,i, loc) -> 
                text (n ^ " = ") 
                  ++ pExp i)
              () enum.eitems)
        ++ unalign ++ line ++ text "} " 
        ++ pAttrs () enum.eattr ++ text";\n"

  | GEnumTagDecl (enum, l) -> (* This is a declaration of a tag *)
        text ("enum " ^ enum.ename ^ ";\n")

  | GCompTag (comp, l) -> (* This is a definition of a tag *)
      let n = comp.cname in
      let su, su1, su2 =
        if comp.cstruct then "struct", "str", "uct"
        else "union",  "uni", "on"
      in
      let sto_mod, rest_attr = separateStorageModifiers comp.cattr in
        text su1 ++ (align ++ text su2 ++ chr ' ' ++ (pAttrs () sto_mod)
                       ++ text n
                       ++ text " {" ++ line
                       ++ ((docList ~sep:line (pFieldDecl ())) () 
                             comp.cfields)
                       ++ unalign)
        ++ line ++ text "}" ++
        (pAttrs () rest_attr) ++ text ";\n"

  | GCompTagDecl (comp, l) -> (* This is a declaration of a tag *)
        text (compFullName comp) ++ text ";\n"

	
	(* print global variable 'extern' declarations, and function prototypes *)    
  | GVarDecl (vi, l) ->
      if not !printCilAsIs && H.mem builtinFunctions vi.vname then begin
        (* Compiler builtins need no prototypes. Just print them in
           comments. *)
        text "/* compiler builtin: \n   " ++
          (pVDecl () vi)
          ++ text ";  */\n"
          
      end else
          (pVDecl () vi)
          ++ text ";\n"

  | GAsm (s, l) -> E.s (E.error "GAsm not supported")
  | GPragma (Attr(an, args), l) -> E.s (E.error "GPragma not supported")
  | GText s  -> E.s (E.error "GText not supported")

			 )

and pFieldDecl () fi = 
  (pType
     (Some (text (if fi.fname = missingFieldName then "" else fi.fname)))
     () 
     fi.ftype)
    ++ text " "
    ++ (match fi.fbitfield with None -> nil 
    | Some i -> text ": " ++ num i ++ text " ")
    ++ pAttrs () fi.fattr
    ++ text ";"
    

    (***** PRINTING DECLARATIONS and TYPES ****)
    

and psType (t:typ) = pSexp "type" (
  match t with
    TVoid a -> [pSexp "void" []]
  | TInt (IInt, a) -> [pSexp "int" []]
  | TInt (_, a) -> E.s (unimp "unknown integer type")
  | TFloat (_, _) -> E.s (unimp "floating point")
  | TPtr (t, a) -> [pSexp "ptr" [psType t]]
  | TArray (t, Some len, a) -> [pSexp "arraylen" [psType t; psConstExp len]]
  | TArray (t, None, a) -> [pSexp "array" [psType t]]
  | TFun (ret, args, varargs, a) -> E.s (E.error "function type decls not supported")
  | TNamed (t, a) -> [pSexp "typeref" [pStr t.tname]]
  | TComp (comp, a) -> [pSexp "comptype" [pStr (Cil.compFullName comp)]]
  | TEnum (_,_) -> E.s (unimp "enum type decls")
  | TBuiltin_va_list a -> E.s (unimp "varargs")
 )

and pType (nameOpt: doc option) (* Whether we are declaring a name or 
                                 * we are just printing a type *)
    () (t:typ) =       (* use of some type *)
  let name = match nameOpt with None -> nil | Some d -> d in
  match t with 
    TVoid a ->
      text "void"
        ++ pAttrs () a 
        ++ text " " 
        ++ name

  | TInt (ikind,a) -> 
      d_ikind () ikind 
        ++ pAttrs () a 
        ++ text " "
        ++ name

  | TFloat(fkind, a) -> 
      d_fkind () fkind 
        ++ pAttrs () a 
        ++ text " " 
        ++ name

  | TComp (comp, a) -> (* A reference to a struct *)
      let su = if comp.cstruct then "struct" else "union" in
      text (su ^ " " ^ comp.cname ^ " ") 
        ++ pAttrs () a 
        ++ name
        
  | TEnum (enum, a) -> 
      text ("enum " ^ enum.ename ^ " ")
        ++ pAttrs () a 
        ++ name
  | TPtr (bt, a)  -> 
      (* Parenthesize the ( * attr name) if a pointer to a function or an 
       * array. However, on MSVC the __stdcall modifier must appear right 
       * before the pointer constructor "(__stdcall *f)". We push them into 
       * the parenthesis. *)
      let (paren: doc option), (bt': typ) = 
        match bt with 
          TFun(rt, args, isva, fa) when !msvcMode -> 
            let an, af', at = partitionAttributes ~default:AttrType fa in
            (* We take the af' and we put them into the parentheses *)
            Some (text "(" ++ pAttrs () af'), 
            TFun(rt, args, isva, addAttributes an at)

        | TFun _ | TArray _ -> Some (text "("), bt

        | _ -> None, bt
      in
      let name' = text "*" ++ pAttrs () a ++ name in
      let name'' = (* Put the parenthesis *)
        match paren with 
          Some p -> p ++ name' ++ text ")" 
        | _ -> name' 
      in
      pType 
        (Some name'')
        () 
        bt'

  | TArray (elemt, lo, a) -> 
      (* ignore the const attribute for arrays *)
      let a' = dropAttributes [ "const" ] a in 
      let name' = 
        if a' == [] then name else
        if nameOpt == None then pAttrs () a' else 
        text "(" ++ pAttrs () a' ++ name ++ text ")" 
      in
      pType 
        (Some (name'
                 ++ text "[" 
                 ++ (match lo with None -> nil | Some e -> pExp e)
                 ++ text "]"))
        ()
        elemt
        
  | TFun (restyp, args, isvararg, a) -> 
      let name' = 
        if a == [] then name else 
        if nameOpt == None then pAttrs () a else
        text "(" ++ pAttrs () a ++ name ++ text ")" 
      in
      pType 
        (Some
           (name'
              ++ text "("
              ++ (align 
                    ++ 
                    (if args = Some [] && isvararg then 
                      text "..."
                    else
                      (if args = None then nil 
                      else if args = Some [] then text "void"
                      else 
                        let pArg (aname, atype, aattr) = 
                          let stom, rest = separateStorageModifiers aattr in
                          (* First the storage modifiers *)
                          (pAttrs () stom)
                            ++ (pType (Some (text aname)) () atype)
                            ++ text " "
                            ++ pAttrs () rest
                        in
                        (docList ~sep:(chr ',' ++ break) pArg) () 
                          (argsToList args))
                        ++ (if isvararg then break ++ text ", ..." else nil))
                    ++ unalign)
              ++ text ")"))
        ()
        restyp

  | TNamed (t, a) ->
      text t.tname ++ pAttrs () a ++ text " " ++ name

  | TBuiltin_va_list a -> 
      text "__builtin_va_list"
	++ pAttrs () a 
        ++ text " " 
        ++ name


	(**** PRINTING ATTRIBUTES *********)
(* Only the C99 attributes (const, volatile, restrict) are supported by Altitude *)

and pAttr (Attr(an, args): attribute) : doc =
  (* Recognize and take care of some known cases *)
  text (match an, args with 
    "const", [] -> "const"
  | "volatile", [] -> "volatile"
  | "restrict", [] -> "__restrict"
  | _ -> E.s (E.error "Custom CIL attributes not supported"))


and pAttrs () (a: attributes) = 
  let rec loop = function
      [] -> nil
    | x :: rest -> 
        let dx = pAttr x in
        if dx == nil then
          loop rest
        else
          dx ++ text " " ++ loop rest
  in
  text " " ++ (loop a) ++ text " "
