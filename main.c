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

static int (*ptr_filter_symbols_func)(int) = &isprint;
static int (*ptr_filter_case_func)(int) = &isprint;
static bool case_insensitive_flag = false;

void usage()
{
    printf("Usage: %s [OPTION]... FILE\n", PROGRAM_NAME);
    fputs("Counts printable characters in a file and creates a histogram.\n", stdout);
    fputs(
        "\n\
    -l, --letters-only              count only letters\n\
    -d, --digits-only               count only decimal digits\n\
    -a, --alphanumeric-only         count only letters and decimal digits\n\
    -p, --punctuation-only          count only punctuation marks\n\
    -L, --lowercase-letters-only    count only lowercase letters among letters\n\
    -U, --uppercase-letters-only    count only uppercase letters among letters\n\
    -C, --case-ignore               count identical letters with different case as a single character\n\
    -r, --range=SIZE                set the histogram range\n\
        --help                      display this help and exit\n\
        --version                   output version information and exit\n\
        \n", stdout
    );
    printf(
        "Examples:\n\
    %s file.txt     Count all printed characters in the file and display a histogram.\n\
    %s -aC file.txt Count only letters and digits in the file, ignoring case, and display a histogram.\
        \n", PROGRAM_NAME, PROGRAM_NAME
    );
}

int compare_symbols(const void *a, const void *b)
{
    size_t amount_a = ((struct SymbolAmount*)a)->amount;
    size_t amount_b = ((struct SymbolAmount*)b)->amount;
    return amount_b - amount_a;
}

void parse_symbols(const unsigned char *symbols)
{
    while (*symbols) {
        char symbol = *symbols;
        symbols++;
        if ((*ptr_filter_symbols_func)(symbol)) {
            if (isalpha(symbol) && !(*ptr_filter_case_func)(symbol))
                continue;
            if (case_insensitive_flag)
                symbol = toupper(symbol);
            ascii_symbols[(int)symbol].symbol = symbol;
            ascii_symbols[(int)symbol].amount++;
        }
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

int main(int argc, char *argv[])
{
    int histogram_range = 32;

    static struct option const long_options[] = {
        {"letters-only", no_argument, NULL, 'l'},
        {"digits-only", no_argument, NULL, 'd'},
        {"alphanumeric-only", no_argument, NULL, 'a'},
        {"punctuation-only", no_argument, NULL, 'p'},
        {"lowercase-letters-only", no_argument, NULL, 'L'},
        {"uppercase-letters-only", no_argument, NULL, 'U'},
        {"case-ignore", no_argument, NULL, 'C'},
        {"range", required_argument, NULL, 'r'},
        {"version", no_argument, NULL, GETOPT_VERSION_CHAR},
        {"help", no_argument, NULL, GETOPT_HELP_CHAR},
    };

    int option;
    while ((option = getopt_long(argc, argv, "ldapLUCr:", long_options, NULL)) != -1) {
        switch (option) {
            case 'l':
                ptr_filter_symbols_func = &isalpha;
                break;
            case 'd':
                ptr_filter_symbols_func = &isdigit;
                break;
            case 'a':
                ptr_filter_symbols_func = &isalnum;
                break;
            case 'p':
                ptr_filter_symbols_func = &ispunct;
                break;
            case 'L':
                ptr_filter_case_func = &islower;
                break;
            case 'U':
                ptr_filter_case_func = &isupper;
                break;
            case 'C':
                case_insensitive_flag = true;
                ptr_filter_case_func = &isprint;
                break;
            case 'r':
                histogram_range = atoi(optarg);
                if (histogram_range > BAR_MAX_WIDTH || histogram_range < 1) {
                    printf("%s: %s: The range must be between 0 and %d\n", PROGRAM_NAME, optarg, BAR_MAX_WIDTH);
                    exit(EXIT_FAILURE);
                }
                break;
            case GETOPT_VERSION_CHAR:
                // TODO: Version message.
                printf("version\n");
                exit(EXIT_SUCCESS);
            case GETOPT_HELP_CHAR:
                usage();
                exit(EXIT_SUCCESS);
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (argc == 1) {
        usage();
        exit(EXIT_FAILURE);
    }

    FILE *ptr_file = fopen(argv[argc - 1], "r");
    if (ptr_file)
        parse_file(ptr_file);
    else printf("%s: %s: No such file\n", PROGRAM_NAME, argv[argc - 1]);

    build_histogram(histogram_range);

    return EXIT_SUCCESS;
}
