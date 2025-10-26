#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <keyboard.h>
#include <time.h>
#include <idtLoader.h>
#include <video_driver.h>
#include <keyboard.h>
#include <sound.h>

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

//LOS BEEP NO ESTAN FUNCIONANDO
		// // Beep de 440 Hz (nota LA) durante 500ms
		// beep(440, 500);

		// // Beep de error (frecuencia baja)
		// beep(200, 1000);

		// // Beep de éxito (frecuencia alta)
		// beep(1000, 200);

		// // Melodía simple
		// beep(262, 250); // Do
		// beep(294, 250); // Re
		// beep(330, 250); // Mi
		// beep(349, 250); // Fa
		// beep(392, 250); // Sol

		// Input de una línea con eco gráfico usando la fuente 8x16
		char buffer[128];
		drawString("modo video", 20, 40, 0x00FFFFFF, 1);
		drawString("Escriba algo y presione Enter:", 20, 80, 0x00FFFFFF, 1);
		uint32_t x = 20; uint32_t y = 110;
		readLineVBE(buffer, 128, &x, y, 0x00FFFFFF);
		// Mostrar lo que se ingresó debajo
		drawString("Ingresaste:", 20, y + 20, 0x00FFFFFF, 1);
		drawString(buffer, 20, y + 40, 0x00FFFFFF, 3);
		


    return 0;
}
