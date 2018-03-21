let string_to_list str =
  let rec loop i limit =
    if i = limit then []
    else (String.get str i) :: (loop (i + 1) limit)
  in
  loop 0 (String.length str);;


let list_to_string s =
  let rec loop s n =
    match s with
      [] -> String.make n '?'
    | car :: cdr ->
       let result = loop cdr (n + 1) in
       Bytes.set result n car;
       result
  in
  loop s 0;;

let rec read_lines () =
try let line = read_line () in
    line :: read_lines()
with
    End_of_file -> []


let rotateArr arr =
    match arr with
    | h::t -> t@[h]

let rec rotate_many_times arr times =
    match times with
    | 0 -> Printf.printf "\n"
    | _ -> let newArr = (rotateArr arr ) in
    let () = Printf.printf "%s " (list_to_string newArr) in
    (rotate_many_times newArr (times-1))


let rec process arr =
    match arr with
    | [] -> ()
    | h::t -> let () = (rotate_many_times (string_to_list h) (String.length h)) in
    process t

let () =
    let n::arr = read_lines()
in process arr;