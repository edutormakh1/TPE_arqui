#include "exceptions.h"
#include "keyboard.h"
#include "video_driver.h"

static void zero_division();
static void invalid_opcode();
static void excepHandler(char * msg);
extern void returnToUserland(); 
extern void _hlt();
extern void _sti();

static Exception exceptions[]={&zero_division, 0, 0, 0, 0, 0, &invalid_opcode};
static char * message[] = {"Zero Division Exception", "Invalid Opcode Exception"};

// Colores para la visualización
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0xFF0000


void exceptionDispatcher(int exception) {
  	Exception ex = exceptions[exception];
	if(ex != 0){
		ex();
	}
}


static void excepHandler(char * msg){
	vdPrint(msg, COLOR_RED);
	newLine();
	newLine();
	vdPrint("Press enter to continue", COLOR_WHITE);
	int c;
	_sti();
	do{
		_hlt();	
	} while((c=getCharFromBuffer()) != '\n');
	vdClear();

}

static void zero_division() {
	excepHandler(message[0]);
}

static void invalid_opcode(){
	excepHandler(message[1]);
}
