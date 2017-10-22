let subset a b =
    let existsInB num = List.mem num b in
    List.for_all (existsInB) a;;

let equal_sets a b =
    (subset a b) && (subset b a);;

(*
set_union psuedocode:
    match b:
    if b is empty: return a
    else if first item in b exists in a:
        recurse a b's tail
    else
        recurse a(concat b's head) b's tail
    if a > b then []
    else a :: range (a+1) b;;
*)

let rec set_union a b =
    let existsInA num = List.mem num a in
    match b with
        | [] -> a
        | (x::xs) ->
        if (existsInA x = false)
            then x :: ( set_union a xs )
            else set_union a xs
    ;;

(*
set_intersection psuedocode:
    match a
    if a is empty: return c
    else if first item in a exists in b:
        recurse a's tail c a's head::c
    else
        recurse a's tail c c
*)


let rec set_intersection_helper (a: 'a list) (b: 'a list) (c: 'a list):
    'a list =
    let existsInB num = List.mem num b in
    match a with
        | [] -> c
        | (x::xs) ->
        if (existsInB x = false)
            then ( set_intersection_helper xs b c)
            else ( set_intersection_helper xs b (x::c) )
    ;;

let set_intersection a b = set_intersection_helper a b []

let rec set_diff_helper (a: 'a list) (b: 'a list) (c: 'a list):
    'a list =
    let existsInB num = List.mem num b in
    match a with
        | [] -> c
        | (x::xs) ->
        if (existsInB x = true)
            then ( set_diff_helper xs b c)
            else ( set_diff_helper xs b (x::c) )
    ;;

let set_diff a b = set_diff_helper a b []

(*
    if x = f(x) return x
    else return computed_fixed_point rq f x
*)

 let rec computed_fixed_point
 (eq: 'a -> 'a -> bool) (f: 'a -> 'a) (x: 'a) : 'a =
    let y = f x in
    if (eq x y) then x
    else (computed_fixed_point eq f y);;


 let rec computed_periodic_point
 (eq: 'a -> 'a -> bool) (f: 'a -> 'a) (p: int) (x: 'a) : 'a =
    let y = f x in
    if (p = 0) then x
    else if ( eq x (f (computed_periodic_point eq f (p-1) y)))
        then x
    else (computed_periodic_point eq f p y);;

 let rec while_away
 (s: 'a -> 'a) (p: 'a -> bool) (x: 'a) : 'a list =
    if ((p x) = false) then []
    else x :: ( while_away s p (s x));;

(*
    if head[0]<=0:
        return rle_decode tail
    else
        return head[1] :: rle_decode (head[0]-1,head[1]) :: tail
*)
let rec rle_decode lp
     = match lp with
    | [] -> []
    | (num,str)::tail ->
        if (num <= 0) then rle_decode tail
        else str :: (rle_decode (((num-1),str) :: tail)) ;;

type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal
(*

*)


type awksub_nonterminals =
  | Expr | Lvalue | Incrop | Binop | Num

let awksub_rules =
   [Expr, [T"("; N Expr; T")"];
    Expr, [N Num];
    Expr, [N Expr; N Binop; N Expr];
    Expr, [N Lvalue];
    Expr, [N Incrop; N Lvalue];
    Expr, [N Lvalue; N Incrop];
    Lvalue, [T"$"; N Expr];
    Incrop, [T"++"];
    Incrop, [T"--"];
    Binop, [T"+"];
    Binop, [T"-"];
    Num, [T"0"];
    Num, [T"1"];
    Num, [T"2"];
    Num, [T"3"];
    Num, [T"4"];
    Num, [T"5"];
    Num, [T"6"];
    Num, [T"7"];
    Num, [T"8"];
    Num, [T"9"]]

(*
    find terminating symbols (1 iteration)
    start by calling function with full rules_list and empty symbols list
    if rules list is empty,
    return symbols list
    else if current symbol in symbols list
        call function with rules_tail and symbols list
    else if current rule has all T / N in symbols list,
        call function with rules_tail and current_symbol :: symbols_list
    else call function with rules_tail and symbols list
*)

let rec symbol_immediately_terminates symbols_list terminating_symbols
     = match symbols_list with
    | [] -> true
    | (N head)::tail ->
        if (not (List.mem head terminating_symbols))
            then false
        else
            symbol_immediately_terminates tail terminating_symbols
    | _::tail  -> symbol_immediately_terminates tail terminating_symbols;;

let rec find_terminating_symbols_iteration rules_list valid_symbols_list
    = match rules_list with
    | [] -> valid_symbols_list
    | (symbol,symbols_list)::tail ->
        if(List.mem symbol valid_symbols_list ||
            not (symbol_immediately_terminates symbols_list valid_symbols_list))
            then find_terminating_symbols_iteration tail valid_symbols_list
        else
            symbol::(find_terminating_symbols_iteration tail valid_symbols_list);;

(*
    while valid_symbols_list is increasing:
        continue calling find_terminating_symbols_iteration on the same rules_list
        and the bigger valid symbols list
        return the total valid symbols list
*)

let rec find_all_terminating_symbols_helper rules_list symbols_list
    = let new_symbols_list = find_terminating_symbols_iteration rules_list symbols_list
        in if ((List.length new_symbols_list) = (List.length symbols_list))
            then symbols_list
        else find_all_terminating_symbols_helper rules_list new_symbols_list


let find_all_terminating_symbols rules_list
    = find_all_terminating_symbols_helper rules_list []

let rec filter_blind_alleys_helper rules_list valid_symbols_list
      = match rules_list with
    | [] -> []
    | (symbol,symbols_list)::tail ->
        if (symbol_immediately_terminates (symbols_list) valid_symbols_list)
            then (symbol,symbols_list) :: filter_blind_alleys_helper tail valid_symbols_list
        else
           filter_blind_alleys_helper tail valid_symbols_list

let rec filter_blind_alleys grammar
      = match grammar with
    | (expr,rules_list) ->
        let symbols_list = find_all_terminating_symbols rules_list
            in (expr, (filter_blind_alleys_helper rules_list symbols_list));;

