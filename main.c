#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 256

// Macros for register access
#define sp (registers[SP])
#define ip (registers[IP])

// Updated InstructionSet enum
typedef enum {
    PSH = 0,
    POP = 1,
    ADD = 2,
    SET = 3,
    HLT = 4
} InstructionSet;

typedef enum {
    A, B, C, D, E, F, IP, SP,
    NUM_OF_REGISTERS
} Registers;

// Global variables
bool running = true;
int stack[STACK_SIZE];
int registers[NUM_OF_REGISTERS];

// Function to load the program from a file
int* load_program(const char* filename, int* program_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int* program = malloc(sizeof(int) * STACK_SIZE);
    int i = 0;

    while (fscanf(file, "%d", &program[i]) != EOF) {
        i++;
    }

    fclose(file);
    *program_size = i;
    return program;
}

// Fetch the current instruction
int fetch(const int* program) {
    return program[ip];
}

// Evaluate the instruction
void eval(int instr, const int* program) {
    switch (instr) {
        case HLT:
            running = false;
            break;
        case PSH:
            sp++;
            stack[sp] = program[++ip];
            break;
        case POP:
            printf("popped %d\n", stack[sp--]);
            break;
        case ADD: {
            int a = stack[sp--];
            int b = stack[sp--];
            stack[++sp] = b + a;
            break;
        }
        case SET:
            printf("SET is not yet implemented.\n");
            break;
        default:
            printf("Unknown instruction: %d\n", instr);
            running = false;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int program_size;
    int* program = load_program(argv[1], &program_size);

    registers[IP] = 0;
    registers[SP] = -1;

    while (running && registers[IP] < program_size) {
        eval(fetch(program), program);
        registers[IP]++;
    }

    free(program);
    return 0;
}
