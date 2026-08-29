#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LABELS 256
#define MAX_INSTRUCTIONS 2048

typedef struct {
    int label_id;
    int instruction_address;
} Label;

Label label_table[MAX_LABELS];
int label_count = 0;

typedef struct {
    unsigned char opcode;    // 0-25 (A-Z)
    unsigned char modifier;  // Наприклад, 1 для T1, B1 тощо
    int arg;                 // Числовий аргумент або адреса
    char string_data[64];    // Для команд типу O/текст/
    int has_string;          // Прапорець наявності рядка
    int is_jump;
    int target_label_id;
} IRInstruction;

IRInstruction program[MAX_INSTRUCTIONS];
int instruction_count = 0;

void add_label(int label_id, int addr) {
    for (int i = 0; i < label_count; i++) {
        if (label_table[i].label_id == label_id) {
            fprintf(stderr, "Error: Duplicate label %d\n", label_id);
            exit(1);
        }
    }
    label_table[label_count++] = (Label){label_id, addr};
}

int find_label_address(int label_id) {
    for (int i = 0; i < label_count; i++) {
        if (label_table[i].label_id == label_id) {
            return label_table[i].instruction_address;
        }
    }
    fprintf(stderr, "Error: Undefined label %d\n", label_id);
    exit(1);
}

// Компіляція (Pass 1) з урахуванням стовпчика, слешів та Q-стопу
void compile_pass1(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open source file %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Видалення коментарів
        char *comment = strchr(line, ';');
        if (comment) *comment = '\0';

        char *ptr = line;
        while (*ptr) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            if (!*ptr) break;

            // Перевірка на мітку (наприклад, "1:")
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
                unsigned char opcode = cmd - 'A';

                unsigned char modifier = 0;
                int arg_val = 0;
                int has_arg = 0;
                char str_buf[64] = {0};
                int has_string = 0;

                // Перевірка на вивід тексту у слешах: O/текст/
                if (cmd == 'O' && *ptr == '/') {
                    ptr++; // пропускаємо перший '/'
                    int idx = 0;
                    while (*ptr && *ptr != '/' && idx < 63) {
                        str_buf[idx++] = *ptr++;
                    }
                    if (*ptr == '/') ptr++; // пропускаємо закриваючий '/'
                    str_buf[idx] = '\0';
                    has_string = 1;
                } 
                // Звичайні числові модифікатори або аргументи
                else if (isdigit((unsigned char)*ptr)) {
                    while (isdigit((unsigned char)*ptr)) {
                        arg_val = arg_val * 10 + (*ptr - '0');
                        ptr++;
                    }
                    modifier = (unsigned char)arg_val;
                    has_arg = 1;
                }

                int is_jump = 0;
                int target_label = 0;

                if (cmd == 'J' && has_arg) {
                    is_jump = 1;
                    target_label = arg_val;
                    arg_val = 0;
                }

                program[instruction_count++] = (IRInstruction){
                    opcode, modifier, arg_val, "", has_string, is_jump, target_label
                };
                
                // Зберігаємо рядок окремо у структурі, якщо він є
                if (has_string) {
                    strcpy(program[instruction_count - 1].string_data, str_buf);
                }

                // ЗАЛІЗОБЕТОННИЙ СТОП: Якщо це команда Q (Quit), припиняємо парсинг файлу повністю!
                if (cmd == 'Q') {
                    fclose(file);
                    printf("Q0 / Q encountered. Compilation stream halted successfully.\n");
                    return;
                }
            } else {
                ptr++;
            }
        }
    }
    fclose(file);
}

void generate_bytecode(const char *output_filename) {
    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create binary file %s\n", output_filename);
        exit(1);
    }

    for (int i = 0; i < instruction_count; i++) {
        IRInstruction *instr = &program[i];

        if (instr->is_jump) {
            instr->arg = find_label_address(instr->target_label_id);
        }

        // Запис у бінарник: Opcode (1) + Modifier (1) + Arg (4) + HasString (1) + StringData (64)
        fwrite(&instr->opcode, sizeof(unsigned char), 1, out);
        fwrite(&instr->modifier, sizeof(unsigned char), 1, out);
        fwrite(&instr->arg, sizeof(int), 1, out);
        fwrite(&instr->has_string, sizeof(int), 1, out);
        fwrite(instr->string_data, sizeof(char), 64, out);
    }

    fclose(out);
    printf("ROM generated: %s (%d instructions compiled)\n", output_filename, instruction_count);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <atom_c.atm> <output.rom>\n", argv[0]);
        return 1;
    }

    compile_pass1(argv[1]);
    generate_bytecode(argv[2]);

    return 0;
}
