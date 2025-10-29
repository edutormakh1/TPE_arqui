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
// Función para ajustar hora BCD
static uint8_t adjust_hour_bcd(uint8_t hour_bcd, int offset) {
    // Convertir BCD a decimal
    int hour_decimal = ((hour_bcd >> 4) * 10) + (hour_bcd & 0x0F);
    
    // Aplicar offset
    hour_decimal += offset;
    
    
    if (hour_decimal < 0) {
        hour_decimal += 24;
    } else if (hour_decimal >= 24) {
        hour_decimal -= 24;
    }
    
    // Convertir de vuelta a BCD
    return ((hour_decimal / 10) << 4) | (hour_decimal % 10);
}

//helper para fechas y horas 
static void print_formatted_time_date(uint8_t* buffer, char separator){
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
        if (i < 2) {
            output_buffer[3*i + 2] = separator;
        }
    }
    output_buffer[8] = '\n';
    output_buffer[9] = 0;
    shell_print_string(output_buffer);
}

void print_time(){
    uint8_t time_buffer[3];
    
    sys_time(time_buffer);
    
    // Ajustar solo la hora a GMT-3 en formato BCD
    time_buffer[0] = adjust_hour_bcd(time_buffer[0], -3);
    
    print_formatted_time_date(time_buffer, ':');
}

void print_date(){
    uint8_t time_buffer[3];
    uint8_t date_buffer[3];
    
    sys_time(time_buffer);
    sys_date(date_buffer);
    
    // Ver si necesitamos ajustar la fecha
    int original_hour = ((time_buffer[0] >> 4) * 10) + (time_buffer[0] & 0x0F);
    if (original_hour < 3) {
        // La hora ajustada sería del día anterior
        int day_decimal = ((date_buffer[0] >> 4) * 10) + (date_buffer[0] & 0x0F);
        day_decimal--;
        
        if (day_decimal <= 0) {
            day_decimal = 30; // Simplificado a mes de 30 dias
            int month_decimal = ((date_buffer[1] >> 4) * 10) + (date_buffer[1] & 0x0F);
            month_decimal--;
            
            if (month_decimal <= 0) {
                month_decimal = 12;
                int year_decimal = ((date_buffer[2] >> 4) * 10) + (date_buffer[2] & 0x0F);
                year_decimal--;
                date_buffer[2] = ((year_decimal / 10) << 4) | (year_decimal % 10);
            }
            date_buffer[1] = ((month_decimal / 10) << 4) | (month_decimal % 10);
        }
        date_buffer[0] = ((day_decimal / 10) << 4) | (day_decimal % 10);
    }
    
    print_formatted_time_date(date_buffer, '/');
}
void play_tron(){
    shell_print_string("Tron game not implemented yet\n");
}
//quedan comandos de tron y demas por implementar
