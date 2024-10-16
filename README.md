# Simple Virtual Machine (VM) in C

This project implements a basic virtual machine (VM) in C. It reads instructions from a `.vm` file and executes them. The VM has a stack, a set of registers, and supports a limited instruction set.

## Features

* **Stack-based architecture:**  The VM uses a stack for data manipulation.
* **Register support:**  A small number of registers are available for storing values.  `IP` (Instruction Pointer) and `SP` (Stack Pointer) are crucial for program execution.
* **Instruction Set:**  The VM currently supports the following instructions:
    * **PSH `value`:** Pushes `value` onto the stack.
    * **POP:** Pops the top value from the stack and prints it.
    * **ADD:** Pops the top two values from the stack, adds them, and pushes the result.
    * **SUB:** Pops the top two values, subtracts the second from the first, and pushes the result.
    * **MUL:** Pops the top two values, multiplies them, and pushes the result.
    * **DIV:** Pops the top two values, divides the first by the second, and pushes the result.
    * **SET:** *TODO*
    * **HLT:** Halts the VM execution.
* **Error Handling:**  Basic error handling is implemented for stack overflow/underflow, division by zero, and integer overflow during arithmetic operations.
* **Dynamic Program Loading:** The program is loaded from a file at runtime, and memory is dynamically allocated and reallocated as needed to accommodate programs of varying sizes. This prevents buffer overflows and allows for larger programs to be executed.
* **Comment Support:**  Lines starting with `#` in the `.vm` file are treated as comments and ignored.
* **MakeFile:** Simplifies compilation with predefined rules and flags.



## Usage

1. **Compile:**  Use the provided Makefile to compile the VM:
   ```bash
   make
   ```

2. **Create a `.vm` program file:**  Write your program using the supported instructions.  For example, `program.vm`:

   ```assembly
   PSH 5
   PSH 10
   ADD
   POP
   HLT
   ```

3. **Run the VM:**
   ```bash
   ./vm program.vm
   ```

   This will execute the instructions in `program.vm` and print the output.  In this example, it would print:

   ```
   Added: 5 + 10 = 15
   Popped: 15
   ```


## Example Use Cases

**1. Simple Arithmetic:**

```assembly
PSH 25
PSH 10
SUB
POP
HLT
```
This program pushes 25 and 10 onto the stack, subtracts 10 from 25, and prints the result (15).

**2. More Complex Calculation:**

```assembly
PSH 4
PSH 2
MUL
PSH 3
ADD
POP
HLT
```

This program calculates (4 * 2) + 3.


**3. Handling larger programs** You can create more complex programs using a combination of these instructions. Because of the dynamic memory allocation during program loading, the VM can handle much larger programs than would be possible with a fixed-size array.


## Future Improvements

* **Implement SET instruction:** Add functionality for the `SET` instruction to allow manipulation of registers.
* **Extend Instruction Set:**  Introduce more instructions (e.g., comparison, branching, I/O).
* **Improved Debugging:**  Enhance error messages and provide better debugging tools.
* **Variable Support:** Introduce variables or labels to make programs more readable and maintainable.


## Contributing

Contributions are welcome!  Feel free to submit pull requests for bug fixes, new features, or improvements to the existing code.
