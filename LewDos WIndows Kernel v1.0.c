#include "Config.h"
#define LDVersion "V1.1"
#define Stack_Chunksize 128
#define Heap_Chunksize 512
#define Routine_Tag_Offset 35
#pragma warning(disable : 4996)

//Virtual Memory Initialisation
int Stackpointer = 0;
int* Stack = 0;
int* Heap = 0;
//routine pointers
char** Routine_Lookup = 0;
int* Routine_Address = 0;
int Routine_Lookup_Length = 0;


/*debug Helper:
ErrorCodes:
    00 = nothing/standart value
    01 = Trapped. tried running code that was not valid/ jump to adress that does not exist
    02 = Program Cause Panic
Permissions:
    0 = User //Only software control, no hardware access
    1 = Driver //Hardware access but only specific
    2 = Root //All access 
*/
int Jmp(char* Arguments) {
    int i = 0;
    char Tag[40];
    sscanf(Arguments, "%s", Tag);
    printf("\033[1;33m LewDosKernel %s: Jumping to Routine %s length %d\033[0m\n", LDVersion, Tag, Routine_Lookup_Length);
    while (i < Routine_Lookup_Length + 1) {
        printf("\033[1;33m LewDosKernel %s: Try %d\033[0m\n", LDVersion, i);
        int compare_result = strcmp(Routine_Lookup[i], Tag);
        if (compare_result == 0) {
            printf("\033[1;33m LewDosKernel %s: Jumping to Routine %s\033[0m\n", LDVersion, Tag);
            break;
        }
        i++;
    }
    return i;
}

int* Logic_Args_Splitter(char* Arguments) {
    char argumentA[20];
    char argumentC[20];
    int OperandA = 0;
    int Destination = 0;
    sscanf(Arguments, "%s" "%s", argumentA, argumentC);
    int startpointA = 0;
    bool pointerA = false;
    if (argumentA[0] == '@') {
        startpointA = 1;
        pointerA = true;
    }
    if (argumentA[startpointA] == '$') {
        char* hexString = argumentA + startpointA + 1;
        sscanf(hexString, "%x", &OperandA);
    }
    else if (argumentA[startpointA] == '%') {
        char* binString = argumentA + startpointA + 1;
        OperandA = strtol(binString, NULL, 2);
    }
    else if (argumentA[startpointA] == '#') {
        char* octString = argumentA + startpointA + 1;
        sscanf(octString, "%o", &OperandA);
    }
    else {
        char* decString = argumentA + startpointA;
        sscanf(decString, "%d", &OperandA);
    }

    if (argumentC[0] == '@') {}
    else {
        printf("\033[1;31m Kernel Panic: Math Destination Argument not a Pointer\033[0m\n");
        return 255;
    }
    if (argumentC[1] == '$') {
        char* hexString = argumentC + 2;
        sscanf(hexString, "%x", &Destination);
    }
    else if (argumentC[1] == '%') {
        char* binString = argumentC + 2;
        Destination = strtol(binString, NULL, 2);
    }
    else if (argumentC[1] == '#') {
        char* octString = argumentC + 2;
        sscanf(octString, "%x", &Destination);;
    }
    else {
        char* decString = argumentC + 1;
        sscanf(decString, "%d", &Destination);
    }

    int* Output = (int*)malloc(3 * sizeof(int));
    Output[0] = OperandA;
    Output[1] = Destination;
    Output[2] = 0;
    if (pointerA == true) {
        Output[2] = 1;
    }
    return Output;
}

void Not(char* Arguments) {
    int* Args = (int*)malloc(3 * sizeof(int));
    Args = Logic_Args_Splitter(Arguments);
    switch (Args[2]) {
    case 0:
        Heap[Args[1]] = ~Args[0];
        break;
    case 1:
        Heap[Args[1]] = ~Heap[Args[0]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Logic is done. Knotted: to %d \033[0m\n", LDVersion, Heap[Args[1]]);
    free(Args);
    return;
}

void Mov(char* Arguments) {
    int* Args = (int*)malloc(3 * sizeof(int));
    Args = Logic_Args_Splitter(Arguments);
    switch (Args[2]) {
    case 0:
        Heap[Args[1]] = Args[0];
        break;
    case 1:
        Heap[Args[1]] = Heap[Args[0]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Logic is done. Knotted: to %d \033[0m\n", LDVersion, Heap[Args[1]]);
    free(Args);
    return;
}

int* Math_Args_Splitter(char* Arguments) {
    char argumentA[20];
    char argumentB[20];
    char argumentC[20];
    int OperandA = 0;
    int OperandB = 0;
    int Destination = 0;
    sscanf(Arguments, "%s" "%s" "%s", argumentA, argumentB, argumentC);
    int startpointA = 0;
    bool pointerA = false;
    if (argumentA[0] == '@'){
        startpointA = 1;
        pointerA = true;
    }
    if (argumentA[startpointA] == '$') {
        char* hexString = argumentA + startpointA + 1;
        sscanf(hexString, "%x", &OperandA);
    }
    else if (argumentA[startpointA] == '%') {
        char* binString = argumentA + startpointA + 1;
        OperandA = strtol(binString, NULL, 2);
    }
    else if (argumentA[startpointA] == '#') {
        char* octString = argumentA + startpointA + 1;
        sscanf(octString, "%o", &OperandA);
    }
    else {
        char* decString = argumentA + startpointA;
        sscanf(decString, "%d", &OperandA);
    }

    int startpointB = 0;
    bool pointerB = false;
    if (argumentB[0] == '@') {
        startpointB = 1;
        pointerB = true;
    }
    if (argumentB[startpointB] == '$') {
        char* hexString = argumentB + startpointB + 1;
        sscanf(hexString, "%x", &OperandB);
    }
    else if (argumentB[startpointB] == '%') {
        char* binString = argumentB + startpointB + 1;
        OperandB = strtol(binString, NULL, 2);
    }
    else if (argumentB[startpointB] == '#') {
        char* octString = argumentB + startpointB + 1;
        sscanf(octString, "%x", &OperandB);
    }
    else {
        char* decString = argumentB + startpointB;
        sscanf(decString, "%d", &OperandB);
    }

    if (argumentC[0] == '@') {}
    else {
        printf("\033[1;31m Kernel Panic: Math Destination Argument not a Pointer\033[0m\n");
        return 255;
    }
    if (argumentC[1] == '$') {
        char* hexString = argumentC + 2;
        sscanf(hexString, "%x", &Destination);
    }
    else if (argumentC[1] == '%') {
        char* binString = argumentC + 2;
        Destination = strtol(binString, NULL, 2);
    }
    else if (argumentC[1] == '#') {
        char* octString = argumentC + 2;
        sscanf(octString, "%x", &Destination);;
    }
    else {
        char* decString = argumentC + 1;
        sscanf(decString, "%d", &Destination);
    }

    int* Output = (int*)malloc(4 * sizeof(int));
    Output[0] = OperandA;
    Output[1] = OperandB;
    Output[2] = Destination;
    Output[3] = 0;
    if (pointerA == true) {
        Output[3] = 1;
    }
    if (pointerB == true) {
        Output[3] = 2;
    }
    if (pointerB == true && pointerA == true) {
        Output[3] = 3;
    }



    return Output;
}

void Add(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] + Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] + Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] + Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] + Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Addition result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void Sub(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] - Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] - Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] - Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] - Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Subtraction result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void Mul(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] * Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] * Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] * Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] * Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Multiplication result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void Div(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] / Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] / Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] / Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] / Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Division result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void And(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] & Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] & Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] & Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] & Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Division result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void Or(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] | Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] | Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] | Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] | Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Division result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}

void Xor(char* Arguments) {
    int* Args = (int*)malloc(4 * sizeof(int));
    Args = Math_Args_Splitter(Arguments);
    switch (Args[3]) {
    case 0:
        Heap[Args[2]] = Args[0] ^ Args[1];
        break;
    case 1:
        Heap[Args[2]] = Heap[Args[0]] ^ Args[1];
        break;
    case 2:
        Heap[Args[2]] = Args[0] ^ Heap[Args[1]];
        break;
    case 3:
        Heap[Args[2]] = Heap[Args[0]] ^ Heap[Args[1]];
        break;
    }
    printf("\033[1;33m LewDosKernel %s: Meth is made. Division result: %d \033[0m\n", LDVersion, Heap[Args[2]]);
    //printf("\033[1;33m LewDosKernel %s: Debugger: %d %d %d \033[0m\n", LDVersion, Args[0], Args[1], Args[2]);
    free(Args);
    return;
}


void Code_Execute(int* Code, int Permission, char filename[]) {
    //initialize Status/debug Flags
    bool Running = true; //for flow control
    bool Halted = false; //for halts, interrupts ect.
    bool Panic = false; //fatal error occured
    bool Run_status = false; //debug flag, if set to 1, the program crashed last last runtime
    int ErrorCode = 0; //if a error occured, will be saved here

    //allocating memory
    Stack = (int*)malloc(Stack_Chunksize * sizeof(int));
    Heap = (int*)malloc(Heap_Chunksize * sizeof(int));

    //Opening Code file
    FILE* ROM_Code = fopen(filename, "r");
    char read_Buffer[80];

    //Running Code
    printf("\033[1;33m LewDosKernel %s: Executing: %s\033[0m\n", LDVersion, filename);
    int Program_Counter = 0;
    while (fgets(read_Buffer, sizeof(read_Buffer), ROM_Code) != NULL) {
        if (Halted == false) {
            char* Arguments = read_Buffer + 4; //make the arguments without the command
            //printf("\033[1;33m LewDosKernel %s: Executing at line %d executing %d\033[0m\n", LDVersion, Program_Counter, Code[Program_Counter]);
            switch (Code[Program_Counter]) {
            case 0:
                //TRP
                Running = false;
                Panic = true;
                Run_status = false;
                ErrorCode = 1;
                printf("\033[1;31m Kernel Panic: Trap instruction Executed!\033[0m\n");
                break;
            case 1:
                //NOP
                //fprintf(stderr, "\033[1;33m LewDosKernel %s: No operation\033[0m\n", LDVersion);
                break;
            case 2:
                //EXIT
                Running = false;
                Panic = false;
                Run_status = true;
                printf("\033[1;32m LewDosKernel %s: Sucsessfully exited the program!\033[0m\n", LDVersion);
                break;
            case 3:
                //PNM
                Running = false;
                Panic = true;
                Run_status = false;
                ErrorCode = 2;
                printf("\033[1;31m Kernel Panic: %s \033[0m\n", Arguments);
                break;
            case 4:
                //HLT
                Halted = true;
                break;
            case 5:
                //ADD
                Add(Arguments);
                break;
            case 6:
                //SUB
                Sub(Arguments);
                break;
            case 7:
                //MUL
                Mul(Arguments);
                break;
            case 8:
                //DIV
                Div(Arguments);
                break;
            case 9:
                //AND
                And(Arguments);
                break;
            case 10:
                //OR
                Or(Arguments);
                break;
            case 11:
                //XOR
                Xor(Arguments);
                break;
            case 12:
                //NOT
                Not(Arguments);
                break;
            case 13:
                //MOV
                Mov(Arguments);
                break;
            case 14:
                //JMP
                Program_Counter = Jmp(Arguments);
                Program_Counter--;
                break;
            case 255:
                //End of file
                Running = false;
                Panic = false;
                Run_status = true;
                printf("\033[1;32m LewDosKernel %s: end of file. sucsessfully exited the program!\033[0m\n", LDVersion);
                break;
            }
        Program_Counter++;
        }
        if (Running == false) {
            break;
        }
        if (Halted == true) {
            Halted = false;
            printf("\033[1;33m LewDosKernel %s: no interrupts yet. Halt has been revoked.\033[0m\n", LDVersion);
        }
    }
    fclose(ROM_Code);
    return;
}

//Code assembler, gets called everytime a new Program needs to be executed
void  Assembler(char filename[], int Permission) {
    //Startup with opening the Assembly code to execute
    fprintf(stderr, "\033[1;33m Assembler.c: Started\033[0m\n");
    FILE* ROM_Code = fopen(filename, "r");
    if (ROM_Code == NULL) {
        printf("\033[1;31m Assembler.c: assembly file not found or cannot open file. \033[0m\n");
        return;
    }
    else {
        printf("\033[1;32m Assembler.c: assembly file found! loading program...\033[0m\n");
    }
    char read_Buffer[80];
    int Code_length = 1;
    char ch;
    while ((ch = fgetc(ROM_Code)) != EOF) { //find out length of the code file
        if (ch == '\n') {
            Code_length++;
        }
    }
    printf("\033[1;33m Assembler.c: code to be assembled: %s, %d Lines long\033[0m\n",filename, Code_length);
    fclose(ROM_Code);
    ROM_Code = fopen(filename, "r");

    // Allocate Memory for assembled output
    int* Op_Code = (int*)malloc(Code_length * sizeof(int));
    if (Op_Code == NULL) {
        printf("\033[1;31m Assembler.c: failed to allocate output memory\033[0m\n");
    }
    //Open Instruction set file
    FILE* Ins_Set;
    char read_ins[10];
    Ins_Set = fopen("Instruction_Set.ldf", "r");
    if (Ins_Set == NULL) {
        printf("\033[1;31m Assembler.c: Failed to open Instruction_Set.ldf file\033[0m\n");
    }
    else {
        printf("\033[1;32m Assembler.c: Sucsessfully Read Instruction set file\033[0m\n");
    }
    fclose(Ins_Set);
    int Instruction_Pointer = 0;
    while (fgets(read_Buffer, sizeof(read_Buffer), ROM_Code) != NULL) {
        char Instruction[40];
        sscanf(read_Buffer, "%s", Instruction);
        int Opcode_pointer = 0;
        Ins_Set = fopen("Instruction_Set.ldf", "r");
        bool found_instruction = 0;
        while (fgets(read_ins, sizeof(read_ins), Ins_Set) != NULL) {
            char Reference[10];
            sscanf(read_ins, "%s", Reference);
            int compare_result = strcmp(Reference, Instruction);
            if (compare_result == 0) {
                Op_Code[Instruction_Pointer] = Opcode_pointer;
                found_instruction = 1;
                //printf("\033[1;32m Assembler.c: OPcode assembled: %s %d \033[0m\n", Instruction, Opcode_pointer);
                break;
            }
            Opcode_pointer++;
        }
        if (found_instruction == 0) {
            /*if (Routine_Lookup_Length > 0) {
                int i = 0;
                while (i < Routine_Lookup_Length + 1) {
                    int compare_result = strcmp(Routine_Lookup[Routine_Lookup_Length], Instruction);
                    if (compare_result == 0) {
                        Op_Code[Instruction_Pointer] = i + Routine_Tag_Offset;
                        printf("\033[1;33m Assembler.c: Routine found. tag already exists: %s \033[0m\n", Instruction);
                        already_exists = true;
                        break;
                    }
                }
            }*/
            Op_Code[Instruction_Pointer] = 1;
            printf("\033[1;33m Assembler.c: Routine found. tag added: %s at address %d\033[0m\n", Instruction, Routine_Lookup_Length);
            char** Tmp_Routine_Lookup = realloc(Routine_Lookup, Routine_Lookup_Length * sizeof(char*));
            Tmp_Routine_Lookup[Routine_Lookup_Length] = malloc(41 * sizeof(char));
            Routine_Lookup = Tmp_Routine_Lookup;
            int* Tmp_Routine_Address = realloc(Routine_Address, Routine_Lookup_Length * sizeof(int));
            Routine_Address = Tmp_Routine_Address;
            strcpy(Routine_Lookup[Routine_Lookup_Length], Instruction);
            Routine_Address[Routine_Lookup_Length] = Instruction_Pointer;
            for (int i = 0; i < Routine_Lookup_Length; i++) {
                printf("%s\n", Routine_Lookup[i]);
            }
            Routine_Lookup_Length++;
        }
        fclose(Ins_Set);
        Instruction_Pointer++;
    }
    Op_Code[Instruction_Pointer] = 255;
    // Close the instruction set file
    fclose(ROM_Code);
    fclose(Ins_Set);
    printf("\033[1;32m Assembler.c: Sucsessfully Assembled Program, returning to Kernel\033[0m\n");
    Code_Execute(Op_Code, Permission, filename);
    free(Op_Code);
    return;
}

int main() {
    printf("\033[1;33m LewDosKernel %s: Started\033[0m\n", LDVersion);
    Assembler("LewdosOS.ass", 0);// load, compile and run OS File

    return 0;
}