(*
Control structures (See CK machines in Hayo's lectures)

Using control structures is different from when we used evaluation to generate code. It's just a different analogy

How to relate control structures to an evaluator:

In eval:
If x then y else z
Here, we control which part executes
To get from here (eval) to code generation, there's a bit of a gap (unlike when generating things like add, sub, etc.)
Small-step evaluation/semantics gives us a better feel of the step by step developments of the program
This relates to the abstract eval in the sense that this is like a tail-recursive version. Useful for nested expressions. Some are post-posed, and bloat the calling stack for example:
  e + f: Compute e (save on calling stack) compute f. This bloats the stack with everything within e
Frame stack will have post-poned computations


*)

type expr =
  | Block of (string * expr)
  | Break of (string * expr)
  | Brkz of (string * string) (* Break out of (second string label) and return value of (second string variable name) is zero *)
  | If of (expr * expr * expr) (* Mutually exclusive with Brkz *)
  | Plus of extr * expr
  | Seq of expr * expr
  | Asg of string * expr
  | Id of string (* For simplicity here *)
  | Const of int
  | Hole (* Placeholder for a term which we will see used later *)

(* if e1 then e2 else e3 =
  _x :=e1;
  blk l2 {
    blk l1 {
    ifz x brk l1;
    brk l2 with e2 };
  e3} *)

let lte (e1, e2, e3) =
  let x = getsym () in
  let l1 = getsym () in
  let l2 = getsym () in
  [...]

let rec discard l = function
  | [] -> failwith "discard"
  | Block (l', _) :: fs when l = l' -> fs (* When breaking out of a block with the same label *)
  | Block (_, _)  :: fs
  | Block (_, _)  :: fs
  | Block (_, _)  :: fs
  | Block (_, _)  :: fs
  | Block (_, _)  :: fs (* Etc. *)
  | Block (_, _)  :: fs -> discard l fs

let rec interpret frames e =
  match e with
    (* Done *)
    | Const n when frames = [] -> n
    (* Values *)
    | Block (_, Const n) -> interpret frames (Const n)
    | Break (l, e) -> interpret (discard l frames) e
    | Plus (Const n1, Const n2) -> interpret frames (Const (n1 + n2))
    | Seq (Const n1, Const n2) -> interpret frames (Const n2)
    | Asg (v, Const n) -> replace store v n; interpret frames (Const n)
    | Id v -> interpret frames (Const (find store v))
    (* Expressions *)
    | Block (l, e) -> interpret ((Block (l, Hole)) :: frames) e
    | Brkz (l, v) ->
      let frames =
        if  find store v = 0 (* If action NOT on code! *)
        then discard l frames
        else frames in
      interpret frames (Const 0) (* Always return the value of 0 *)
    | If (e1, e2, e3) -> interpret ((If (Hole, e2, e3)) :: frames) e1
    | Seq (e1, e2) -> interpret ((Seq(Hole, e2)) ::  frames) e1
    | Plus (e1, e2) -> interpret ((Plus(Hole, e2)) ::  frames) e1
    | Asg (v, e) -> interpret ((Asg(v, Hole)) ::  frames) e
    (* "Continuations" - Check for holes in hd of frames *)
    | Const n -> (match (List.hd frames) with (* Place Const n into any existing holes on the top of the frames stack *)
      | Block (l, Hole) -> interpret (List.tl frames) Block(l, Const n)
      | If (Hole, e2, e3) when n = 0 -> interpret (List.tl frames) e2 (* Only do this if Const n (the expression) is false, else do e1 *)
      | If (Hole, e2, e3) -> interpret (List.tl frames) e1 (* When n != 0 *)
      | Seq (Hole, e) -> interpret ((Seq (Const n, Hole)) :: (List.tl frames)) e
      | Plus (Hole, e) -> interpret ((Plus (Const n, Hole)) :: (List.tl frames)) e
      | Seq (Const n', Hole) -> [...]
      | Plus (Const n', Hole) -> [...]
      [...]
    )
    | Hole -> failwith "Interpretation error"

(*
Assignment:

Build on previous assignment. NO NEED TO USE ABOVE CODE

If then else
While
Simple control would be nice
In terms of Function calls:
  CK would be too hard
  Just do simple c-style function (Low-order function)
    In x86 you have call statement
    Simply put them all on the stack
    say call and it will read from stack in order
    ret at end
*)
