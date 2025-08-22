#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>

#define ASCII_SIZE 256
#define BUFFER_SIZE 512

#define HISTOGRAM_SYMBOL_FILL '#'
#define HISTOGRAM_SYMBOL_BLANK '_'

#define GETOPT_HELP_CHAR -2
#define GETOPT_VERSION_CHAR -3

struct SymbolAmount {
    unsigned char symbol;
    size_t amount;
} ascii_symbols[ASCII_SIZE];

int compare_symbols(const void *a, const void *b)
{
    size_t amount_a = ((struct SymbolAmount*)a)->amount;
    size_t amount_b = ((struct SymbolAmount*)b)->amount;
    return amount_b - amount_a;
}

void parse_symbols(const unsigned char *symbols)
{
    while (*symbols) {
        if (!iscntrl(*symbols)) {
            ascii_symbols[*symbols].symbol = *symbols;
            ascii_symbols[*symbols].amount++;
        }
        symbols++;
    }
    return;
}

void parse_file(const char *path)
{
    FILE *ptr_file = fopen(path, "r");

    unsigned char buffer[BUFFER_SIZE];
    while (fgets((char*)buffer, BUFFER_SIZE, ptr_file))
        parse_symbols(buffer);
    fclose(ptr_file);

    return;
}

void draw_bar(const unsigned char symbol, const size_t amount, const size_t range, const size_t range_max)
{
    char bar[range_max + 1];
    for (size_t i = 0; i < range_max; i++)
        bar[i] = (i < range) ? HISTOGRAM_SYMBOL_FILL : HISTOGRAM_SYMBOL_BLANK;
    printf("%c: [%s] (%lu)\n", symbol, bar, amount);
    return;
}

void build_histogram(const size_t histogram_range_max)
{
    qsort(ascii_symbols, ASCII_SIZE, sizeof(struct SymbolAmount), compare_symbols);

    const size_t max_value = ascii_symbols[0].amount;
    for (unsigned char i = 0; ascii_symbols[i].amount > 0; i++) {
        const float scale = (float)ascii_symbols[i].amount / max_value;
        draw_bar(ascii_symbols[i].symbol, ascii_symbols[i].amount, scale * histogram_range_max, histogram_range_max);
    }
    return;
}

int main(int argc, char *argv[])
{
    static struct option const long_options[] = {
        {"file", required_argument, NULL, 'f'},
        {"line", required_argument, NULL, 'l'},
        {"version", no_argument, NULL, GETOPT_VERSION_CHAR},
        {"help", no_argument, NULL, GETOPT_HELP_CHAR},
    };

    int option;
    while ((option = getopt_long(argc, argv, "f:l:", long_options, NULL)) != -1) {
        switch (option) {
            case 'f':
                // TODO: File open validation.
                parse_file(optarg);
                break;
            case 'l':
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
