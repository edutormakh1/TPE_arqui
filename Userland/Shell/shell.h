#include "../Userlib/userlib.h"

#define INPUT_MAX 100
#define HISTORY_MAX 4096
#define PROMPT "TEM_shell>" 
#define CURSOR '_'
#define ERROR_MSG "Use command 'help' to see available commands\n"
#define HELP_MSG "To change font size press + or -\n Available commands: \n"

//notas para el play_song
#define N_DO4     262
#define N_RE4     294
#define N_MI4     330
#define N_FA4     349
#define N_SOL4    392
#define N_LA4     440
#define N_SI4     494
#define N_DO5     523
#define N_RE5     587
#define N_MI5     659

typedef struct{
    char character;
    uint64_t fd; //stdout o stderr
}HistoryEntry;

typedef void (*Runnable)(void);

typedef struct Command{
     char* name;
    Runnable function;
}Command;

//funciones de la shell
void shell_putchar(char c, uint64_t fd);
void shell_print_string(char *str);
void shell_print_err(char *str);
void shell_newline();
void read_line(char * buf, uint64_t max);
void process_line(char * line, uint32_t * history_len);
void incfontsize();
void decfontsize();
void redraw_history();

//comandos
void help();
void cls();
void print_time();
void print_date();
void print_saved_registers();
void play_tron();
void test_invalid_opcode();
void test_divide_by_zero();
void play_song();
//benchmarks
void benchmark_fps();          
void benchmark_cpu();          
void benchmark_memory();       
void benchmark_keyboard();     
