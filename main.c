#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>

#define PROGRAM_NAME "hist"

#define ASCII_SIZE 256
#define BUFFER_SIZE 512
#define BAR_MAX_WIDTH 256

#define HISTOGRAM_SYMBOL_FILL '#'
#define HISTOGRAM_SYMBOL_BLANK '_'

#define GETOPT_HELP_CHAR -2
#define GETOPT_VERSION_CHAR -3

struct SymbolAmount {
    unsigned char symbol;
    size_t amount;
} ascii_symbols[ASCII_SIZE];

struct Bar {
    unsigned char symbol;
    size_t amount;
    size_t range;
    char data[BAR_MAX_WIDTH];
};

static int (*ptr_filter_func)(int) = &isprint;
static bool case_insensitive_flag = false;

int compare_symbols(const void *a, const void *b)
{
    size_t amount_a = ((struct SymbolAmount*)a)->amount;
    size_t amount_b = ((struct SymbolAmount*)b)->amount;
    return amount_b - amount_a;
}

void parse_symbols(const unsigned char *symbols)
{
    while (*symbols) {
        if ((*ptr_filter_func)(*symbols)) {
            char symbol = case_insensitive_flag ? toupper(*symbols) : *symbols;
            ascii_symbols[(int)symbol].symbol = symbol;
            ascii_symbols[(int)symbol].amount++;
        }
        symbols++;
    }
    return;
}

void parse_file(FILE *file)
{
    unsigned char buffer[BUFFER_SIZE];
    while (fgets((char*)buffer, BUFFER_SIZE, file))
        parse_symbols(buffer);
    fclose(file);
    return;
}

void draw_bar(struct Bar *bar, const size_t fill_range)
{
    for (size_t i = 0; i < bar->range; i++)
        bar->data[i] = (i < fill_range) ? HISTOGRAM_SYMBOL_FILL : HISTOGRAM_SYMBOL_BLANK;
    bar->data[bar->range] = '\0';
    printf("%c: [%s] (%lu)\n", bar->symbol, bar->data, bar->amount);
    return;
}

void build_histogram(const size_t histogram_range_max)
{
    qsort(ascii_symbols, ASCII_SIZE, sizeof(struct SymbolAmount), compare_symbols);

    struct Bar bar = {.range = histogram_range_max, .data = {0}};
    const size_t max_value = ascii_symbols[0].amount;
    for (unsigned char i = 0; ascii_symbols[i].amount > 0; i++) {
        const float scale = (float)ascii_symbols[i].amount / max_value;
        bar.symbol = ascii_symbols[i].symbol;
        bar.amount = ascii_symbols[i].amount;
        draw_bar(&bar, scale * histogram_range_max);
    }

    return;
}

void process_file(const char *path)
{
    FILE *ptr_file = fopen(path, "r");
    if (ptr_file)
        parse_file(ptr_file);
    else printf("%s: %s: No such file\n", PROGRAM_NAME, path);
    return;
}

int main(int argc, char *argv[])
{
    static struct option const long_options[] = {
        {"file", required_argument, NULL, 'f'},
        {"string", required_argument, NULL, 's'},
        {"letters-only", no_argument, NULL, 'l'},
        {"digits-only", no_argument, NULL, 'd'},
        {"alphanumeric-only", no_argument, NULL, 'a'},
        {"case-insensitive", no_argument, NULL, 'C'},
        {"version", no_argument, NULL, GETOPT_VERSION_CHAR},
        {"help", no_argument, NULL, GETOPT_HELP_CHAR},
    };

    int option;
    while ((option = getopt_long(argc, argv, "ldaCf:s:", long_options, NULL)) != -1) {
        switch (option) {
            case 'l':
                ptr_filter_func = &isalpha;
                break;
            case 'd':
                ptr_filter_func = &isdigit;
                break;
            case 'a':
                ptr_filter_func = &isalnum;
                break;
            case 'C':
                case_insensitive_flag = true;
                break;
            case 'f':
                process_file(optarg);
                break;
            case 's':
                parse_symbols((unsigned char*)optarg);
                break;
            case GETOPT_VERSION_CHAR:
                // TODO: Version message.
                printf("version\n");
                exit(EXIT_SUCCESS);
            case GETOPT_HELP_CHAR:
                // TODO: Help message.
                printf("help\n");
                exit(EXIT_SUCCESS);
            default:
                // TODO: Usage function.
                return EXIT_FAILURE;
        }
    }

    if (argc == 1)
        // TODO: Usage function.
        goto func_end;

    build_histogram(16);

func_end:
    return EXIT_SUCCESS;
}
