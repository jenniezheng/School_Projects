
type ('nonterminal, 'terminal) symbol =
  | N of 'nonterminal
  | T of 'terminal


(* convert_grammar *
 * Takes two arguments: the rule list and a nonterminal. Recursively iterates through
 * input rule list to return any rules that correspond to the non-
 * terminal. When called with a rule list but no non-terminal, this
 function converts a rule list to a function according to hw2 specs . *)

let convert_grammar gram_list = let rec grammer_list_to_function rules symbol=
    match rules with
    | [] -> []
    | (sym, rhs)::rest ->
    if(sym==symbol) then
        rhs::(grammer_list_to_function rest symbol)
    else grammer_list_to_function rest symbol in

    match gram_list with
    | (symbol, grammer_list)  -> (symbol, (grammer_list_to_function grammer_list));;


(* convert_grammar *
 * Finds the first valid prefix when given a grammar, an acceptor, and a fragment . *)
let parse_prefix grammar acceptor fragment =
    let rec match_single_rule rules rule acceptor derivation fragment =
         (*Rule fully matched. Try acceptor.*)
        if ((List.length rule) = 0)
        then (acceptor derivation fragment)
        else match fragment with
        (*Fragment is empty. Rule cannot be fully matched.*)
        | [] -> None
        (*Try to match.*)
        | frag_head::frag_tail -> match rule with
            (*Should never get here.*)
            | [] -> None
            (*Try to match terminal.*)
            | (T ter)::rule_tail ->
            if (ter = frag_head) then (match_single_rule rules rule_tail acceptor derivation frag_tail)
            else None
            (*Try to match nonterminal.*)
            | (N non_ter)::rule_tail ->
            let custom_acceptor = match_single_rule rules rule_tail acceptor and rhs=rules non_ter in
            (match_multiple_rules non_ter rules rhs custom_acceptor derivation fragment)

    and match_multiple_rules start_symbol rules remaining_rules_with_start_symbol acceptor derivation fragment =
        match remaining_rules_with_start_symbol with
            (* No rules apply so None *)
            | [] -> None
            (* Try each rule *)
            | current_rule::rule_tail ->
            let new_deriv= (derivation @ [start_symbol, current_rule]) in
            let possibility = (match_single_rule rules current_rule acceptor new_deriv fragment) in
            match possibility with
                (* Rule didn't work *)
                | None -> (match_multiple_rules start_symbol rules rule_tail acceptor derivation fragment)
                (* Rule worked *)
                | something -> something in
    match grammar with
    | (start_symbol, rules) ->
    match_multiple_rules start_symbol rules (rules start_symbol) acceptor [] fragment;;



