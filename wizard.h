/* wizard.h */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

extern GtkWidget *do_assistant();
extern char psql_error[2048];
static const char *restricted[] { "%", "--", ";", "\n", "\"", "'", "(", ")", "[", "]", "{", "}", };

#endif
