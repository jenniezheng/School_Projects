
let () = print_string "Starting\n";;


type awksub_nonterminals =
  | Expr | Term | Lvalue | Incrop | Binop | Num


let awkish_grammar =
  (Expr,
   function
     | Expr ->
         [[N Term; N Binop; N Expr];
          [N Term]]
     | Term ->
     [[N Num];
      [N Lvalue];
      [N Incrop; N Lvalue];
      [N Lvalue; N Incrop];
      [T"("; N Expr; T")"]]
     | Lvalue ->
     [[T"$"; N Expr]]
     | Incrop ->
     [[T"++"];
      [T"--"]]
     | Binop ->
     [[T"+"];
      [T"-"];
      [T"*"];
      [T"/"]]
     | Num ->
     [[T"0"]; [T"1"]; [T"2"]; [T"3"]; [T"4"];
      [T"5"]; [T"6"]; [T"7"]; [T"8"]; [T"9"]])

let dyck_grammar =
  (Expr,
   function
    | Expr ->
         [ [T"(";T")"]; [T"(";N Expr;T")"]; [T"(";T")"; N Expr]; [T"(";N Expr; T")"; N Expr];]
    | _ -> [])


let printSymbol symbol = match symbol with
    | Expr ->  print_string "Expr"
    | Incrop -> print_string "Incrop"
    | Lvalue -> print_string "Lvalue"
    | Num -> print_string "Num"
    | Term -> print_string "Term"
    | Binop -> print_string "Binop"

let printSymbolWithN symbol =
    (Printf.printf "N "; printSymbol symbol)


let rec print_symbol_arr symArr = match symArr with
    | [] ->  print_string " "
    | [(N s)] -> (printSymbolWithN s)
    | (N s)::t -> (printSymbolWithN s; Printf.printf "; "; print_symbol_arr t)
    | [(T s)] -> (Printf.printf "T \"%s\"" s)
    | (T s)::t -> (Printf.printf "T \"%s\"; " s; print_symbol_arr t);;


let rec print_symbol_list_list symArrList = match symArrList with
    | [] ->  print_string "\n"
    | h::t -> (
        print_string "[";
    print_symbol_arr h;
    print_string "], ";
    print_symbol_list_list t
)



let rec print_grammer mygrammer = match mygrammer with
    | [] -> print_string ""
    | (left,righthandside)::tail ->
        (   print_string "(";
            printSymbol left;
            print_string ", [";
            print_symbol_arr righthandside;
            print_string "]);\n";
            (print_grammer tail));;


let rec print_string_list strlist = match strlist with
    | [] -> ()
    | h::t -> (print_string h; print_string " "; print_string_list t);;

let rec print_derivation deriv = match deriv with
    | None -> print_string "None\n"
    | Some(a,b) -> (print_string "[";
        print_grammer a;
        print_string "]";
        print_string "remainder: ";
    print_string_list b;);;



let accept_all derivation string = Some (derivation, string)


let test_1 =
  let deriv = (parse_prefix awkish_grammar accept_all ["2"; "+";"2";"/"; "$"; "1"; "*";"0";"++"])
  in deriv = Some
    ([(Expr, [N Term; N Binop; N Expr]);
(Term, [N Num]);
(Num, [T "2"]);
(Binop, [T "+"]);
(Expr, [N Term; N Binop; N Expr]);
(Term, [N Num]);
(Num, [T "2"]);
(Binop, [T "/"]);
(Expr, [N Term; N Binop; N Expr]);
(Term, [N Lvalue]);
(Lvalue, [T "$"; N Expr]);
(Expr, [N Term]);
(Term, [N Num]);
(Num, [T "1"]);
(Binop, [T "*"]);
(Expr, [N Term]);
(Term, [N Num]);
(Num, [T "0"]);
], ["++"])

let test_2 =
  let deriv = (parse_prefix dyck_grammar accept_all ["(";"(";"("; ")"; "("; "("; ")"; ")"; ")";"("; ")";"("; ")"; ")" ]) in
  let () = print_derivation deriv in
  deriv = Some
    ([(Expr, [T "("; N Expr; T ")"]);
(Expr, [T "("; N Expr; T ")"; N Expr]);
(Expr, [T "("; T ")"; N Expr]);
(Expr, [T "("; N Expr; T ")"]);
(Expr, [T "("; T ")"]);
(Expr, [T "("; T ")"; N Expr]);
(Expr, [T "("; T ")"]);
], [])


let print_bool b = match b with
    | true -> print_string "passed\n"
    | false -> print_string "failed\n"
let ()= print_bool test_1
let ()= print_bool test_2