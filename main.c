#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define STACK_SIZE 256
#define MAX_LINE_LENGTH 100
#define INITIAL_SIZE 32

// Macros for register access
#define sp (registers[SP])
#define ip (registers[IP])

typedef enum {
    PSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    SET,
    HLT,
    UNKNOWN
} InstructionSet;

typedef enum {
    A, B, C, D, E, F, IP, SP,
    NUM_OF_REGISTERS
} Registers;

// Global variables
bool running = true;
int stack[STACK_SIZE];
int registers[NUM_OF_REGISTERS];

// Function prototypes
InstructionSet string_to_instruction(const char* str);
int* load_program(const char* filename, int* program_size);
int fetch(const int* program);
bool push(int value);
bool pop(int* value);
bool add();
bool sub();
bool mul();
bool div_op();
void eval(int instr, const int* program);
bool set();

// Function to convert string to InstructionSet enum
InstructionSet string_to_instruction(const char* str) {
    if (strcmp(str, "PSH") == 0) return PSH;
    if (strcmp(str, "POP") == 0) return POP;
    if (strcmp(str, "ADD") == 0) return ADD;
    if (strcmp(str, "SUB") == 0) return SUB;
    if (strcmp(str, "MUL") == 0) return MUL;
    if (strcmp(str, "DIV") == 0) return DIV;
    if (strcmp(str, "SET") == 0) return SET;
    if (strcmp(str, "HLT") == 0) return HLT;
    return UNKNOWN;
}

// Function to load the program from a file
int* load_program(const char* filename, int* program_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int allocated_size = INITIAL_SIZE; // Start with a base size
    int* program = malloc(sizeof(int) * allocated_size);
    if (!program) {
        perror("Failed to allocate memory for program");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) == 0 || (line[0] == '\n')) continue;

        char* comment_start = strchr(line, '#');
        if (comment_start) *comment_start = '\0';

        char* token = strtok(line, " \n");
        while (token != NULL) {
            InstructionSet instruction = string_to_instruction(token);
            if (instruction == PSH) {
                token = strtok(NULL, " \n");
                if (token) {
                    // Ensure enough space
                    if (i + 2 > allocated_size) {
                        allocated_size *= 2; // Double the size
                        program = realloc(program, sizeof(int) * allocated_size);
                        if (!program) {
                            perror("Failed to reallocate memory for program");
                            exit(EXIT_FAILURE);
                        }
                    }
                    program[i++] = PSH;
                    program[i++] = atoi(token);
                }
            } else if (instruction != UNKNOWN) {
                // Ensure enough space for other instructions
                if (i >= allocated_size) {
                    allocated_size *= 2;
                    program = realloc(program, sizeof(int) * allocated_size);
                    if (!program) {
                        perror("Failed to reallocate memory for program");
                        exit(EXIT_FAILURE);
                    }
                }
                program[i++] = instruction;
            }
            token = strtok(NULL, " \n");
        }
    }

    fclose(file);
    *program_size = i; // Set the actual size of the loaded program
    return program;
}
// Fetch the current instruction
int fetch(const int* program) {
    return program[ip];
}

// Push a value onto the stack
bool push(int value) {
    if (sp < STACK_SIZE - 1) {
        sp++;
        stack[sp] = value;
        return true;
    } else {
        fprintf(stderr, "Error: Stack overflow\n");
        return false;
    }
}

// Pop a value from the stack
bool pop(int* value) {
    if (sp >= 0) {
        *value = stack[sp--];
        return true;
    } else {
        fprintf(stderr, "Error: Stack underflow\n");
        return false;
    }
}

// Perform addition operation
bool add() {
    int a, b;
    if (!pop(&a) || !pop(&b)) return false;
    
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        fprintf(stderr, "Error: Integer overflow in addition\n");
        return false;
    }
    
    int result = b + a;
    if (!push(result)) return false;
    
    printf("Added: %d + %d = %d\n", b, a, result);
    return true;
}

// Perform subtraction operation
bool sub() {
    int a, b;
    if (!pop(&a) || !pop(&b)) return false;
    
    if ((b > 0 && a < INT_MIN + b) || (b < 0 && a > INT_MAX + b)) {
        fprintf(stderr, "Error: Integer overflow in subtraction\n");
        return false;
    }
    
    int result = b - a;
    if (!push(result)) return false;
    
    printf("Subtracted: %d - %d = %d\n", b, a, result);
    return true;
}

// Perform multiplication operation
bool mul() {
    int a, b;
    if (!pop(&a) || !pop(&b)) return false;
    
    if (a > 0 && b > 0 && a > INT_MAX / b) {
        fprintf(stderr, "Error: Integer overflow in multiplication\n");
        return false;
    }
    if (a > 0 && b <= 0 && b < INT_MIN / a) {
        fprintf(stderr, "Error: Integer overflow in multiplication\n");
        return false;
    }
    if (a <= 0 && b > 0 && a < INT_MIN / b) {
        fprintf(stderr, "Error: Integer overflow in multiplication\n");
        return false;
    }
    if (a <= 0 && b <= 0 && a != 0 && b < INT_MAX / a) {
        fprintf(stderr, "Error: Integer overflow in multiplication\n");
        return false;
    }
    
    int result = b * a;
    if (!push(result)) return false;
    
    printf("Multiplied: %d * %d = %d\n", b, a, result);
    return true;
}

// Perform division operation
bool div_op() {
    int a, b;
    if (!pop(&a) || !pop(&b)) return false;
    
    if (a == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return false;
    }
    
    if (b == INT_MIN && a == -1) {
        fprintf(stderr, "Error: Integer overflow in division\n");
        return false;
    }
    
    int result = b / a;
    if (!push(result)) return false;
    
    printf("Divided: %d / %d = %d\n", b, a, result);
    return true;
}

bool set() {
    printf("SET not implemented yet\n");
    return false;
}

// Evaluate the instruction
void eval(int instr, const int* program) {
    switch (instr) {
        case HLT:
            running = false;
            break;
        case PSH:
            if (!push(program[++ip])) running = false;
            break;
        case POP: {
            int value;
            if (pop(&value)) {
                printf("Popped: %d\n", value);
            } else {
                running = false;
            }
            break;
        }
        case ADD:
            if (!add()) running = false;
            break;
        case SUB:
            if (!sub()) running = false;
            break;
        case MUL:
            if (!mul()) running = false;
            break;
        case DIV:
            if (!div_op()) running = false;
            break;
        case SET:
            if (!set()) running = false;
            break;
        default:
            printf("Unknown instruction: %d\n", instr);
            running = false;
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    const char* ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".vm") != 0) {
        fprintf(stderr, "Error: Only .vm files are accepted.\n");
        return EXIT_FAILURE;
    }

    int program_size;
    int* program = load_program(filename, &program_size);

    registers[IP] = 0;
    registers[SP] = -1;

     while (running) {
        if (ip < 0 || ip >= program_size) {
            fprintf(stderr, "Error: Program terminated without HLT instruction or invalid instruction encountered.\n");
            free(program); // Ensure to free before exit
            return EXIT_FAILURE;
        }
        int current_instruction = fetch(program);
        eval(current_instruction, program);
        ip++;
    }


    free(program);
    return 0;
}