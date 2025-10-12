#include "shell.h"

static Command commands[] = {
    {"help", help},
    {"cls", cls},
    {"print_time", print_time},
    {"print_date", print_date},
    {"print_saved_registers", print_saved_registers},
    {"play_tron", play_tron},
    {"test_invalid_opcode", test_invalid_opcode},
    {"test_divide_by_zero", test_divide_by_zero},
    {0, 0} // Marca el final de la lista
}; //lo de usar el 0 como marca de fin no se si es muy correcto

// Función para buscar y ejecutar comandos
void process_line(char * line, uint32_t * history_len) {

    if (strlen(line) == 0) {
        return;
    }

    for (int i = 0; commands[i].name != 0; i++) {
        if (strcmp(line, commands[i].name) == 0) {
            commands[i].function();
            return;
        }
    }
    
    shell_print_err(ERROR_MSG);
}

void help(){
    shell_print_string(HELP_MSG);
    shell_print_string("Comandos disponibles:\n");
    for (int i = 0; commands[i].name != 0; i++) {
        shell_print_string("  \'");
        shell_print_string(commands[i].name);
        shell_print_string(" \'");
    }
    shell_newline();
}

void cls(){
    sys_clear();

}
void test_divide_by_zero(){
    cls();
    int a = 1;
    int b = 0;
    a= a / b; // Esto genera la excepcion   
}
void test_invalid_opcode(){
    cls();
    generate_invalid_opcode();
}

void print_saved_registers(){
    char buffer[REGSBUF_SIZE];
    if(sys_regs(buffer)){
        shell_print_string(buffer);
    } else {
        shell_print_err("No se han guardado registros\n");
    }
}
//helper para fechas y horas
static void print_formatted_time_date(void (*syscall_fn)(uint8_t*), char separator){
    uint8_t buffer[3]; // 2 digits + null terminator
    syscall_fn(buffer);
    char number_buffer[4];
    char output_buffer[10]; // "HH:MM:SS\n\0" or "DD/MM/YY\n\0"
    for (int i = 0; i < 3; i++) {
        uint64_t digits = num_to_str(buffer[i], number_buffer, 16);
        if (digits == 1) {
            output_buffer[3*i] = '0';
            output_buffer[3*i + 1] = number_buffer[0];
        } else {
            output_buffer[3*i] = number_buffer[0];
            output_buffer[3*i + 1] = number_buffer[1];
        }
        output_buffer[3*i + 2] = separator;
    }
    output_buffer[8] = '\n';
    output_buffer[9] = 0;
    shell_print_string(output_buffer);
}
void print_time(){
    print_formatted_time_date(sys_time, ':');
}

void print_date(){
    print_formatted_time_date(sys_date, '/');

}
void play_tron(){
    shell_print_string("Juego Tron no implementado aún\n");
}
//quedan comandos de tron y demas por implementar
