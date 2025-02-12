#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define EXAMPLES_FOLDER "examples/"
#define cc_with_cflags(cmd) cmd_append(cmd, "cc", "-Wall", "-Wextra", "-ggdb", "-I.")
#define cc_output(cmd, output_path) cmd_append(cmd, "-o", output_path);

struct {
    const char *input_path;
    const char *output_path;
} examples[] = {
    { .input_path = EXAMPLES_FOLDER"fib.c",      .output_path = BUILD_FOLDER"fib"      },
    { .input_path = EXAMPLES_FOLDER"square.c",   .output_path = BUILD_FOLDER"square"   },
    { .input_path = EXAMPLES_FOLDER"primes.c",   .output_path = BUILD_FOLDER"primes"   },
    { .input_path = EXAMPLES_FOLDER"coprimes.c", .output_path = BUILD_FOLDER"coprimes" },
    { .input_path = EXAMPLES_FOLDER"lexer.c",    .output_path = BUILD_FOLDER"lexer"    },
};

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Cmd cmd = {0};
    Procs procs = {0};
    long nprocs = 1;

    const char *program_name = shift(argv, argc);
    String_View jprefix = sv_from_cstr("-j");
    while (argc > 0) {
        const char *flag = shift(argv, argc);
        if (sv_starts_with(sv_from_cstr(flag), jprefix)) {
            nprocs = atoi(flag + jprefix.count);
        } else {
            nob_log(ERROR, "Unknown flag `%s`", flag);
            return 1;
        }
    }

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    cc_with_cflags(&cmd);
    cc_output(&cmd, BUILD_FOLDER"generator.o");
    cmd_append(&cmd, "-c", "generator.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    for (size_t i = 0; i < ARRAY_LEN(examples); ) {
        for (size_t j = 0; i < ARRAY_LEN(examples) && j < nprocs; ++j, ++i) {
            cc_with_cflags(&cmd);
            cc_output(&cmd, examples[i].output_path);
            cmd_append(&cmd, examples[i].input_path, BUILD_FOLDER"generator.o");
            da_append(&procs, nob_cmd_run_async_and_reset(&cmd));
        }
        if (!nob_procs_wait_and_reset(&procs)) return 1;
    }

    return 0;
}
