#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* kind: 'w' word, 'o' operator, 'n' IO number */
typedef struct {
    char kind;
    char *text;
    int quoted;
} Tok;

/* Operators, longest first. A real tokenizer must find these without spaces around them. */
static const char *OPS[] = {"<<-", "&&", "||", ">>", "<<", ">&", "<&", "|", "&", ";", "<", ">"};
#define NOPS 12
#define MAXTOK 2048

static char *xdup(const char *s) {
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    memcpy(p, s, n + 1);
    return p;
}

/* Split one command line into toks. Return the token count, or -1 with *err set to
   "unterminated quote" or "trailing backslash". */
static int tokenize(const char *line, Tok *toks, const char **err) {
    /* TODO: replace this space split with a character-by-character walk that handles
       quotes, backslashes, operators, IO numbers and comments. */
    int nt = 0;
    const char *p = line;
    (void)err;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        const char *q = p;
        while (*q && *q != ' ' && *q != '\t') q++;
        char *w = (char *)malloc((size_t)(q - p) + 1);
        memcpy(w, p, (size_t)(q - p));
        w[q - p] = 0;
        toks[nt].kind = 'w';
        toks[nt].text = w;
        toks[nt].quoted = 0;
        nt++;
        p = q;
    }
    return nt;
}

/* Print one token the way the lessons show it: [word] (operator) {io-number}. */
static void print_tok(const Tok *t) {
    char open = t->kind == 'w' ? '[' : (t->kind == 'o' ? '(' : '{');
    char close = t->kind == 'w' ? ']' : (t->kind == 'o' ? ')' : '}');
    printf("%c%s%c", open, t->text, close);
}

int main(void) {
    static char line[8192];
    static Tok toks[MAXTOK];
    while (fgets(line, sizeof line, stdin)) {
        size_t n = strlen(line);
        if (n > 0 && line[n - 1] == '\n') line[--n] = 0;
        if (n == 0) continue;
        const char *err = "";
        int nt = tokenize(line, toks, &err);
        if (nt < 0) {
            printf("ERR %s\n", err);
            continue;
        }
        if (nt == 0) continue;
        for (int i = 0; i < nt; i++) {
            if (i > 0) putchar(' ');
            print_tok(&toks[i]);
        }
        putchar('\n');
    }
    return 0;
}
