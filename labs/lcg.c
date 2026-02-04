#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef unsigned long long ull;

int splitting_read(char* content, char*** tokens_res);
bool get_arg(char** tokens, int token_count, const char* arg_name, char* value, size_t max_len);
bool get_arg_ull(char** args, int count_args, const char* arg_name, ull* value);
bool get_arg_str(char** args, int count_args, const char* arg_name, char* value, size_t max_len);
bool is_valid_number(const char* str);
bool process_get_c(char** args, int count_args, FILE* output_file);
bool process_get_a(char** args, int count_args, FILE* output_file);
bool process_lcg(char** args, int count_args, FILE* output_file);
bool process_test(char** args, int count_args, FILE* output_file);

int main(void) {
    FILE* input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        return 1;
    }

    FILE* output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        return 1;
    }

    char* content = NULL; size_t len_content = 0; // содержимое строки (команда и аргументы)
    ssize_t read_res = getline(&content, &len_content, input_file);

    if (read_res == -1) { // команда не распознана 
        fprintf(output_file, "incorrect command");
        free(content);
        fclose(input_file);
        fclose(output_file);
        return 0;
    }

    char** tokens = NULL; // разбитые команда + аргументы
    int token_count = splitting_read(content, &tokens);

    if (token_count <= 1) {
        fprintf(output, "incorrect command\n");
        free(content);
        fclose(input_file);
        fclose(output_file);
        return 0;
    }

    const char* command = tokens[0]; // команда
    size_t cmd_len = strlen(command); // длина команды
    char** args = tokens + 1; // массив строк с аргументами вида name=value
    int count_args = token_count - 1; // длина массива args

    bool success = false;
    if (cmd_len == 5 && strncmp(command, "get_c", 5) == 0) {
        success = process_get_c(args, count_args, output_file);
    } else if (cmd_len == 5 && strncmp(command, "get_a", 5) == 0) {
        success = process_get_a(args, count_args, output_file);
    } else if (cmd_len == 3 && strncmp(command, "lcg", 3) == 0) {
        success = process_lcg(args, count_args, output_file);
    } else if (cmd_len == 4 && strncmp(command, "test", 4) == 0) {
        success = process_test(args, count_args, output_file);
    } else {
        success = false;
    }

    if (!success) {
        fprintf(output_file, "incorrect command");
    }

    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
    free(content);
    fclose(input_file);
    fclose(output_file);

    return 0;
}

int splitting_read(char* content, char*** tokens_res) { // дробим content на tokens
    int count = 0; int i = 0;
    while (content[i] != '\0') {
        while (content[i] == ' ' || content[i] == '\t' || content[i] == '\n') {
            i++;
        }
        if (content[i] == '\0') break;
        count++;
        while (content[i] != ' ' && content[i] != '\t' && content[i] != '\n' && content[i] != '\0') {
            i++;
        }
    }

    if (count == 0) {
        return 0;
    }

    char** tokens = malloc(count * sizeof(char*));
    if (tokens == NULL) {
        return -1;
    }

    int i = 0; int token_idx = 0;
    while (content[i] != '\0') {
        while (content[i] == ' ' || content[i] == '\t' || content[i] == '\n') {
            i++;
        }
        if (content[i] == '\0') break;
        int start = i; // начало токена
        while (content[i] != ' ' && content[i] != '\t' && content[i] != '\n' && content[i] != '\0') {
            i++;
        }
        int token_len = i - start; // длина текущего токена
        tokens[token_idx] = malloc((token_len + 1) * sizeof(char));
        if (tokens[token_idx] == NULL) { // проверка реза маллока чуть что почистим что уже добавили
            for (int j = 0; j < token_idx; j++) {
                free(tokens[j]);
            }
            free(tokens);
            return -1;
        }
        strncpy(tokens[token_idx], content + start, token_len);
        tokens[token_idx][token_len] = '\0';
        token_idx++;
    }
    *tokens_res = tokens; // записываем во "внешний" массив 
    return count; 
}

bool get_arg(char** tokens, int token_count, const char* arg_name, char* value, size_t max_len) {
    for (int i = 0; i < count; i++) {
        char* eq = strchr(tokens[i], '=');
        if (eq == NULL) {
            continue;
        }
        int name_len = eq - tokens[i];
        if (name_len != strlen(name)) {
            continue;
        }
        if (strncmp(tokens[i], name, name_len) != 0) {
            continue;
        }
        const char* value_str = eq + 1;
        if (strlen(value_str) == 0 || strlen(value_str) >= max_len) {
            return false;
        }
        strcpy(value, value_str);
        return true;
    }
    return false;
}

bool get_arg_ull(char** args, int count_args, const char* arg_name, ull* value) {
    // по arg_name в args находим и записываем в value (для всех кроме test - там после "=" идет строка)
    for (int i = 0; i < count_args; i++) {
        char* eq = strchr(args[i], '=');

        if (eq == NULL) { // если не осталось = для проверки
            continue;
        }

        int name_len = eq - args[i]; 
        if (name_len != strlen(arg_name)) { // не сходятся по длине
            continue;
        }

        if (strncmp(args[i], arg_name, name_len) != 0) { // не идентичны
            continue;
        }

        const char* value_str = eq + 1;
        if (!is_valid_number(value_str)) {
            return false;
        }

        char* endptr;
        *value = strtoull(value_str, &endptr, 10);
        if (*endptr != '\0') {
            return false;
        }
        return true;
    }
    return false;
}

bool get_arg_str(char** args, int count_args, const char* arg_name, char* value, size_t max_len) {
    // нужна для test там после "=" идет строка
    for (int i = 0; i < count_args; i++) {
        char* eq = strchr(args[i], '=');

        if (eq == NULL) {
            continue;
        }

        int name_len = eq - args[i];
        if (name_len != strlen(arg_name)) {
            continue;
        }

        if (strncmp(args[i], arg_name, name_len) != 0) {
            continue;
        }

        const char* value_str = eq + 1;
        if (strlen(value_str) == 0 || strlen(value_str) >= max_len) {
            return false;
        }

        strcpy(value, value_str);
        return true;
    }
    return false;
}

bool is_valid_number(const char* str) {
    if (*str == '\0') {
        return false;
    }
    for (const char* p = str; *p; p++) {
        if (!isdigit(*p)) {
            return false;
        }
    }
    return true;
}

bool process_get_c(char** args, int count_args, FILE* output_file) {

}

bool process_get_a(char** args, int count_args, FILE* output_file) {
    
}

bool process_lcg(char** args, int count_args, FILE* output_file) {
    
}

bool process_test(char** args, int count_args, FILE* output_file) {
    
}


