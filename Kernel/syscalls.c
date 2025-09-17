#include "../Include/System_Config.h"
#include "../Include/main.h"
#include <stdarg.h>

/*  Syscalls is neededfor the kernel to be able to access critical hardware, like the console output or storage media
    For every system this code needs to be adjusted or rewritten, this is why this code is very well documented. 
    If you dont understand something or need assistance with porting, please email me at toby@analogic.ch
    or add me on discord @mutzbunny*/

//For Ripes RiscV emulator, there are functions called Ecalls, kinda like x86 Bios calls to write to the screen
//this code defines which calls are for what function. If your system accepts syscalls like this,
//you can easely adapt this code to your machine.
#define ECALL_PRINT_INT       1
#define ECALL_PRINT_FLOAT     2
#define ECALL_PRINT_STRING    4
#define ECALL_PRINT_CHAR      11
#define ECALL_PRINT_HEX       34
#define ECALL_PRINT_BIN       35
#define ECALL_PRINT_UNSIGNED  36

//This is a Inline assembly function to call those system calls. for me it loads two registers;
//a7 which defines what Ecall you want to execute and a0 which has the character to print.
static inline void ecall(int a7, int a0) {
    register int ra0 asm("a0") = a0;
    register int ra7 asm("a7") = a7;
    asm volatile ("ecall" : : "r"(ra0), "r"(ra7) : "memory");
}

//defined here are all the system calls that can be made, but now as C functions, to be called by the code below
static void print_char(char c)         { ecall(ECALL_PRINT_CHAR, c); }
static void print_string(const char* s){ ecall(ECALL_PRINT_STRING, (int)s); }
static void print_int(int val)         { ecall(ECALL_PRINT_INT, val); }
static void print_unsigned(unsigned val){ ecall(ECALL_PRINT_UNSIGNED, val); }
static void print_hex(unsigned val)    { ecall(ECALL_PRINT_HEX, val); }
static void print_bin(unsigned val)    { ecall(ECALL_PRINT_BIN, val); }

/*
Main Print function. This gets called by the Kernel and all the modules to output Strings.
it accepts formatted strings to insert variables:
%d for integer values
%u for unsigned values
%x to print hexadecimal
%b to print the binary value
%c to print a character
%s for full strings (requires a pointer)
%p to output a pointer (not whats at the pointers pointing location, but where the pointer is pointing in hex)
*/
void Serial_out(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt); 

//loops through all the characters in your pointed string, to find all the formatting stuff.
    for (const char* p = fmt; *p; ++p) {
        if (*p != '%') { //if it doesnt find any formatting character, it just prints the character
            print_char(*p);
            continue;
        }
//if i does find a formatting character, (atm just a %)
        ++p; // skip '%'
        switch(*p) { //determine that formatting parameters we have
            case 'd': { //%d integer
                int val = va_arg(args, int);
                print_int(val);
                break;
            }
            case 'u': { //%u unsigned
                unsigned val = va_arg(args, unsigned);
                print_unsigned(val);
                break;
            }
            case 'x': { //%x hexadecimal value
                unsigned val = va_arg(args, unsigned);
                print_hex(val);
                break;
            }
            case 'b': { //%b binary value
                unsigned val = va_arg(args, unsigned);
                print_bin(val);
                break;
            }
            case 'c': { //%c single character
                char val = (char)va_arg(args, int);
                print_char(val);
                break;
            }
            case 's': { //%s string
                const char* val = va_arg(args, const char*);
                print_string(val);
                break;
            }
            case 'p': { //%p pointer as hex
                void* val = va_arg(args, void*);
                print_hex((unsigned int)val);
                break;
            }

            default: // unknown specifier, print literally
                print_char('%');
                print_char(*p);
        }
    }

    va_end(args);
}