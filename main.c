#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 256
#define MAX_LINE_LENGTH 100

// Macros for register access
#define sp (registers[SP])
#define ip (registers[IP])

typedef enum {
    PSH,
    POP,
    ADD,
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
void push(int value);
int pop();
void add();
void eval(int instr, const int* program);

// Function to convert string to InstructionSet enum
InstructionSet string_to_instruction(const char* str) {
    if (strcmp(str, "PSH") == 0) return PSH;
    if (strcmp(str, "POP") == 0) return POP;
    if (strcmp(str, "ADD") == 0) return ADD;
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

    int* program = malloc(sizeof(int) * STACK_SIZE);
    char line[MAX_LINE_LENGTH];
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) == 0 || (line[0] == '\n')) {
            continue;
        }

        // Skip comments
        char* comment_start = strchr(line, '#');
        if (comment_start) {
            *comment_start = '\0';  // Null-terminate the string at the comment
        }

        // Check if the line is now empty after removing comments
        if (strlen(line) == 0) {
            continue;
        }

        // Split the line into tokens and process
        char* token = strtok(line, " \n");
        while (token != NULL) {
            InstructionSet instruction = string_to_instruction(token);
            if (instruction == PSH) {
                // Read the next token for the value
                token = strtok(NULL, " \n");
                if (token) {
                    program[i++] = PSH;  // Add the PSH instruction
                    program[i++] = atoi(token);  // Add the value
                }
            } else if (instruction != UNKNOWN) {
                program[i++] = instruction;  // Add other instructions directly
            }
            token = strtok(NULL, " \n");
        }
    }

    fclose(file);
    *program_size = i;
    return program;
}

// Fetch the current instruction
int fetch(const int* program) {
    return program[ip];
}

// Push a value onto the stack
void push(int value) {
    if (sp < STACK_SIZE - 1) {
        sp++;
        stack[sp] = value;
    } else {
        fprintf(stderr, "Error: Stack overflow\n");
        running = false;
    }
}

// Pop a value from the stack
int pop() {
    if (sp >= 0) {
        return stack[sp--];
    } else {
        fprintf(stderr, "Error: Stack underflow\n");
        running = false;
        return 0;
    }
}

// Perform addition operation
void add() {
    int a = pop();
    int b = pop();
    int result = b + a;
    push(result);
    printf("Added: %d + %d = %d\n", b, a, result);
}

// Evaluate the instruction
void eval(int instr, const int* program) {
    switch (instr) {
        case HLT:
            running = false;
            break;
        case PSH:
            push(program[++ip]);
            break;
        case POP:
            printf("Popped: %d\n", pop());
            break;
        case ADD:
            add();
            break;
        case SET:
            printf("SET is not yet implemented.\n");
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

    // Check if the file has a .vm extension
    const char* filename = argv[1];
    const char* ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".vm") != 0) {
        fprintf(stderr, "Error: Only .vm files are accepted.\n");
        return EXIT_FAILURE;
    }

    int program_size;
    int* program = load_program(filename, &program_size);

    // Initialize the instruction pointer and stack pointer
    registers[IP] = 0;
    registers[SP] = -1;

    // Run the program
    while (running && registers[IP] < program_size) {
        eval(fetch(program), program);
        registers[IP]++;
    }

    // Clean up
    free(program);
    return 0;
}