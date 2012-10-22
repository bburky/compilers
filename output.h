#ifndef OUTPUT_H
#define OUTPUT_H

FILE *listing_file, *symbol_file;

void init_output(const char *filename);

void write_listing_line(int lineno, const char *line);

void write_symbol_token(int lineno, token tok);

#endif
