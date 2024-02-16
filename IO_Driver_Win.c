#include "IO_Driver.h"
#include <stdio.h>
#include <stdlib.h>


//Print command. Parameters are the message as a Char array (String) and the ANSI atributes. you can split the atributes by a ';' for example red, italic would be "3;31" the "" are important
/*
ANSI Color list:
Color:	Forground	Background
Black	30			40
Red		31			41
Green	32			42
Yellow	33			43
Blue	34			44
Magenta	35			45
Cyan	36			46
White	37			47

Additionally you can add the ANSI commands for (some of them dont work always...)
Bold Italic Underlined Blinking Inverted Hidden Dim
1	 3		4		   5		7		 8		2

you can also move your cursor:
Up	Down Left Right
A	B	 D	  C
you can also jump to the beginning of line with 'E' and to the previous line with 'F'
*/


void Systemout_Print(char* Message, char* Color) {
	printf("\033[%sm%s\033[0m\n", Color, Message);
	return;
}

//Clears the Entire Terminal. for things like Teletypes that wont work. maybe insert two, three newlines in here if you plan on using a teletype
void Systemout_Clear() {
	system("cls");
	return;
}