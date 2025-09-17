extern int main(void);
extern unsigned int _stack_top;
extern int _write(int, const char*, int);

// align _start to 4 bytes
void _start(void) __attribute__((section(".text"), aligned(4)));

//stack initialisation and callin of main
void _start(void) {
    asm volatile("mv sp, %0" :: "r"(&_stack_top));
    main();
    while (1) {}
}
