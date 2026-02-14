#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

typedef unsigned long long ull;

int splitting_read(char* content, char*** tokens_res);
bool get_arg_ull(char** args, int count_args, const char* arg_name, ull* value);
bool get_arg_str(char** args, int count_args, const char* arg_name, char* value, size_t max_len);
bool is_valid_number(const char* str);
ull gcd(ull a, ull b);
bool process_get_c(char** args, int count_args, FILE* output_file);
int get_prime_divs(ull n, ull** divs_res);
bool process_get_a(char** args, int count_args, FILE* output_file);
bool process_lcg(char** args, int count_args, FILE* output_file, ull *period);
bool frequency_test(char** args, int count_args, FILE* output_file);
bool gap_test(char **args, int count_args, FILE *output_file);

int main(void) {
    FILE* input_file = fopen("/Users/sanueee/vscode_projects/C_projects/ds/labs/input.txt", "r");
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
        fprintf(output_file, "incorrect command\n");
        free(content);
        fclose(input_file);
        fclose(output_file);
        return 0;
    }

    const char* command = tokens[0]; // команда
    size_t cmd_len = strlen(command); // длина команды
    char** args = tokens + 1; // массив строк с аргументами вида name=value
    int count_args = token_count - 1; // длина массива args
    ull period = 0;

    bool success = false;
    if (cmd_len == 5 && strncmp(command, "get_c", 5) == 0) {
        success = process_get_c(args, count_args, output_file);
    } else if (cmd_len == 5 && strncmp(command, "get_a", 5) == 0) {
        success = process_get_a(args, count_args, output_file);
    } else if (cmd_len == 3 && strncmp(command, "lcg", 3) == 0) {
        success = process_lcg(args, count_args, output_file, &period);
        fprintf(stdout, "период = %llu", period);
    } else if (cmd_len == 4 && strncmp(command, "freq", 4) == 0) {
        success = frequency_test(args, count_args, output_file);
    } else if (cmd_len == 3 && strncmp(command, "gap", 3) == 0) {
        success = gap_test(args, count_args, output_file);
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

    i = 0; int token_idx = 0;
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

ull gcd(ull a, ull b) {
    while (b != 0) {
        ull temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

bool process_get_c(char** args, int count_args, FILE* output_file) {
    ull cmin, cmax, m;

    if (!get_arg_ull(args, count_args, "cmin", &cmin) ||
        !get_arg_ull(args, count_args, "cmax", &cmax) ||
        !get_arg_ull(args, count_args, "m", &m)) {
            return false;
    }
    
    if (cmin > cmax || m == 0) {
        return false;
    }
    
    bool found = false;
    for (ull c = cmin; c <= cmax; c++) {
        if (gcd(c, m) == 1) {
            fprintf(output_file, "%llu\n", c);
            found = true;
        }
    }
    
    if (!found) {
        fprintf(output_file, "no solution");
    }
    
    return true;
}

int get_prime_divs(ull n, ull** divs_res) {
    if (n <= 1) {
        return 0;
    }
    
    ull* divs = malloc(64 * sizeof(ull));
    if (divs == NULL) {
        return -1;
    }
    
    int count = 0;
    if (n % 2 == 0) { // если двойка есть в простых делителях
        divs[count++] = 2;
        while (n % 2 == 0) {
            n /= 2;
        }
    }
    // когда мы доходим до составного числа i, число n уже не делится на i
    // все простые делители этого составного числа были убраны раньше
    for (ull i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            divs[count++] = i;
            while (n % i == 0) {
                n /= i;
            }
        }
    }
    
    if (n > 1) {
        divs[count++] = n;
    }
    
    *divs_res = divs;
    return count;
}

bool process_get_a(char** args, int count_args, FILE* output_file) {
    ull m;
    if (!get_arg_ull(args, count_args, "m", &m)) {
        return false;
    }
    if (m <= 1) {
        return false;
    }
    
    ull* divs_m = NULL;
    int divs_count = get_prime_divs(m, &divs_m);
    
    bool found = false;
    for (ull a = 2; a < m; a++) {
        ull b = a - 1;
        
        bool all_divide = true;
        for (int i = 0; i < divs_count; i++) {
            if (b % divs_m[i] != 0) {
                all_divide = false;
                break;
            }
        }
        
        if (!all_divide) {
            continue;
        }
        
        if (m % 4 == 0 && b % 4 != 0) {
            continue;
        }
        
        fprintf(output_file, "%llu\n", a);
        found = true;
        break;
    }
    
    if (!found) {
        fprintf(output_file, "no solution");
    }
    
    free(divs_m);
    return true;
}

bool process_lcg(char** args, int count_args, FILE* output_file, ull *period) {
    ull a, x0, c, m, n;
    
    if (!get_arg_ull(args, count_args, "a", &a) ||
        !get_arg_ull(args, count_args, "x0", &x0) ||
        !get_arg_ull(args, count_args, "c", &c) ||
        !get_arg_ull(args, count_args, "m", &m) ||
        !get_arg_ull(args, count_args, "n", &n)) {
        return false;
    }

    if (n == 0 || m == 0 || a >= m || c >= m || x0 >= m) {
        fprintf(output_file, "no solution");
        return true;
    }

    ull x = x0; bool found = false;
    for (ull i = 0; i < n; i++) {
        fprintf(output_file, "%llu\n", x);
        x = (a * x + c) % m;
        if (x == x0 && !found) {
            *period = i+1;
            found = true;
        }
    }
    
    return true;
}

bool frequency_test(char** args, int count_args, FILE* output_file) {
    char filename[256];
    if (!get_arg_str(args, count_args, "inp", filename, sizeof(filename))) {
        fprintf(output_file, "incorrect command");
        return false;
    }

    FILE* input = fopen(filename, "r");
    if (!input) {
        fprintf(output_file, "incorrect command");
        return false;
    }

    ull* numbers = malloc(1024 * sizeof(ull));
    size_t capacity = 1024;
    size_t count = 0;

    ull num;
    while (fscanf(input, "%llu", &num) == 1) {
        if (count >= capacity) {
            capacity *= 2;
            numbers = realloc(numbers, capacity * sizeof(ull));
        }
        numbers[count++] = num;
    }
    fclose(input);

    ull min_val = numbers[0];
    ull max_val = numbers[0];
    for (size_t i = 1; i < count; i++) {
        if (numbers[i] < min_val) min_val = numbers[i];
        if (numbers[i] > max_val) max_val = numbers[i];
    }

    const int k = 10;
    int freq[10] = {0};
    
    long double range = (long double)(max_val - min_val);
    for (size_t i = 0; i < count; i++) {
        long double normalized = (long double)(numbers[i] - min_val) / range;
        int interval = (int)(normalized * k);
        if (interval >= k) {
            interval = k - 1;
        }
        freq[interval]++;
    }
    
    long double expected = (long double)count / k;
    long double phi_square = 0.0;    
    for (int i = 0; i < k; i++) {
        phi_square += (freq[i] - expected) * (freq[i] - expected) / expected;
    }
    
    // табличные значения стр. 58
    const long double phi_lower = 3.325;
    const long double phi_upper = 16.92;
    
    fprintf(output_file, "n=%zu\n", count);
    fprintf(output_file, "[%llu, %llu]\n", min_val, max_val);
    fprintf(output_file, "k=%d\n", k);
    fprintf(output_file, "v=%d\n", k - 1);
    
    for (int i = 0; i < k; i++) {
        ull start = min_val + (ull)(range * i / k);
        ull end = min_val + (ull)(range * (i + 1) / k);
        fprintf(output_file, "frequency in №%d - [%llu , %llu): %5d\n", i+1, start, end, freq[i]); // частоты интервалов
    }
    
    fprintf(output_file, "x^2=%.4Lf\n", phi_square); // фи квадрат   
    fprintf(output_file, "result: ");
    if (phi_square >= phi_lower && phi_square <= phi_upper) {
        fprintf(output_file, "random\n");
        fprintf(output_file, "  (%.3Lf <= %.4Lf <= %.3Lf)\n", phi_lower, phi_square, phi_upper);
    } else {
        fprintf(output_file, "NOT random\n");
    }
    
    free(numbers);
    return true;
}

bool gap_test(char **args, int count_args, FILE *output_file)
{
    char filename[256];

    if (!get_arg_str(args, count_args, "inp", filename, sizeof(filename)))
    {
        fprintf(output_file, "incorrect command");
        return false;
    }

    FILE *input = fopen(filename, "r");
    if (!input)
    {
        fprintf(output_file, "incorrect command");
        return false;
    }

    ull max_value = 0ULL;
    ull temp;

    while (fscanf(input, "%llu", &temp) == 1)
        if (temp > max_value)
            max_value = temp;

    if (max_value == 0)
    {
        fclose(input);
        fprintf(output_file, "empty file\n");
        return false;
    }

    ull modulus = max_value + 1;
    rewind(input);

    long double alpha = .0;
    long double beta = .5;
    int n = 500; // сколько интервалов нужно найти
    int t = 5;   // интервалы длиной >= t вместе с count[i]

    // инициализация
    int j = 0;
    int s = 0;

    int *count = calloc(t + 1, sizeof(int));
    if (!count)
    {
        fclose(input);
        fprintf(output_file, "memory error");
        return false;
    }

    /* имем, когда a <= Uj <= bet */
    while (s < n)
    {
        int r = 0;
        long double Uj;

        while (1)
        {
            ull raw_value;
            if (fscanf(input, "%llu", &raw_value) != 1)
            {
                fclose(input);
                free(count);
                fprintf(output_file, "not enough data");
                return false;
            }

            Uj = (long double)raw_value / (long double)modulus;
            j++;

            if (Uj >= alpha && Uj < beta)
            {
                break;
            }
            r++;
        }

        if (r >= t)
        {
            count[t]++;
        }
        else
        {
            count[r]++;
        }
        s++;
    }

    long double p = beta - alpha, pr;
    long double chi_square = 0.0L;

    for (int r = 0; r <= t; r++)
    {
        pr = r < t ? p * powl(1.0 - p, r) : powl(1.0 - p, t);

        long double expected = n * pr;   // ожидамое
        long double observed = count[r]; // наблюдаемов

        chi_square += (observed - expected) * (observed - expected) / expected;
    }

    // для каждого r вычисляем вероятность pr
    // expected = n * pr -> сколько должно быть интвервалов длиной r

    // k - 1 = (t + 1) - 1 = t
    int df = t;

    long double chi = 11.07;

    fprintf(output_file, "a = %.2Lf, b = %.2Lf\n", alpha, beta);
    fprintf(output_file, "n = %d intervals found\n", n);
    fprintf(output_file, "\nintervals lengths distribution:\n");
    for (int r = 0; r <= t; r++)
    {
        if (r < t)
            fprintf(output_file, "Length %d: %d\n", r, count[r]);
        else
            fprintf(output_file, "Length ≥%d: %d\n", t, count[r]);
    }

    fprintf(output_file, "\nX2 = %.4Lf\n", chi_square);
    fprintf(output_file, "critical value (df=%d, a = 0.05): %.4Lf\n", df, chi);
    fprintf(output_file, "test result: %s\n",
            chi_square < chi ? "random" : "not random");

    free(count);
    fclose(input);

    return true;
}