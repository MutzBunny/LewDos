#ifndef OS_CONFIG_H
#define OS_CONFIG_H

//Define Target Hardware Constants
#define System_Memory 65536			//Amount of RAM avaiable to the Target system
#define System_has_Filesystem 0		//define if target system has Storage media avaiable (Floppy drive, harddisks ect.)

#if System_has_Filesystem == 0
//if you have no Storage attached, configure those Values.
#define User_Name "root"
#define Password ""
//Module Loading if there is no Filesystem
//extern void Module_Function_Routine_Name(); to define all the loaded Functions by modules also define the 4Letter Call ID. and in what place in the Function Table the routine is.
extern void Module_Does_Other_Thing();
#define HELW 0
extern void Module_Does_Things();
#define OTHW 1
typedef void (*Module_function_type)();
const Module_function_type Module_functions[] = {
    //Add Module function routine names here, to make them callable.
    &Module_Does_Things,
    &Module_Does_Other_Thing
};

#endif


#endif // !OS_CONFIG_H