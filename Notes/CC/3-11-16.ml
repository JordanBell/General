open Parser_types
open Printf

(*****      Lecture Notes

  Interpretation and Compilation
------
We have done evaluation already. However, this is different from interpretation.
Evaluation runs on OCaml, but compilers/interpreters must not live within another language. It must exist independently, especially when a big language like OCaml compilers a simple language like out own.

  But how?

Let's take our evaluation algorithm, and look at it in laymen's terms.
For example, for evaluating operators, we break it down to these intuitive steps

Operator:
  1. Evaluate first arg
  2. Store the result of 1
  3. Evaluate the second arg
  4. Store the result of 3
  5. Apply the operator to the store results
  6. Return the results

Identifier:
  1. Look it up in the environment

Constant:
  1. Return the value

Let:
  1. Evaluate the definition
  2. Store the result
  3. Update the environment
  4. Evaluate the body
  5. Return the result

The environment is Compiler-time, not Run-time
We want to take the above algorithms, but without the ocaml infrastructure.

Imagine that we're driving a simple machine with a defined instruction set.
Assume that the machine is given, and an instruction set that is given.
They can be given in two ways:
  1. Get an actual description of a machine and its instruction code, ie the x86 or ARM (may have lots of details)
  2. Idealised Instruction sets

Idealised Instruction Set:
  - Sequence of instructions acting on memory
  - Registers:
    - RAM: nat-indexed registers (slower)
  - Arithmetic-logic operations (data)
  - Test and jump (instruction pointer)
  - Push and Pop (stack pointer)
  - Call and return (IP and SP)


  A Tiny Instruction Set
add r5, r8  // Add the data at both registers to the accumulator
st  r8      // Store the accumulator's value into the register
ld  34      // Load the value into the accumulator

In OCaml:*)

(* Configuration *)
open Hashtbl
let ram = Hashtbl.create 100
let acc = ref 0

(* Instruction execution *)
let op (op, addr1, addr2) =
  acc := op (find ram addr1) (find ram addr2)
let st addr = replace ram addr !acc
let ldc n = acc := n

(*

This can now work with the outlined algorithms for evaluation.

The following is the interpreter version of the evaluator. This will do the same, using the tiny instruction set.

*)

(* symt = symbol table *)
let rec interpret symt = function
  | Operator (opcode, e, f) ->
    let addr1 = interpret symt e in
    let addr2 = interpret symt f in
    op (fun_of_op opcode, addr1, addr2);
    let addr3 = new_addr () in
    st addr3
  | Identifier (id_str) -> lookup id_str symt
  | Const_int n ->
    let addr = new_addr() in
    ldc n;
    st addr;
    addr;
  | Let (id_str, e, f) ->
    let addr1 = interpret symt e in
    interpret ((x, addr1) :: symt) f

(*...*)

let _ = addr_base := 0;
        let addr = interpret [] let_x in (* Returns the address of the result *)
        find ram addr;; (* Print the value at that address *)

(*

Think of the interpretation as a simulation of how the code will run.
Now we need to generate the code

  Code Generation
Take the interpreter code, but whenever we come across executing one of the instructions, do not evaluate it, just print the command to a string buffer. This will be the generated code.

Create a string buffer to hold the generated code.

*)

let code = Buffer.create 100
let codegen_op *op, addr1, addr2) =
  (string_of_operator op)
  ^ " r" ^ (string_of_int addr1)
  ^ ", r" (string_of_int addr2)
  ^ "\n" |> Buffer.add_string code
(* And do codegen_ functions for other operations, st and ld *)


(*

Now copy the code from interpret and, when coming across an op/st/ld instruction call, instead call the codegen_ function for that operation.
When we come across a recursive "interpret" call, recurse into the "codegen" call with the same arguments.

A code generator is a lazy version of an interpreter. Instead of actually doing it, it simply says, "You should do this" into the string buffer. Essentially generates procedural instructions for execution later.

Note: Use Monads to reduce the code repetition (if you want to be fancy; not necessary)


OPTIMISATIONS
This way is inefficient: We use too many registers. At the moment, it uses a new address for each thing
Now we'll need to do memory management.
  - Garbage collectors (not advised for us)
` - Stack discipline
    When we perform an operation, do not store it in a new register: Store it in the register of the first operand's register

Base_addr: Everything below is old, everything above is new
new_ref() now returns a reference above the addr_base


Assignment:
Implement SIP (Simple imperative programming language)
Let
Local Veriables
No functions (optional for 9 or 10)
If and While require a JMPZ (jump if zero) intruction (or variations thereof)

if (a) { b } else { c }
  becomes

jmpz (a) b

c: ...
  jump d
b: ...
d: ...


Mind the interaction between stack and store.
  If stack has too many elements, this is stack overflow. Better to crash than to produce wrong result. Check for test in simulation
  Pointers onto the stack? Pointers dangle if the stack get deallocated.

For function calls:
  Put address of arguments on the stack
  Put the current instruction pointer onto stack
  Jump to location of function (function assumes the arguments are on the stack)
  Place result in accumulator
  Remove arguments from stack
  Jump back using old IP on the stack

May (carefully) add new instructions
  Must be simple variations on existing instructions


Are we expected to implement input/output?
How much is SIP

*)
