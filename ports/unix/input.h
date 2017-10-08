#ifndef MICROPY_INCLUDED_UNIX_INPUT_H
#define MICROPY_INCLUDED_UNIX_INPUT_H

char *prompt(char *p);
void prompt_read_history(void);
void prompt_write_history(void);

#endif // MICROPY_INCLUDED_UNIX_INPUT_H
