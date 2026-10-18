/* BEHEMOTH CLI */
#include "beast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* read_file(const char* path, size_t* outlen) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(sz+1);
    fread(buf, 1, sz, f);
    buf[sz]=0;
    fclose(f);
    *outlen = (size_t)sz;
    return buf;
}

static void print_usage(void) {
    printf("behemoth - Full Metal Stack VM + compiler\n");
    printf("Usage:\n");
    printf("  behemoth run   <file.bst>     compile & run\n");
    printf("  behemoth dis   <file.bst>     disassemble bytecode\n");
    printf("  behemoth bench <file.bst>     compile & run with stats\n");
    printf("  behemoth test                 run test suite\n");
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 0; }
    if (!strcmp(argv[1], "test")) {
        /* run all tests via shell? just invoke make test externally; here print hint */
        printf("Run: make test\n");
        return 0;
    }
    if (argc < 3) { print_usage(); return 1; }
    const char* mode = argv[1];
    const char* path = argv[2];
    size_t len;
    char* src = read_file(path, &len);
    if (!src) { fprintf(stderr, "cannot read %s\n", path); return 1; }

    VM vm; vm_init(&vm);
    char err[512]; err[0]=0;
    int entry = compile_source(&vm, src, len, err, sizeof(err));
    if (err[0] || entry < 0) {
        fprintf(stderr, "compile error: %s\n", err[0]?err:"unknown");
        vm_free(&vm); free(src);
        return 1;
    }
    if (!strcmp(mode, "dis")) {
        disassemble(&vm, &vm.functions[entry], "__main__");
        for (int i=0;i<vm.nfuncs;i++) {
            if (i != entry) disassemble(&vm, &vm.functions[i], vm.functions[i].name);
        }
    } else {
        int rc = vm_run(&vm, &vm.functions[entry]);
        if (rc) { fprintf(stderr, "runtime error\n"); }
        if (!strcmp(mode, "bench")) {
            fprintf(stderr, "\n--- stats ---\n");
            fprintf(stderr, "vm steps   : %lu\n", (unsigned long)vm.steps);
            fprintf(stderr, "gc runs    : %d\n", vm.gc_runs);
            fprintf(stderr, "obj freed  : %d\n", vm.objects_freed);
            fprintf(stderr, "bytes alloc: %lu\n", (unsigned long)vm.bytes_alloc);
        }
    }
    vm_free(&vm);
    free(src);
    return 0;
}
