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
typedef struct bcommand_s bcommand;
typedef struct boption_s boption;
typedef struct bcmd_context_s bcmd_context;
typedef opt_result (* arg_handle)(const char*, const void*);

struct bcommand_s {
	char *name;
	char *tip;
	int (*handler)(int argc, char **argv);
};
struct boption_s {
	char *long_name;
	char short_name;
	char *tip;

	arg_type type;
	void *arg;
	char *arg_name;
};

#ifdef __CMD_INTERNAL
struct bcmd_context_s {
	char *name;
	char *usage;
	char *description;
	char *epilog;

	boption *options;
	bcommand *commands;

	_Bool print_errors;
	_Bool handle_help;
};
#endif

int bcommand_len(const bcommand *commands);
int boption_len(const boption *options);

bcommand *bcommand_cat(const bcommand *first, const bcommand *second);
boption *boption_cat(const boption *first, const boption *second);

_Bool bcommand_null(bcommand command);
_Bool boption_null(boption option);

bcmd_context *bcmd_context_new(char *name);

void bcmd_context_set_name(bcmd_context *context, char *name);
void bcmd_context_set_usage(bcmd_context *context, char *usage);
void bcmd_context_set_description(bcmd_context *context, char *description);
void bcmd_context_set_epilog(bcmd_context *context, char *epilog);
void bcmd_context_set_print_errors(bcmd_context *context, _Bool print_errors);
void bcmd_context_set_handle_help(bcmd_context *context, _Bool handle_help);

void bcmd_context_pushc(bcmd_context *context, const bcommand *commands);
void bcmd_context_pusho(bcmd_context *context, const boption *options);

void bcmd_context_clearc(bcmd_context *context);
void bcmd_context_cleano(bcmd_context *context);

void bcmd_context_delete(bcmd_context **context);

opt_result bcmd_context_parseo(bcmd_context *context, int *argc, char **argv);
cmd_result bcmd_context_getc(bcmd_context *context, int *argc, char **argv,
							 bcommand *found_command);

void bcmd_context_print_help(bcmd_context *context);

#ifdef __CMD_INTERNAL
void move_to_last(int i, int argc, char **argv);
#endif

#endif
