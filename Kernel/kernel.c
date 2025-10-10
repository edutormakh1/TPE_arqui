#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <time.h>
#include <idtLoader.h>
#include "keyboard.h"
#include "vgaDriver.h"
#include <vgaText.h>

// Forward declaration for the VGA demo function implemented in vgaDemo.c
//void vga_demo(void);



extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

// Busy-wait sleep using the PIT timer seconds counter
static void sleep(int seconds) {
	if (seconds <= 0) return;
	int start = seconds_elapsed();
	while ((seconds_elapsed() - start) < seconds) {
		// spin
	}
}


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{	

	load_idt();

	ncClear();
	ncNewline();
	ncPrintStyle("Arquitectura de computadoras", 0x30);
	ncNewline();

	uint8_t time_buffer[3];
	getTime(time_buffer);
	// time_buffer: [hours, minutes, seconds]
	ncPrintHex(time_buffer[0]);
	ncPrint(":");
	ncPrintHex(time_buffer[1]);
	ncPrint(":");
	ncPrintHex(time_buffer[2]);
	ncNewline();

	ncPrint("Kernel inicializado correctamente");

	ncPrint("Presiona 'g' para modo grafico VGA o cualquier otra tecla para continuar:");
	ncNewline();
	
	// Read ASCII key, ignoring break and non-printables
	char key = 0;
	while (key == 0) {
		key = readKeyAsciiBlocking();
	}
	if (key == 'g' || key == 'G') {
		ncPrint("Cambiando a modo grafico VGA...");
		ncNewline();
		sleep(2);
		
		// Initialize VGA graphics mode
		if (vga_set_mode_13h() == 0) {
			// Optional: Run the VGA demo
			

			// Simple graphics text input: prompt and read a line
			// Draw prompt
			vga_set_default_palette();
			const char *prompt = "Ingrese texto y presione Enter:";
			vga_draw_string(8, 8, prompt, 15);

			// Input buffer
			char buffer[128];
			vga_text_input(8, 24, buffer, sizeof(buffer), 14);

			// Echo the result below
			vga_draw_string(8, 40, "Ingresado:", 11);
			vga_draw_string(8 + 9*8, 40, buffer, 10);

			// Wait for any key press (ASCII) before continuing
			(void)readKeyAsciiBlocking();
		}
		
		// The screen will need to be re-initialized here if you want to return to text mode
		// For now, we'll just hang
		while (1);
	}
	/*
	ncPrint("[TEXT]: ");
	// // pollForPressedKey();
	 while (1); // para que no se quede colgada
	*/
	
	


	return 0;
}