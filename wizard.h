/* wizard.h */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

extern GtkWidget *do_assistant();
extern char psql_error[2048];
static const char *restricted[] {"%", "--", ";", "\n", "\"", "'", "(", ")", "[", "]", "{", "}"};
#endif
