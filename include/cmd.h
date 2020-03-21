#ifndef __CMD_H__
#define __CMD_H__

#define HELP_LONG  "help"
#define HELP_SHORT 'h'

typedef enum {
	ARG_NONE,
	ARG_HANDLE,
	ARG_INT,
	ARG_FLOAT,
	ARG_DOUBLE,
	ARG_STR
} arg_type;
typedef enum {
	OPT_SUCCESS,
	OPT_INVALID,
	OPT_MISPLACED,
	OPT_OTHER_ERROR,
	ARG_BAD_VALUE,
	ARG_MISSING,
	ARG_SHOULD_NOT_BE
} opt_result;
typedef enum {
	COMMAND_SUCCESS,
	COMMAND_INVALID,
	COMMAND_MISSING
} cmd_result;
typedef struct b_command_s b_command;
typedef struct b_option_s b_option;
typedef struct b_cmd_context_s b_cmd_context;
typedef opt_result (* arg_handle)(const char*, const void*);

struct b_command_s {
	char *name;
	char *tip;
	int (*handler)(int argc, char **argv);
};
struct b_option_s {
	char *long_name;
	char short_name;
	char *tip;

	arg_type type;
	void *arg;
	char *arg_name;
};

#ifdef __CMD_INTERNAL
struct b_cmd_context_s {
	char *name;
	char *usage;
	char *description;
	char *epilog;

	b_option *options;
	b_command *commands;

	_Bool print_errors;
	_Bool handle_help;
};
#endif

int command_len(const b_command *commands);
int option_len(const b_option *options);

b_command *cat_commands(const b_command *first, const b_command *second);
b_option *cat_options(const b_option *first, const b_option *second);

_Bool command_is_null(b_command command);
_Bool option_is_null(b_option option);

b_cmd_context *new_context(char *name);

void set_name(b_cmd_context *context, char *name);
void set_usage(b_cmd_context *context, char *usage);
void set_description(b_cmd_context *context, char *description);
void set_epilog(b_cmd_context *context, char *epilog);
void set_print_errors(b_cmd_context *context, _Bool print_errors);
void set_handle_help(b_cmd_context *context, _Bool handle_help);

void push_commands(b_cmd_context *context, const b_command *commands);
void push_options(b_cmd_context *context, const b_option *options);

void clear_commands(b_cmd_context *context);
void clean_options(b_cmd_context *options);

void delete_context(b_cmd_context **context);

opt_result parse_options(b_cmd_context *context, int *argc, char **argv);
cmd_result extract_command(b_cmd_context *context, int *argc, char **argv,
						   b_command *found_command);

void print_help(b_cmd_context *context);

#ifdef __CMD_INTERNAL
void move_to_last(int i, int argc, char **argv);
#endif

#endif
