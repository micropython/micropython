//----------------------------------------------------------------------------//
#ifndef __MAIN_TEST_H
#define __MAIN_TEST_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Exported test functions ------------------------------------------------------- */
void do_ping_test(char *ip, int size, int count, int interval);
void do_ping_call(char *ip, int loop, int count);
void interactive_question(char *question, char *choice, char *buf, int buf_size);
void start_interactive_mode(void);

#ifdef __cplusplus
  }
#endif

#endif // __MAIN_TEST_H

//----------------------------------------------------------------------------//
