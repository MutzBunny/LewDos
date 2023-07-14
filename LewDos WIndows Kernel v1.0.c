#include "Config.h"
#define LDVersion "V1.0"
#pragma warning(disable : 4996)


/*debug Helper:
Sections:
    CInS = Code Instruction Selector // extracts the Instruction from a whole line of code
    ILMA = Instruction list memory allocation
    OAMA = Opcode array memory allocation
    ISFM = Instruction File Manager
    RSR  = ROM startup Read // Reads the Operating system (LewdosOS.ass) file and starts up the kernel
*/

//Code Instruction Selector
char* CInS(const char* code_line) {
    // Find the length of the line of code
    size_t length = strlen(code_line);
    
    // Allocate memory for the first word
    char* Instruction = malloc(length + 1);
    if (Instruction == NULL) {
        fprintf(stderr, "\033[1;31mAssembler.c:(CInS) Memory allocation failed.\033[0m\n");
        exit(EXIT_FAILURE);
    }
    
    // Copy the first word into the new string
    int i = 0;
    while (code_line[i] != '\0' && code_line[i] != ' ') {
        Instruction[i] = code_line[i];
        i++;
    }
    Instruction[i] = '\0';
    
    return Instruction;
}

//main Assembler function, inputs 2D array of code to be assembled and stored in "Instruction List" the final result will be stored in "Op_Code"
//void Assembler(char Input_code[][80], int Code_length){
    //int Code_length = 1;
    /*while (Input_code[Code_length - 1][0] != NULL) {
        fprintf(stderr, "\033[1;33m %d %s\033[0m\n", Code_length, Input_code[Code_length - 1]);
        if (Input_code[Code_length - 1][0] != NULL){ fprintf(stderr, "\033[1;32m True\033[0m\n"); } else{ fprintf(stderr, "\033[1;31m false\033[0m\n"); }
        Code_length++;
    }*/
//}

int main() {
    fprintf(stderr, "\033[1;33m LewDosKernel %s: Started\033[0m\n", LDVersion);
    FILE* ROM_Code;
    char read_Buffer[80];  // Buffer to store read data
    ROM_Code = fopen("LewdosOS.ass", "r");
    if (ROM_Code == NULL) {
        fprintf(stderr, "\033[1;31m LewDosKernel %s:(RSR) No OS File found\033[0m\n", LDVersion);
        return 1;
    }
    else {
        printf("\033[1;32m LewDosKernel %s:(RSR) OS File found! Loading OS...\033[0m\n", LDVersion);
    }
    int Code_length = 1;
    char ch;
    while ((ch = fgetc(ROM_Code)) != EOF) {
        if (ch == '\n') {
            Code_length++;
        }
    }
    printf("\033[1;33m LewDosKernel %s:(RSR) OS file is %d Lines long\033[0m\n", LDVersion, Code_length);
    char** Code = (char**)malloc(Code_length * sizeof(char*));
    if (Code == NULL) {
        fprintf(stderr, "\033[1;31m LewDosKernel %s:(RSR) Memory allocation failed.\033[0m\n", LDVersion);
    }
    int copy_counter = 0;
    while (fgets(read_Buffer, sizeof(read_Buffer), ROM_Code) != NULL) {
    read_Buffer[strcspn(read_Buffer, "\n")] = '\0';
    fprintf(stderr, "\033[1;31m buffer: %s\033[0m\n", read_Buffer);
    Code[copy_counter] = malloc((strlen(read_Buffer) + 1) * sizeof(char));
    if (Code[copy_counter] == NULL) {
        // Handle memory allocation error
        fprintf(stderr, "\033[1;31m LewDosKernel %s:(RSR) Memory allocation failed.\033[0m\n", LDVersion);
        break; // or return or exit the program
    }
    strcpy(Code[copy_counter], read_Buffer);
    copy_counter++;
}
    //Assembler(Code, size);
    Code_length--;
    fprintf(stderr, "\033[1;33m Assembler.c: Started\033[0m\n");
    printf("\033[1;33m Assembler.c: Assembling %d Lines of code...\033[0m\n", Code_length);
    FILE* Ins_Set;
    char read_ins[100];  // Buffer to store read data

    // Dynamically allocate memory for the Opcode array
    int* Op_Code = (int*)malloc(Code_length * sizeof(int));
    if (Op_Code == NULL) {
        fprintf(stderr, "\033[1;31m Assembler.c:(OAMA) Memory allocation failed.\033[0m\n");
    }

    // Dynamically allocate memory for the Instruction List
    char** Instruction_List = (char**)malloc(Code_length * sizeof(char*));
    if (Instruction_List == NULL) {
        fprintf(stderr, "\033[1;31m Assembler.c:(ILMA) Memory allocation failed.\033[0m\n");
    }

    // Create Instruction list from code with the CInS
    for (int i = 0; i < Code_length; i++) {
        Instruction_List[i] = CInS(Code[i]);
    }

    // Open the Instruction set file in read mode
    Ins_Set = fopen("Instruction_Set.ldf", "r");
    if (Ins_Set == NULL) {
        fprintf(stderr, "\033[1;31m Assembler.c:(ISFM) Failed to open Instruction_Set.ldf file\033[0m\n");
    }
    fprintf(stderr, "\033[1;32m Assembler.c:(ISFM) Sucsessfully Read Instruction set file\033[0m\n");

    int Work_On_Opcode = 0;
    while (fgets(read_ins, sizeof(read_ins), Ins_Set) != NULL) {
        read_ins[strcspn(read_ins, "\n")] = '\0';
        // Compare the line with each string in the array
        for (int i = 0; i < Code_length; i++) {
            fprintf(stderr, "\033[1;33m I did a thing: %s\033[0m\n", Instruction_List[i]);
            if (strcmp(read_ins, Instruction_List[i]) == 0) {
                Op_Code[i] = Work_On_Opcode;  //Store the line number (Opcode) of the corresponding Instruction in the output array
                fprintf(stderr, "\033[1;33m I did a thing: %d\033[0m\n", Op_Code[i]);
            }
        }
        Work_On_Opcode++;
    }

    for (int i = 0; i < Code_length; i++) {
        fprintf(stderr, "\033[1;33m Compiled code: %d\033[0m\n", Op_Code[i]);
    }

    // Close the instruction set file
    fclose(Ins_Set);
    free(Instruction_List);
    free(Ins_Set);
    fprintf(stderr, "\033[1;32m Assembler.c:(ISFM) Sucsessfully Assembled Program\033[0m\n");

    bool Running = true;
    bool Panic = false;
    bool Run_status = false;
    int Program_Counter = 0;
    while (Running == true) {
        fprintf(stderr, "\033[1;33m LewDosKernel %s: Executing at line %d executing %d\033[0m\n", LDVersion, Program_Counter, Op_Code[Program_Counter]);
        Program_Counter++;
        switch (Op_Code[Program_Counter]) {
        case 0:
            Running = false;
            Panic = true;
            Run_status = false;
            fprintf(stderr, "\033[1;31m Kernel Panic: Trap instruction Executed!\033[0m\n");
            break;
        case 1:
            //NOP
            fprintf(stderr, "\033[1;33m LewDosKernel %s: No operation\033[0m\n", LDVersion);
            break;
        case 2:
            Running = false;
            Panic = false;
            Run_status = true;
            fprintf(stderr, "\033[1;32m LewDosKernel %s: Sucsessfully exited the program!\033[0m\n", LDVersion);
            break;
            //case 3:
        }

        /*if (Program_Counter > Code_length) {
            Running = false;
            Run_status = true;
        }*/
    }

    return 0;
}