#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_SIZE 512
#define MEMORY_SIZE 65536
#define REG_COUNT 8

int32_t stack[STACK_SIZE];
int sp = 0;
uint8_t memory[MEMORY_SIZE];
int32_t registers[REG_COUNT];
int zero_flag = 0;

void push(int32_t val) {
    if (sp >= STACK_SIZE) { printf("Stack Overflow!\n"); exit(1); }
    stack[sp++] = val;
}

int32_t pop() {
    if (sp <= 0) { printf("Stack Underflow!\n"); exit(1); }
    return stack[--sp];
}

int32_t atom_read_ps2() {
    printf("[HW - PS/2]: Reading keyboard port 0x60...\n");
    return 30;
}

int32_t atom_poll_usb() {
    printf("[HW - USB]: Polling XHCI/UHCI bus...\n");
    return 1;
}

void atom_vga_print(int32_t val) {
    printf("[VGA Output]: %d (Char: '%c')\n", val, (char)(val % 128));
}

int main() {
    printf("=== ATOM KERNEL BOOT (From B/T.c) ===\n");
    printf("[SYS Setup]: 1\n");
    push(10);
    push(10);
    { int32_t b = pop(); int32_t a = pop(); zero_flag = (a == b); push(a == b ? 1 : 0); }
    if (zero_flag) goto label_20;
    push(999);
    atom_vga_print(pop());
    return 0;
label_20:
    push(777);
    atom_vga_print(pop());
    return 0;
    printf("=== ATOM KERNEL SHUTDOWN ===\n");
    return 0;
}
