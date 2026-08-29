#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_SIZE 256
#define MEMORY_SIZE 65536
#define MAX_LABELS 100
#define MAX_INSTRUCTIONS 1000

// Структура для міток
typedef struct {
    int label_id;
    int instruction_index;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

// Структура інструкції Atom
typedef struct {
    char cmd;
    int modifier;
    int arg;
    char text[128]; // Для виводу тексту типу O/текст/
    int has_text;
    int is_jump;
    int target_label;
} Instruction;

Instruction program[MAX_INSTRUCTIONS];
int instruction_count = 0;

// Стекова машина та пам'ять
int stack[STACK_SIZE];
int sp = -1;

unsigned char memory[MEMORY_SIZE];
int heap_ptr = 0;

int registers[16];
int flags = 0;

// Операції зі стеком
void push(int val) {
    if (sp < STACK_SIZE - 1) {
        stack[++sp] = val;
    } else {
        fprintf(stderr, "Runtime Error: Stack Overflow!\n");
        exit(1);
    }
}

int pop() {
    if (sp >= 0) {
        return stack[sp--];
    } else {
        fprintf(stderr, "Runtime Error: Stack Underflow!\n");
        exit(1);
    }
}

// Додавання мітки
void add_label(int label_id, int index) {
    labels[label_count++] = (Label){label_id, index};
}

int find_label(int label_id) {
    for (int i = 0; i < label_count; i++) {
        if (labels[i].label_id == label_id) {
            return labels[i].instruction_index;
        }
    }
    fprintf(stderr, "Error: Undefined label %d\n", label_id);
    exit(1);
}

// Читання та парсинг файлу Atom (Pass 1: збір міток та інструкцій)
void load_program(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open source file %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Видалення коментарів після ';'
        char *comment = strchr(line, ';');
        if (comment) *comment = '\0';

        char *ptr = line;
        while (*ptr) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            if (!*ptr) break;

            // Перевірка на мітку (наприклад, "10:")
            if (isdigit((unsigned char)*ptr)) {
                int label_id = 0;
                while (isdigit((unsigned char)*ptr)) {
                    label_id = label_id * 10 + (*ptr - '0');
                    ptr++;
                }
                if (*ptr == ':') {
                    ptr++;
                    add_label(label_id, instruction_count);
                    continue;
                }
            }

            // Зчитування команди (A-Z)
            if (isalpha((unsigned char)*ptr)) {
                char cmd = toupper((unsigned char)*ptr);
                ptr++;

                int modifier = 0;
                int arg = 0;
                int has_arg = 0;
                char text_buf[128] = {0};
                int has_text = 0;

                // Перевірка на рядок у слешах для команди O: O/текст/
                if (cmd == 'O' && *ptr == '/') {
                    ptr++; // пропускаємо '/'
                    int idx = 0;
                    while (*ptr && *ptr != '/' && idx < 127) {
                        text_buf[idx++] = *ptr++;
                    }
                    if (*ptr == '/') ptr++; // пропускаємо закриваючий '/'
                    text_buf[idx] = '\0';
                    has_text = 1;
                }
                // Зчитування числових модифікаторів або аргументів
                else if (isdigit((unsigned char)*ptr)) {
                    while (isdigit((unsigned char)*ptr)) {
                        arg = arg * 10 + (*ptr - '0');
                        ptr++;
                    }
                    modifier = arg;
                    has_arg = 1;
                }

                int is_jump = 0;
                int target_label = 0;
                if (cmd == 'J' && has_arg) {
                    is_jump = 1;
                    target_label = arg;
                    arg = 0;
                }

                program[instruction_count++] = (Instruction){
                    cmd, modifier, arg, "", has_text, is_jump, target_label
                };

                if (has_text) {
                    strcpy(program[instruction_count - 1].text, text_buf);
                }

                // ЗАЛІЗОБЕТОННИЙ СТОП: Якщо це Quit (Q), припиняємо читати файл взагалі!
                if (cmd == 'Q') {
                    fclose(file);
                    return;
                }
            } else {
                ptr++;
            }
        }
    }
    fclose(file);
}

// Виконання програми інтерпретатором
void run_interpreter() {
    int pc = 0;
    FILE *files[10] = {0};

    // Розв'язання переходів для міток
// Розв'язання переходів для міток
    for (int i = 0; i < instruction_count; i++) {
        if (program[i].is_jump) {
            program[i].arg = find_label(program[i].target_label); // Виправили тут!
        }
    }
    while (pc < instruction_count) {
        Instruction instr = program[pc];

        switch (instr.cmd) {
            case 'A': { // Allocate
                int size = pop();
                if (heap_ptr + size < MEMORY_SIZE) {
                    push(heap_ptr);
                    heap_ptr += size;
                } else {
                    fprintf(stderr, "Runtime Error: Heap Overflow\n");
                    exit(1);
                }
                break;
            }
            case 'B': { // Branch / Умови (B1-B4)
                int cond = pop();
                if (instr.modifier == 1) { // B1: if then
                    if (!cond) {
                        // Якщо умова хибна, можна шукати відповідний else/endif
                    }
                } else if (instr.modifier == 2) { // B2: else
                    
                } else if (instr.modifier == 3) { // B3: end if
                    
                } else if (instr.modifier == 4) { // B4: elseif
                    
                }
                break;
            }
            case 'C': { // Compare
                int b = pop();
                int a = pop();
                int res = (a == b) ? 1 : 0;
                push(res);
                flags = res;
                break;
            }
            case 'D': { // Data
                push(instr.arg);
                break;
            }
            case 'E': { // Execute
                int addr = pop();
                // Виконання коду за адресою в пам'яті
                break;
            }
            case 'F': { // File operations (F1-F4)
                if (instr.modifier == 1) { // F1: Відкрити файл
                    int addr = pop();
                    char *fname = (char*)&memory[addr];
                    FILE *f = fopen(fname, "r+");
                    if (!f) f = fopen(fname, "w+");
                    int fid = 0;
                    while (files[fid] != NULL && fid < 10) fid++;
                    files[fid] = f;
                    push(fid);
                } else if (instr.modifier == 2) { // F2: Читати байт
                    int fid = pop();
                    if (files[fid]) {
                        int ch = fgetc(files[fid]);
                        push(ch == EOF ? -1 : ch);
                    }
                } else if (instr.modifier == 3) { // F3: Записати байт
                    int val = pop();
                    int fid = pop();
                    if (files[fid]) fputc(val, files[fid]);
                } else if (instr.modifier == 4) { // F4: Закрити файл
                    int fid = pop();
                    if (files[fid]) {
                        fclose(files[fid]);
                        files[fid] = NULL;
                    }
                }
                break;
            }
            case 'G': { // Get
                push(registers[instr.arg]);
                break;
            }
            case 'H': { // Hardware
                break;
            }
            case 'I': { // Input
                push(getchar());
                break;
            }
            case 'J': { // Jump
                pc = instr.arg;
                continue;
                break;
            }
            case 'K': { // Kernel
                break;
            }
            case 'L': { // Loop (L1-L2)
                if (instr.modifier == 1) { // L1: початок циклу
                    
                } else if (instr.modifier == 2) { // L2: кінець циклу
                    
                }
                break;
            }
            case 'M': { // Memory
                int addr = pop();
                push(memory[addr]);
                break;
            }
            case 'N': { // Next (Інкремент)
                push(pop() + 1);
                break;
            }
            case 'O': { // Output
                if (instr.has_text) {
                    printf("%s", instr.text);
                } else {
                    int val = pop();
                    putchar(val);
                }
                fflush(stdout);
                break;
            }
            case 'P': { // Push/Pop / Duplicate
                if (sp >= 0) push(stack[sp]); // DUP
                break;
            }
            case 'Q': { // Quit
                return;
                break;
            }
            case 'R': { // Register
                registers[0] = pop();
                break;
            }
            case 'S': { // Setup / Store
                int val = pop();
                int addr = pop();
                memory[addr] = (unsigned char)val;
                break;
            }
            case 'T': { // Transform (Арифметика T1-T4)
                int b = pop();
                int a = pop();
                if (instr.modifier == 1) push(a + b);      // T1 (+)
                else if (instr.modifier == 2) push(a - b); // T2 (-)
                else if (instr.modifier == 3) push(a * b); // T3 (*)
                else if (instr.modifier == 4) {            // T4 (/)
                    if (b != 0) push(a / b);
                    else { fprintf(stderr, "Runtime Error: Division by zero\n"); exit(1); }
                }
                break;
            }
            case 'U': { // Unpack / Pack (U1-U2)
                if (instr.modifier == 1) { // U1: Pack
                    
                } else if (instr.modifier == 2) { // U2: Unpack
                    
                }
                break;
            }
            case 'V': { // Vector
                break;
            }
            case 'W': { // Wait
                break;
            }
            case 'X': { // Logic (X1-X3)
                int b = pop();
                int a = pop();
                if (instr.modifier == 1) push(a & b);      // X1: AND
                else if (instr.modifier == 2) push(a | b); // X2: OR
                else if (instr.modifier == 3) push(a ^ b); // X3: XOR
                break;
            }
            case 'Y': { // Yield
                break;
            }
            case 'Z': { // Zero
                sp = -1;
                break;
            }
            default:
                break;
        }
        pc++;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program.atm>\n", argv[0]);
        return 1;
    }

    load_program(argv[1]);
    run_interpreter();

    return 0;
}
