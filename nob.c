#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "build/"
#define EXAMPLES_FOLDER "examples/"
#define cc_with_cflags(cmd) cmd_append(cmd, "cc", "-Wall", "-Wextra", "-ggdb", "-I.")

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Cmd cmd = {0};
    Procs procs = {0};

    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    cc_with_cflags(&cmd);
    cmd_append(&cmd, "-c", "-o", BUILD_FOLDER"generator.o", "generator.c");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    cc_with_cflags(&cmd);
    cmd_append(&cmd, "-o", BUILD_FOLDER"fib", EXAMPLES_FOLDER"fib.c", BUILD_FOLDER"generator.o");
    da_append(&procs, nob_cmd_run_async_and_reset(&cmd));

    cc_with_cflags(&cmd);
    cmd_append(&cmd, "-o", BUILD_FOLDER"square", EXAMPLES_FOLDER"square.c", BUILD_FOLDER"generator.o");
    da_append(&procs, nob_cmd_run_async_and_reset(&cmd));

    cc_with_cflags(&cmd);
    cmd_append(&cmd, "-o", BUILD_FOLDER"primes", EXAMPLES_FOLDER"primes.c", BUILD_FOLDER"generator.o");
    da_append(&procs, nob_cmd_run_async_and_reset(&cmd));


    if (!nob_procs_wait_and_reset(&procs)) return 1;

    return 0;
}
