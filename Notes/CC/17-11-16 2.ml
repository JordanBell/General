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
  | If of (expr * expr * expr)
  | Plus of extr * expr
  | Seq of expr * expr
  | Asg of string * expr
  | Id of string (* For simplicity here *)
  | Const of int
  | Hole (* Placeholder for a term which we will see used later *)

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
