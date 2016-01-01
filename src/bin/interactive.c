/* vi:set ts=4 sw=4 expandtab:
 *
 * Copyright 2016, Chris Leishman (http://github.com/cleishm)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../../config.h"
#include "interactive.h"
#include "util.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <histedit.h>
#include <limits.h>
#include <neo4j-client.h>


// FIXME: internal to libedit
unsigned char ed_newline(EditLine *el, int ch);


static int editline_setup(shell_state_t *state,
        EditLine **el, History **el_history);
static int setup_history(shell_state_t *state, History *el_history);
static char *prompt(EditLine *el);
static unsigned char literal_newline(EditLine *el, int ch);
static unsigned char check_line(EditLine *el, int ch);
static int process_input(shell_state_t *state,
        const char *input, size_t length, const char **end,
        int (*evaluate)(shell_state_t *state, const char *directive));


int interact(shell_state_t *state,
        int (*evaluate)(shell_state_t *state, const char *directive))
{
    EditLine *el = NULL;
    History *el_history = NULL;

    if (editline_setup(state, &el, &el_history))
    {
        goto cleanup;
    }

    const char *input;
    int length;
    while ((input = el_gets(el, &length)) != NULL)
    {
        const char *end;
        int r = process_input(state, input, length, &end, evaluate);
        if (r < 0)
        {
            goto cleanup;
        }
        if (r > 0)
        {
            break;
        }

        const char *c;
        for (c = input; c < end && isspace(*c); ++c)
            ;
        if (c != end)
        {
            const char *entry = temp_copy(state, input, end - input);
            if (entry == NULL)
            {
                print_errno(state->err, "unexpected error", errno);
                goto cleanup;
            }
            HistEvent ev;
            if (history(el_history, &ev, H_ENTER, entry) < 0)
            {
                print_errno(state->err, "unexpected error", errno);
                goto cleanup;
            }
            if (state->histfile != NULL &&
                    history(el_history, &ev, H_SAVE, state->histfile) < 0)
            {
                print_errno(state->err, "unexpected error", errno);
                goto cleanup;
            }
        }

        if (end < (input + length - 1))
        {
            char *buffer = temp_copy(state, end, (input + length - 1) - end);
            if (buffer == NULL)
            {
                print_errno(state->err, "unexpected error", errno);
                goto cleanup;
            }
            el_push(el, buffer);
        }
    }

    if (input == NULL)
    {
        fputc('\n', state->out);
    }

cleanup:
    if (el_history != NULL)
    {
        history_end(el_history);
    }
    if (el != NULL)
    {
        el_end(el);
    }
    return -1;
}


int editline_setup(shell_state_t *state, EditLine **el, History **el_history)
{
    assert(el != NULL);
    assert(el_history != NULL);

    *el = el_init(state->prog_name, state->in, state->out, state->err);
    if (*el == NULL)
    {
        print_errno(state->err, "failed to initialize editline", errno);
        return -1;
    }
    el_set(*el, EL_CLIENTDATA, state);
    el_set(*el, EL_PROMPT, &prompt);
    el_set(*el, EL_EDITOR, "emacs");

    *el_history = history_init();
    if (*el_history == NULL)
    {
        print_errno(state->err, "failed to initialize history", errno);
        return -1;
    }
    HistEvent ev;
    history(*el_history, &ev, H_SETSIZE, 500);
    history(*el_history, &ev, H_SETUNIQUE, 1);
    el_set(*el, EL_HIST, history, *el_history);

    if (state->histfile != NULL && setup_history(state, *el_history))
    {
        return -1;
    }

    el_set(*el, EL_ADDFN, "ed-literal-newline",
            "Add a literal newline", literal_newline);
    el_set(*el, EL_ADDFN, "ed-check-line",
            "Check a line for a complete directive "
            "or insert a newline", check_line);
    el_set(*el, EL_BIND, "\r", "ed-literal-newline", NULL);
    el_set(*el, EL_BIND, "\n", "ed-check-line", NULL);
    el_set(*el, EL_BIND, "-a", "\r", "ed-literal-newline", NULL);
    el_set(*el, EL_BIND, "-a", "\n", "ed-check-line", NULL);
    el_set(*el, EL_BIND, "-a", "k", "ed-prev-line", NULL);
    el_set(*el, EL_BIND, "-a", "j", "ed-next-line", NULL);

    el_source(*el, NULL);

    el_set(*el, EL_SIGNAL, 1);
    return 0;
}


int setup_history(shell_state_t *state, History *el_history)
{
    assert(state->histfile != NULL);

    char dir[PATH_MAX];
    if (neo4j_dirname(state->histfile, dir, sizeof(dir)) < 0)
    {
        fprintf(state->err, "invalid history file\n");
        return -1;
    }
    if (neo4j_mkdir_p(dir))
    {
        print_errno(state->err, "failed to create history file", errno);
        return -1;
    }

    HistEvent ev;
    if (history(el_history, &ev, H_LOAD, state->histfile) < 0)
    {
        if (errno != ENOENT)
        {
            print_errno(state->err, "failed to load history", errno);
            return -1;
        }

        if (history(el_history, &ev, H_SAVE, state->histfile) < 0)
        {
            print_errno(state->err, "failed to create history file", errno);
            return -1;
        }
    }
    return 0;
}


char *prompt(EditLine *el)
{
    return "neo4j> ";
}


unsigned char literal_newline(EditLine *el, int ch)
{
    char s[2] = { '\n', 0 };
    if (el_insertstr(el, s))
    {
        return CC_ERROR;
    }
    return CC_REFRESH;
}


unsigned char check_line(EditLine *el, int ch)
{
    shell_state_t *state;
    if (el_get(el, EL_CLIENTDATA, &state))
    {
        return CC_FATAL;
    }
    const LineInfo *li = el_line(el);

    size_t length = li->lastchar - li->buffer;
    char *line = temp_copy(state, li->buffer, length);
    if (line == NULL)
    {
        return CC_FATAL;
    }
    line[length] = '\n';

    bool complete;
    ssize_t n = neo4j_cli_uparse(line, length + 1,
            NULL, &length, &complete);
    if (n < 0)
    {
        return CC_FATAL;
    }
    if (complete || length == 0)
    {
        ed_newline(el, ch);
        return CC_NEWLINE;
    }

    return literal_newline(el, ch);
}


int process_input(shell_state_t *state,
        const char *input, size_t length, const char **end,
        int (*evaluate)(shell_state_t *state, const char *directive))
{
    const char *input_end = input + length;
    while (input < input_end)
    {
        const char *start;
        bool complete;
        ssize_t n = neo4j_cli_uparse(input, input_end - input,
                &start, &length, &complete);
        if (n < 0)
        {
            print_errno(state->err, "unexpected error", errno);
            return -1;
        }
        if (n == 0 || !complete)
        {
            break;
        }

        const char *directive = temp_copy(state, start, length);
        if (directive == NULL)
        {
            print_errno(state->err, "unexpected error", errno);
            return -1;
        }

        int r = evaluate(state, directive);
        if (r < 0)
        {
            *end = input_end;
            return 0;
        }
        if (r > 0)
        {
            return 1;
        }
        input += n;
        assert(input <= input_end);
    }
    for (; input < input_end && isspace(*input); ++input)
        ;
    *end = input;
    return 0;
}
