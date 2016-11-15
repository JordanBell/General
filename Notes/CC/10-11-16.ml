(*
Machine code generation (x86)

  We have had abstract machines so far. Now we're using real machines (x86)
  x86 is extremely fiddly.
  Some system-specific aspects make it even MORE fiddly.

There are some choices:
  LLVM (Low-level virtual machine; is actually a slightly higher level abstract machine. This then maps to truly low-level code)
  x86-64 (We will use this)

x86-64 Assembly
  64-bit
  Registers + stack
  Many of the operators have instructions (but few kinds)
  Start simple-minded
  Optimisation later
  Mac OS X flavored (it is slightly different on linux)

Registers
  General purpose registers (GPR)
    Some legacy stuff
    Some can be used in various sizes
      Prefix determines its use (AX, EAX, RAX (We will use RAX))
    Some have implicit uses, used by the processor
  Flags & Instruction pointer

Instructions
  Operations have one or two arguments
    Size given in suffix (movb, movw, movl, movg)
    If two arguments, one of them is the result destination
  Many instructions (x86 is CISC)
    data transfer: MOV, stack stuff: PUSH, conversion, arithmetic (ADD), logical (AND), string, control (JMP),
    Do not use: I/O, cache management, system calls

Syntax of assembly
  Two major styles (Intel vs. AT&T (we will use AT&T))
    Constants: $10, $0x64
    Registers: %rdi, ...
    Instructions: op src, dest (adds src to dest, stores in dest)
    Indirection: offset(%reg), ie 7(%rdi)

ABI: Interacting with the OS
  Essential for I/O
    For own functions you can use whatever you want
  Understand the calling convention:
    rid, rsi, rex, dcx, r8, r9, stack offsets (shouldn't need more than 6 arguments. If so, place extra arguments on the stack)
  Call directly into standard c functions
    get the result of the function call from the register: rax

Stack Structure [...]
  ESP: "End of Stack Pointer" register

Hacking it
  "Template"

      #include <stdio.h>
      #include <stdlib.h>

      void print(int x)
      {
        ptintf("%d\n", x);
        exit(0);
      }

      int main()
      {
        int y = 260;
        // Put my code here. Move the result into the variable y
        print(y);
        return 1;
      }

    The above code will allow us to insert some of our own assembly.
    Now run: gcc -O0 -S templ.c
    This will generate some pure assembly (-S does assembly instead of the c program, instead of executable)
    See templ.s for notes on where this assembly goes

*)

let string_of_opcode_x86 = function
  | Plus -> "ADD"
  | Minus -> "SUB"
  | Times -> "MUL"
  | Divide -> "DIV"
  | _ -> failwith ("Binary operator instruction not supported: " ^ (string_of_opcode opc))

let codegenx86_op op =
  "pop  %rax\n" ^
  "pop  %rbx\n" ^
  (string_of_opcode_x86 op) ^ " %rax, %rbx"
  "push %rbx\n" |> Buffer.add_string code

let codegenx86_let _ =
  "pop %rax\n" ^
  "pop %rbx\n" ^
  "push %rax\n"
  |> Buffer.add_string code

let rec codegenx86 symt = function
  | Operator (op, e1, e2) ->
    codegenx86 symt e1;
    codegenx86 symt e2;
    codegenx86_op op;
    sp := !sp + 1

  | Identifier (x) ->
    let addr = lookup x symt in
    codegenx86_id (addr);
    sp := !sp + 1

  | Const (n) ->
    codegenx86_st n;
    sp := !sp + 1

  | Let (x, e1, e2) ->
    codegenx86 symt e1;
    codegenx86 ((x, !sp) :: symt) e2;
    codegenx86_let ()
(*
note: nodegenx86_id addr depends on the template assembly
*)

(*

Assignment: Same as last week, but executable. Must bring laptop
Compile with
cc templ.s a.out

*)
