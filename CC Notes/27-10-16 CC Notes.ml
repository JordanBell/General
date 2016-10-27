
(* CONSTANT FOLDING *)
(* Note: The follow code is done before evaluation, and simplifies the parsed expression tree before evaluation (this is the precomputation) *)
| Op(opcode, Const (i), Const (j)) -> Const (op_fun i j)
| Op(opcode, e, e) -> (* Do as normal *)

| If(Const (b), e, f) -> if b then fold e else fold f

(* Similarly, while or if with constant values can have their expressions inferred *)

(* CONSTANT PROPOGATION

- Involves variables which have known values
- It builds on top of constant folding

Example:
let x = 10 in
let y = 20 in
x * y

Becomes
10 * 20
becomes
200
*)

(* FUNCTION INLINING

- Involves functions with known arguments
- Builds on the above two concepts
- Substitutes a function call with an inline calculation if the arguments are known.

Example:
foo(x, y) { x * y }

main()
{
  foo(5, 10);
}

Becomes:
main()
{
  5 * 10
  // Becomes
  500
}

WE CAN STILL INLINE for non-constant values
But you need to be careful
  let f x = x + x in f (g y)
Does it become
  (g y) + (g y)?

No! Only in some cases. f might access global variables for example, and will do something different on the second call of (g y)

*)

(* STATE AND PROPOGATION
Example (for non-concurrent code)
  x := 1; y := !x + 1
becomes
  x := 1; y := 4
Because nothing happens between the declaration of x and its use for calculating y
*)

(* Misc intuitive optimisation
  let x = a + (b * c) + d in
  let y = e + (b * c) + f
becomes
  let btc = b * c in
  let x = a + btc + d in
  let y = e + btc + f
*)

(* Languages prefer immutable identifiers because it allows for complex optimisation. *)

(* LOOP UNROLLING
  for (int i = 0; i < 5; i++)
  {
    foo(i);
  }
becomes
  foo(1); foo(2); foo(3); foo(4); foo(5)
*)

(* EXERCISE
Implement front-end optimisations (using the evaluator)

Basic: Constant folding, constant inlining
Medium: Constant propagation + loop unrolling
Advanced: Anything else

- Create 10 new test cases to illustrate best optimisations
- Make optimisations optimal with a -o argument for the parser
- Compare output
- Advanced: Count evaluation steps (define a monad)

It would be great if it could say, "Without opimisation, 10 steps. With, only 3 steps. Improvement!"
*)
