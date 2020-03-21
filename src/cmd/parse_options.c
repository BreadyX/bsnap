#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define __CMD_INTERNAL
#include "cmd.h"

static bool BOOL_TRUE = true;
static bool BOOL_FALSE = false;

typedef enum { LONG, SHORT } opt_type;
typedef struct {
	opt_type type;
	bool skip;
	char *opt;
	char *opt_next;

	char invalid_shortopt;
} opt_arg;

static opt_result set_bools(b_option *options);
static opt_result set_to(b_option *opt, void *val);

static opt_result handle_opt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help);

static opt_result handle_longopt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help);
static char *separate_optarg_with_equal(char *opt_full, char **opt, char **arg);
static b_option *find_longopt(char *to_find, b_option *where);

static opt_result handle_shortopt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help);
static b_option *find_shortopt(char to_find, b_option *where);
static opt_result handle_shortopt_with_arg(opt_arg *to_eval, b_option *found);

static void handle_help(b_cmd_context *context);
static void clean_argv(int *argc, char **argv, int n_args);

static void print_error(char *name, opt_arg *argv, opt_result status);
static void perror_sep_optarg(char *err_str, char *name, char *orig_opt);

opt_result parse_options(b_cmd_context *context, int *argc, char **argv)
{
	if (!context)
		return OPT_INVALID;

	b_option *options = context->options;
	int index = 0, n_args = *argc;
	opt_result status = OPT_SUCCESS;
	opt_arg to_eval = {0};

	_Bool help = false;

	status = set_bools(options);
	if (status != OPT_SUCCESS)
		goto error;

	for (index = 1; index < n_args; index++) {
		if (argv[index][0] == '-') { // opt
			if (argv[index][1] == '\0') {
				status = OPT_INVALID;
				goto error;
			}
			if (argv[index][1] == '-') {
				if (argv[index][2] == '\0') { // terminator
					n_args = index + 1;
					break;
				}
				to_eval.type = LONG;
				to_eval.opt = &argv[index][2];
			} else {
				to_eval.type = SHORT;
				to_eval.opt = &argv[index][1];
			}
			to_eval.skip = false;
			to_eval.opt_next = (index == *argc - 1) ? NULL : argv[index + 1];
			status = handle_opt(context, &to_eval, &help);
			if (status != OPT_SUCCESS)
				goto error;
			if (to_eval.skip)
				index++;
		} else { // argv
			move_to_last(index, *argc, argv);
			index--;
			n_args--;
		}
	}
	clean_argv(argc, argv, n_args);

	if (help)
		handle_help(context);

	return status;

error:
	if (context->print_errors)
		print_error(context->name, &to_eval, status);
	return status;
}

opt_result set_bools(b_option *options)
{
	opt_result status = OPT_SUCCESS;
	for (int i = 0; !option_is_null(options[i]); i++) {
		if (options[i].type == ARG_NONE) {
			status = set_to(&options[i], &BOOL_FALSE);
			if (status != OPT_SUCCESS)
				return status;
		}
	}
	return status;
}

opt_result set_to(b_option *opt, void *val)
{
	void *to_set = opt->arg;
	arg_type type = opt->type;
	char *dup, *end_ptr;

	errno = 0;
	if (!to_set || !val) {
		errno = EFAULT;
		return OPT_OTHER_ERROR;
	}
	switch (type) {
		case ARG_NONE:
			*((bool *) to_set) = *((bool *) val);
			return OPT_SUCCESS;
		case ARG_HANDLE:
			return ((arg_handle) to_set)(opt->long_name, val);
		case ARG_INT:
			*((int *) to_set) = (int)strtol(((char *) val), &end_ptr, 10);
			if (errno != 0 || *end_ptr != '\0')
				return ARG_BAD_VALUE;
			return OPT_SUCCESS;
		case ARG_FLOAT:
			*((float *) to_set) = strtof(((char *) val), &end_ptr);
			if (errno != 0 || *end_ptr != '\0')
				return ARG_BAD_VALUE;
			return OPT_SUCCESS;
		case ARG_DOUBLE:
			*((double *) to_set) = strtod(((char *) val), &end_ptr);
			if (errno != 0 || *end_ptr != '\0')
				return ARG_BAD_VALUE;
			return OPT_SUCCESS;
		case ARG_STR:
			dup = strdup(((char *) val));
			if (!dup)
				return OPT_OTHER_ERROR;
			*((char **)to_set) = dup;
			return OPT_SUCCESS;
		default:
			return ARG_BAD_VALUE;
	}
}

opt_result handle_opt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help)
{
	if (to_eval->type == LONG)
		return handle_longopt(context, to_eval, handle_help);
	return handle_shortopt(context, to_eval, handle_help);
}

opt_result handle_longopt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help)
{
	opt_result out = OPT_SUCCESS;
	char *opt, *arg, *sep, back;
	b_option *found;

	sep = separate_optarg_with_equal(to_eval->opt, &opt, &arg);
	back = *sep;
	*sep = '\0';

	found = find_longopt(opt, context->options);
	if (!found) {
		if (strcmp(opt, HELP_LONG) == 0 && context->handle_help) {
			if (arg) {
				out = ARG_SHOULD_NOT_BE;
				goto exit;
			}
			*handle_help = true;
			out = OPT_SUCCESS;
			goto exit;
		} else {
			out = OPT_INVALID;
			goto exit;
		}
	}

	if (found->type != ARG_NONE) {
		if (!arg) {
			if (to_eval->opt_next == NULL || to_eval->opt_next[0] == '-') {
				out = ARG_MISSING;
				goto exit;
			}
			to_eval->skip = true;
			arg = to_eval->opt_next;
		}
		out = set_to(found, arg);
	} else {
		if (arg) {
			out = ARG_SHOULD_NOT_BE;
			goto exit;
		}
		out = set_to(found, &BOOL_TRUE);
	}

exit:
	*sep = back;
	return out;
}

char *separate_optarg_with_equal(char *opt_full, char **opt, char **arg)
{
	char *c;

	*opt = opt_full;
	c = strchr(opt_full, '=');
	if (!c) {
		if (arg)
			*arg = NULL;
		c = &opt_full[strlen(opt_full)];
	} else {
		if (arg)
			*arg = c + 1;
	}
	return c;
}

b_option *find_longopt(char *to_find, b_option *where)
{
	int len = option_len(where);
	for (int i = 0; i < len; i++)
		if (strcmp(to_find, where[i].long_name) == 0)
			return &where[i];
	return NULL;
}

opt_result handle_shortopt(b_cmd_context *context, opt_arg *to_eval, _Bool *handle_help)
{
	opt_result status = OPT_SUCCESS;
	char opt;
	b_option *found;

	for (int i = 0; (opt = to_eval->opt[i]) != '\0'; i++) {
		found = find_shortopt(opt, context->options);
		if (!found) {
			if (opt == HELP_SHORT && context->handle_help) {
				*handle_help = true;
				continue;
			} else {
				to_eval->invalid_shortopt = opt;
				return OPT_INVALID;
			}
		}
		if(found->type != ARG_NONE) {
			if (i == 0) {
				return handle_shortopt_with_arg(to_eval, found);
			} else {
				to_eval->invalid_shortopt = opt;
				return OPT_MISPLACED;
			}
		}

		status = set_to(found, &BOOL_TRUE);
		if (status != OPT_SUCCESS)
			return status;
	}
	return status;
}

b_option *find_shortopt(char to_find, b_option *where)
{

	for (int i = 0; !option_is_null(where[i]); i++)
		if (to_find == where[i].short_name)
			return &where[i];
	return NULL;
}

opt_result handle_shortopt_with_arg(opt_arg *to_eval, b_option *found)
{
	char *arg;
	if (to_eval->opt[1] == '\0') {
		if (to_eval->opt_next == NULL || to_eval->opt_next[0] == '-') {
			to_eval->invalid_shortopt = to_eval->opt[0];
			return ARG_MISSING;
		}
		to_eval->skip = true;
		arg = to_eval->opt_next;
	} else
		arg = &to_eval->opt[1];
	
	return set_to(found, arg);
}

void handle_help(b_cmd_context *context)
{
	print_help(context);
	exit(EXIT_SUCCESS);
}

void clean_argv(int *argc, char **argv, int n_args)
{
	int i, j;

	for (i = 1, j = n_args; j < *argc; i++, j++)
		argv[i] = argv[j];
	for ( ; i < *argc; i++)
		argv[i] = NULL;
	*argc = *argc - n_args + 1;
}

void print_error(char *name, opt_arg *argv, opt_result status)
{
	switch (status) {
		case OPT_INVALID:
			if (argv->type == LONG)
				perror_sep_optarg("%s: Invalid option --%s\n", name, argv->opt);
			else
				fprintf(stderr, "%s: Invalid option -%c\n", name,
						argv->invalid_shortopt);
			break;
		case OPT_MISPLACED:
			fprintf(stderr, "%s: Option -%c is misplaced\n", name,
				    argv->invalid_shortopt);
			break;
		case OPT_OTHER_ERROR:
			fprintf(stderr, "%s: An error occurred while parsing: ", name);
			perror(NULL);
			break;
		case ARG_BAD_VALUE:
			if (argv->type == LONG)
				perror_sep_optarg("%s: Argument to option --%s is of invalid type\n",
								  name, argv->opt);
			else {
				fprintf(stderr, "%s: Argument to option -%c is of invalid type\n",
						name, argv->opt[0]);
			}
			break;
		case ARG_MISSING:
			if (argv->type == LONG)
				perror_sep_optarg("%s: Argument to option --%s is missing\n",
								  name, argv->opt);
			else {
				fprintf(stderr, "%s: Argument to option -%c is missing\n",
						name, argv->invalid_shortopt);
			}
			break;
		case ARG_SHOULD_NOT_BE:
			perror_sep_optarg("%s: Option --%s doesn't allow for arguments\n",
					          name, argv->opt);
			break;

		case OPT_SUCCESS:
			fprintf(stderr, "%s: Everything is ok but this is somehow an error, "
				    "I am confused\n", name);
			break;
	}
}

void perror_sep_optarg(char *err_str, char *name, char *orig_opt)
{
	char *opt, *sep, back;
	sep = separate_optarg_with_equal(orig_opt, &opt, NULL);
	back = *sep;
	fprintf(stderr, err_str, name, opt);
	*sep = back;
}
