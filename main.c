#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define ASCII_SIZE 256
#define BUFFER_SIZE 512

#define HISTOGRAM_SYMBOL_FILL '#'
#define HISTOGRAM_SYMBOL_BLANK '_'

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
    if (argc == 1)
        goto func_end;

    FILE *ptr_file = fopen(argv[argc - 1], "r");

    if (!ptr_file) {
        printf("(!) Not able to open the file.\n");
        goto func_end;
    }

    unsigned char buffer[BUFFER_SIZE];
    while (fgets((char*)buffer, BUFFER_SIZE, ptr_file))
        parse_symbols(buffer);
    fclose(ptr_file);

    build_histogram(16);

func_end:
    return EXIT_SUCCESS;
}
