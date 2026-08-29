# Atom1.1-PL
1.1
i just added to some commands numbers and else, well, i'm too lazy, so here is the translated document:
Technical Specification and Documentation of the Atom Programming Language (v1.0)

This document serves as the basic guide and architectural specification for the Atom language — an ultra-minimalistic low-level language designed for use within its own micro-OS. The architecture combines principles of a linear pipeline and stack-based data processing (in the spirit of Forth concepts), ensuring maximum execution speed and minimal hardware resource requirements.

1. How the Language Works

The Atom language is built on the concept of a single data stack and linear sequential instruction execution.

Stack Model: All operands and computation results pass through a global data stack. A command can take values from the stack, perform an operation on them, and push the result back.

Linear Pipeline: The program executes instruction by instruction from left to right, without complex tree-like nesting or syntactic constructs like curly braces or semicolons.

Atomicity: Each command is encoded by a single Latin letter (from A to Z), making parsing extremely fast and suitable for bare-metal operation.

2. Commands (Alphabet from A to Z)

The complete language alphabet consists of 26 basic instructions, each responsible for a specific low-level operation:

A (Allocate): Allocates a block of dynamic memory in RAM (heap). Takes size from the stack, returns the address of the allocated buffer.

B (Branch / Conditions): Controls execution flow through modifiers:

B1 — Start of condition (if then)

B2 — Alternative branch (else)

B3 — End of conditional block (end if)

B4 — Intermediate check (elseif)

C (Compare): Compares the top two elements of the stack. Writes logical result True (1) or False (0) to the stack.

D (Data): Loads a specific numeric value directly onto the stack (e.g., D33).

E (Execute): Dynamically executes an instruction or code block at the address on top of the stack.

F (File / File System): File stream operations through modifiers:

F1 — Open file (memory address of name -> descriptor)

F2 — Read byte or character from file

F3 — Write data from buffer to file

F4 — Close file by descriptor

G (Get): Reads a value directly from a hardware port or system register.

H (Hardware): Direct low-level interaction with the hardware bus or device controllers.

I (Input): Interactive reading of a character or signal from the keyboard or input stream onto the stack.

J (Jump): Unconditional jump to a specified label (classic goto).

K (Kernel): System function call of the micro-OS (kernel interrupt for process management).

L (Loop / Cycles): Organization of cyclic blocks through modifiers:

L1 — Start of loop

L2 — End of loop

M (Memory): Full read/write operations on RAM at a specific address.

N (Next): Increment (increases the value on the stack top by exactly 1).

O (Output): Universal output: prints a number, character, or entire text buffer to screen / VGA buffer.

P (Push/Pop / Duplicate): Stack operations: duplicate top element (like DUP) or remove excess.

Q (Quit): Terminates the session, stops current execution process, or exits the program.

R (Register): Fast operations with internal hardware processor registers for immediate data access.

S (Setup / Store): System environment initialization, register setup, or state saving.

T (Transform / Arithmetic): Universal computation block with numeric modifiers:

T1 — Addition (+)

T2 — Subtraction (-)

T3 — Multiplication (*)

T4 — Division (/)

U (Unpack / Pack): Packed data operations through modifiers:

U1 — Pack data

U2 — Unpack data

V (Vector): Configuration and redirection of hardware interrupt vectors.

W (Wait): Pause, execution delay, or waiting for the next processor tick.

X (XOR / Logic): Bitwise and logical operations through modifiers:

X1 — Logical or bitwise AND

X2 — Logical or bitwise OR

X3 — Exclusive XOR

Y (Yield): Voluntarily transfers control from the current process to the scheduler.

Z (Zero): Instantly zeroes the entire stack at the kernel level, or checks the top for zero.

3. How to Work with Them

The programming process in Atom is based on passing data through the stack.

Loading Data: First, necessary numbers or variables are pushed onto the stack using the D command (or via I for input).

Processing: Commands like T (transform), C (compare), or X (logic) take these values from the stack, process them, and return the result to the top of the stack.

Saving or Output: The result can be saved to memory using M or displayed on screen via O.

Example Sequence Logic:

Command D10 places the number 10 on the stack.

The next command T2 takes 10, adds 2, and pushes 12 back onto the stack.

Command O takes 12 and prints it to the screen.

4. Syntax

The syntax of the Atom language is maximally simplified to avoid unnecessary characters:

Command Format: Uppercase Latin letter (A to Z) plus an optional numeric argument with no spaces between them (e.g., D42, M100, W5).

Separators: Commands are separated by spaces or newlines.

Jump Labels: Jump labels are indicated by a number at the start of a line (e.g., 10:).

Comments: Everything after a ; symbol until the end of the line is ignored by the interpreter.

