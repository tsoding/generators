#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define EXAMPLES_FOLDER "examples/"
#define cc_with_cflags(cmd) cmd_append(cmd, "cc", "-Wall", "-Wextra", "-ggdb", "-I.")
#define cc_output(cmd, output_path) cmd_append(cmd, "-o", output_path);

long number_of_processors(void)
{
#ifdef __linux__
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    // TODO: implement number_of_processors() for other platforms
    // TODO: contribute number_of_processors() to nob.h
    return 4; // chosen by fair dice roll
#endif
}

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

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    cc_with_cflags(&cmd);
    cc_output(&cmd, BUILD_FOLDER"generator.o");
    cmd_append(&cmd, "-c", "generator.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    long nprocs = number_of_processors();
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
