/* ============================================================================
 * BEHEMOTH — a from-scratch, full-metal-stack virtual machine + compiler.
 *
 *   Layer 0 (silicon)  : bytecode opcodes, raw integer arithmetic
 *   Layer 1 (systems)  : mark-sweep garbage collector over heap objects
 *   Layer 2 (compiler) : lexer -> parser -> AST -> type checker -> optimizer
 *                        -> bytecode codegen -> stack VM
 *
 * Single binary, zero dependencies beyond libc. Built to be falsifiable:
 *   make && make test && make bench
 *
 * (c) Hesam Jamali — hesam-oxe — Full Metal Stack
 * ========================================================================== */
#ifndef BEAST_H
#define BEAST_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* ----------------------------- Opcodes ------------------------------------ */
typedef enum {
    OP_HALT = 0,
    OP_NOP,
    OP_PUSH_CONST,      /* u16 const index */
    OP_POP,
    OP_DUP,
    OP_LOAD,            /* u16 slot */
    OP_STORE,           /* u16 slot */
    OP_LOAD_GLOBAL,     /* u16 global index */
    OP_STORE_GLOBAL,    /* u16 global index */
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_NEG,
    OP_CONCAT,
    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE,
    OP_NOT,
    OP_JMP,             /* i32 offset (relative to next instruction) */
    OP_JMP_IF_FALSE,    /* i32 offset */
    OP_CALL,            /* u16 func index, u8 argc */
    OP_RET,
    OP_PRINT,
    OP_MAKE_CLOSURE,    /* (reserved) */
    OP_COUNT
} OpCode;

static inline const char* opcode_name(OpCode o) {
    static const char* names[] = {
        "HALT","NOP","PUSH_CONST","POP","DUP","LOAD","STORE",
        "LOAD_GLOBAL","STORE_GLOBAL","ADD","SUB","MUL","DIV","MOD","NEG",
        "CONCAT","EQ","NE","LT","LE","GT","GE","NOT","JMP","JMP_IF_FALSE",
        "CALL","RET","PRINT","MAKE_CLOSURE"
    };
    if (o >= OP_COUNT) return "?";
    return names[o];
}

/* ----------------------------- Values ------------------------------------- */
typedef enum { VAL_NIL, VAL_INT, VAL_BOOL, VAL_STR } ValType;

typedef struct Obj Obj;
struct Obj {
    int          marked;
    struct Obj*  next;        /* global allocation list */
    int          kind;        /* 0 = string */
    union {
        struct { char* chars; size_t len; } str;
    } as;
};

typedef struct Value {
    ValType type;
    union {
        int64_t i;
        int     b;
        Obj*    o;
    } as;
} Value;

static inline Value val_nil(void){ Value v; v.type=VAL_NIL; v.as.i=0; return v; }
static inline Value val_int(int64_t x){ Value v; v.type=VAL_INT; v.as.i=x; return v; }
static inline Value val_bool(int x){ Value v; v.type=VAL_BOOL; v.as.b=x?1:0; return v; }
static inline Value val_str(Obj* o){ Value v; v.type=VAL_STR; v.as.o=o; return v; }

/* ----------------------------- Chunk (bytecode) ---------------------------- */
typedef struct {
    uint8_t* code;
    size_t   count;
    size_t   cap;
    Value*   consts;
    size_t   nconsts;
    size_t   ccap;
    int*     lines;          /* source line per byte (for errors) */
} Chunk;

void chunk_init(Chunk* c);
void chunk_free(Chunk* c);
int  chunk_emit(Chunk* c, uint8_t byte, int line);
int  chunk_add_const(Chunk* c, Value v);
void chunk_patch_i32(Chunk* c, size_t pos, int32_t v);

/* ----------------------------- Function ------------------------------------ */
typedef struct Function {
    char*  name;
    int    arity;
    int    nlocals;
    int    nglobals;       /* (unused, globals are VM-wide) */
    Chunk  chunk;
    int    is_native;
} Function;

/* ----------------------------- AST ---------------------------------------- */
typedef enum {
    /* expressions */
    E_INT, E_BOOL, E_STR, E_VAR, E_BIN, E_UN, E_CALL,
    /* statements */
    S_LET, S_ASSIGN, S_IF, S_WHILE, S_RETURN, S_PRINT, S_BLOCK, S_EXPR,
    /* top level */
    N_FUNC, N_PROGRAM
} NodeKind;

typedef enum { T_INT, T_BOOL, T_STR, T_VOID, T_UNKNOWN, T_ERROR } Type;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    int      line;
    Type     type;
    /* expression payloads */
    int64_t  intv;
    int      boolv;
    char*    strv;
    char*    name;
    int      op;            /* for BIN/UN: token code */
    Node*    left;
    Node*    right;
    Node**   args;
    int      nargs;
    /* statement payloads */
    Node*    cond;
    Node*    thenb;
    Node*    elseb;
    Node*    body;
    Node**   stmts;
    int      nstmts;
    /* function */
    char**   params;
    int      nparams;
    Node*    ret;           /* return expr (optional) */
    Node**   funcs;
    int      nfuncs;
    Node*    mainstmts;     /* block */
};

Node* node_new(NodeKind k, int line);
void  node_free(Node* n);

/* ----------------------------- VM ----------------------------------------- */
#define BEAST_STACK_MAX  4096
#define BEAST_FRAMES_MAX 256
#define BEAST_GLOBALS_MAX 1024

typedef struct CallFrame {
    Function* fn;
    size_t    ip;
    int       base_slot;   /* first local slot for this frame */
} CallFrame;

typedef struct VM {
    /* GC */
    Obj*      objects;     /* head of allocation list */
    size_t    bytes_alloc;
    size_t    next_gc;
    /* execution */
    Value     stack[BEAST_STACK_MAX];
    int       sp;
    CallFrame frames[BEAST_FRAMES_MAX];
    int       frame_count;
    Function* functions;   /* function table */
    int       nfuncs;
    int       fcap;
    Value     globals[BEAST_GLOBALS_MAX];
    int       nglobals;
    /* stats */
    uint64_t  steps;
    int       gc_runs;
    int       objects_freed;
} VM;

void vm_init(VM* v);
void vm_free(VM* v);
int  vm_run(VM* v, Function* entry);   /* 0=ok, nonzero=error */
void vm_push(VM* v, Value val);
Value vm_pop(VM* v);

/* GC */
Obj*  gc_alloc_str(VM* v, const char* s, size_t len);
void  gc_collect(VM* v, VM* root_vm /*same*/);

/* ----------------------------- Compiler API -------------------------------- */

/* High-level: compile source -> VM populated with functions, returns entry index or -1 */
int compile_source(VM* v, const char* src, size_t len, char* errbuf, size_t errbufsz);

/* ----------------------------- Disassembler -------------------------------- */
void disassemble(VM* v, Function* f, const char* name);

/* ----------------------------- Util --------------------------------------- */
void beast_error(const char* fmt, ...);

#endif /* BEAST_H */
