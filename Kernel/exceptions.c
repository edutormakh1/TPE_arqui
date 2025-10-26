#include "exceptions.h"
#include "keyboard.h"
#include "video_driver.h"

static void zero_division(RegisterContext *ctx);
static void invalid_opcode(RegisterContext *ctx);
static void excepHandler(char * msg, RegisterContext *ctx);
extern void returnToUserland(); 
extern void _hlt();
extern void _sti();

static Exception exceptions[]={&zero_division, 0, 0, 0, 0, 0, &invalid_opcode};
static char * message[] = {"Zero Division Exception", "Invalid Opcode Exception"};

// Colores para la visualización
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0xFF0000
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF

void exceptionDispatcher(int exception, RegisterContext *ctx) {
  	Exception ex = exceptions[exception];
	if(ex != 0){
		ex(ctx);
	}
}


static void zero_division(RegisterContext *ctx) {
	excepHandler(message[0], ctx);
}

static void invalid_opcode(RegisterContext *ctx){
	excepHandler(message[1], ctx);
}

/**
 * Imprime un registro con formato: "NOMBRE: 0xVALOR"
 */
static void printRegister(const char *name, uint64_t value) {
	vdPrint(name, COLOR_CYAN);
	vdPrint(": 0x", COLOR_WHITE);
	vdPrintHex(value, COLOR_YELLOW);
	newLine();
}

static void excepHandler(char * msg, RegisterContext *ctx){
	vdClear();
	
	// Imprimir el tipo de excepción
	vdPrint("====================================", COLOR_WHITE);
	newLine();
	vdPrint("   EXCEPTION DETECTED", COLOR_RED);
	newLine();
	vdPrint("====================================", COLOR_WHITE);
	newLine();
	newLine();
	
	vdPrint("Type: ", COLOR_WHITE);
	vdPrint(msg, COLOR_RED);
	newLine();
	newLine();
	
	// Mostrar el Instruction Pointer (RIP)
	vdPrint("Instruction Pointer (RIP): 0x", COLOR_WHITE);
	vdPrintHex(ctx->rip, COLOR_YELLOW);
	newLine();
	newLine();
	
	// Mostrar todos los registros de propósito general
	vdPrint("=== General Purpose Registers ===", COLOR_CYAN);
	newLine();
	printRegister("RAX", ctx->rax);
	printRegister("RBX", ctx->rbx);
	printRegister("RCX", ctx->rcx);
	printRegister("RDX", ctx->rdx);
	printRegister("RSI", ctx->rsi);
	printRegister("RDI", ctx->rdi);
	printRegister("RBP", ctx->rbp);
	printRegister("RSP", ctx->rsp);
	newLine();
	
	vdPrint("=== Extended Registers ===", COLOR_CYAN);
	newLine();
	printRegister("R8 ", ctx->r8);
	printRegister("R9 ", ctx->r9);
	printRegister("R10", ctx->r10);
	printRegister("R11", ctx->r11);
	printRegister("R12", ctx->r12);
	printRegister("R13", ctx->r13);
	printRegister("R14", ctx->r14);
	printRegister("R15", ctx->r15);
	newLine();
	
	vdPrint("=== Segment and Flags ===", COLOR_CYAN);
	newLine();
	printRegister("CS     ", ctx->cs);
	printRegister("SS     ", ctx->ss);
	printRegister("RFLAGS ", ctx->rflags);
	newLine();
	
	vdPrint("====================================", COLOR_WHITE);
	newLine();
	vdPrint("Press ENTER to continue...", COLOR_YELLOW);
	newLine();
	
	int c;
	_sti();
	do{
		_hlt();	
	} while((c=getCharFromBuffer()) != '\n');
	vdClear();
}
