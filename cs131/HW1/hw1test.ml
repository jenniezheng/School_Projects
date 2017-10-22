let my_subset_test0 = subset [1] [4;3;2;1];;
let my_equal_sets_test0 = not (equal_sets [] [4;3;2;1]);;
let my_equal_sets_test1 = equal_sets [1;2] [2;2;1;2]
let my_set_union_test0 = equal_sets (set_union [1;2] [2;2;1;2]) [2;1]
let my_set_intersection_test0 = equal_sets (set_intersection [0;-1;1] [4;3;2;1]) [1]
let my_set_diff_test0 = equal_sets (set_diff [1;2;5] [1;2;3;4]) [5]
let my_computed_fixed_point_test0 =
  computed_fixed_point (=) (fun x -> x / 3) 200 = 0
let my_computed_periodic_point_test0 =
  computed_periodic_point (=) (fun x -> (x mod 5) ) 4 13 = 3
let my_while_away_test0 =
  equal_sets (while_away ((+) 5) ((>) 20) 0) [0; 5; 10; 15]
let my_rle_decode_test0 =
  equal_sets (rle_decode [2,0; 3,3]) [0; 0; 3; 3; 3]

type my_types =
  | Expr | Rvalue
  | Binop | UnaryOp | Num
  | Char
  | Non_Terminating_Trash | Non_Terminating_Trash2

let rules_1 =
   [Expr, [T"("; N Non_Terminating_Trash; T")"];
    Expr, [T"("; N Expr; T")"];
    Expr, [N Num];
    Expr, [N Expr; N Binop; N Expr];
    Expr, [N Num; N Binop; N Num];
    Expr, [N Char; N Binop; N Char];
    Expr, [N Non_Terminating_Trash2; N Binop; N Char];
    Expr, [N Num; N UnaryOp];
    UnaryOp, [T"++"];
    UnaryOp, [T"--"];
    Binop, [T"+"];
    Binop, [T"-"];
    Char, [T"a"];
    Char, [T"b"];
    Char, [T"c"];
    Char, [T"d"];
    Char, [T"e"];
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

let rules_2 =
    [Expr, [T"("; N Expr; T")"];
    Expr, [N Num];
    Expr, [N Expr; N Binop; N Expr];
    Expr, [N Num; N Binop; N Num];
    Expr, [N Char; N Binop; N Char];
    Expr, [N Num; N UnaryOp];
    UnaryOp, [T"++"];
    UnaryOp, [T"--"];
    Binop, [T"+"];
    Binop, [T"-"];
    Char, [T"a"];
    Char, [T"b"];
    Char, [T"c"];
    Char, [T"d"];
    Char, [T"e"];
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

let grammar_1 = Num, rules_1
let grammar_2 = Num, rules_2


let my_filter_blind_alleys_test0 =
  filter_blind_alleys grammar_1 = grammar_2
