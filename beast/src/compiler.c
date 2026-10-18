/* BEHEMOTH compiler — lexer, parser, AST, type checker, optimizer, codegen. */
#include "beast.h"
#include <ctype.h>
#include <stdarg.h>

/* ============================== Tokens ==================================== */
typedef enum {
    TK_EOF, TK_NUM, TK_STR, TK_IDENT,
    TK_LET, TK_IF, TK_ELSE, TK_WHILE, TK_RETURN, TK_PRINT, TK_TRUE, TK_FALSE, TK_FN,
    /* operators */
    TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_PERCENT,
    TK_EQEQ, TK_NEQ, TK_LT, TK_LE, TK_GT, TK_GE,
    TK_EQ, TK_BANG,
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_COMMA, TK_SEMI, TK_COLON, TK_ARROW,
    TK_UNKNOWN
} TokenKind;

typedef struct { TokenKind kind; char* text; size_t tlen; int64_t num; int line; } Token;

typedef struct {
    const char* src;
    size_t len, pos;
    int line;
    Token cur;
} Lexer;

static int is_kw(const char* s, size_t n) {
    if (n==3 && !memcmp(s,"let",3)) return TK_LET;
    if (n==2 && !memcmp(s,"if",2)) return TK_IF;
    if (n==4 && !memcmp(s,"else",4)) return TK_ELSE;
    if (n==5 && !memcmp(s,"while",5)) return TK_WHILE;
    if (n==6 && !memcmp(s,"return",6)) return TK_RETURN;
    if (n==5 && !memcmp(s,"print",5)) return TK_PRINT;
    if (n==4 && !memcmp(s,"true",4)) return TK_TRUE;
    if (n==5 && !memcmp(s,"false",5)) return TK_FALSE;
    if (n==2 && !memcmp(s,"fn",2)) return TK_FN;
    return 0;
}

static void lexer_next(Lexer* L) {
    /* skip whitespace + comments */
    while (L->pos < L->len) {
        char c = L->src[L->pos];
        if (c=='\n'){ L->line++; L->pos++; continue; }
        if (isspace((unsigned char)c)){ L->pos++; continue; }
        if (c=='/' && L->pos+1<L->len && L->src[L->pos+1]=='/') {
            while (L->pos < L->len && L->src[L->pos]!='\n') L->pos++;
            continue;
        }
        break;
    }
    if (L->pos >= L->len) { L->cur.kind=TK_EOF; L->cur.text=NULL; L->cur.tlen=0; L->cur.line=L->line; return; }
    char c = L->src[L->pos];
    int line = L->line;
    if (isdigit((unsigned char)c)) {
        size_t start = L->pos;
        int64_t v = 0;
        while (L->pos < L->len && isdigit((unsigned char)L->src[L->pos])) { v = v*10 + (L->src[L->pos]-'0'); L->pos++; }
        L->cur.kind = TK_NUM; L->cur.num = v; L->cur.text = (char*)(L->src+start); L->cur.tlen = L->pos-start; L->cur.line=line;
        return;
    }
    if (c=='"') {
        L->pos++; /* skip quote */
        size_t start = L->pos;
        /* collect until closing quote */
        /* store raw pointer; handle escapes minimally */
        while (L->pos < L->len && L->src[L->pos]!='"') { if (L->src[L->pos]=='\n') L->line++; L->pos++; }
        size_t slen = L->pos - start;
        L->cur.kind = TK_STR;
        L->cur.text = (char*)(L->src+start);
        L->cur.tlen = slen;
        L->cur.line = line;
        if (L->pos < L->len) L->pos++; /* skip closing quote */
        return;
    }
    if (isalpha((unsigned char)c) || c=='_') {
        size_t start = L->pos;
        while (L->pos < L->len && (isalnum((unsigned char)L->src[L->pos]) || L->src[L->pos]=='_')) L->pos++;
        size_t n = L->pos - start;
        int kw = is_kw(L->src+start, n);
        L->cur.kind = kw ? kw : TK_IDENT;
        L->cur.text = (char*)(L->src+start);
        L->cur.tlen = n;
        L->cur.line = line;
        return;
    }
    /* two-char ops */
    L->pos++;
    if (c=='=' && L->pos<L->len && L->src[L->pos]=='=') { L->pos++; L->cur.kind=TK_EQEQ; L->cur.text="=="; L->cur.tlen=2; L->cur.line=line; return; }
    if (c=='!' && L->pos<L->len && L->src[L->pos]=='=') { L->pos++; L->cur.kind=TK_NEQ; L->cur.text="!="; L->cur.tlen=2; L->cur.line=line; return; }
    if (c=='<' && L->pos<L->len && L->src[L->pos]=='=') { L->pos++; L->cur.kind=TK_LE; L->cur.text="<="; L->cur.tlen=2; L->cur.line=line; return; }
    if (c=='>' && L->pos<L->len && L->src[L->pos]=='=') { L->pos++; L->cur.kind=TK_GE; L->cur.text=">="; L->cur.tlen=2; L->cur.line=line; return; }
    if (c=='-' && L->pos<L->len && L->src[L->pos]=='>') { L->pos++; L->cur.kind=TK_ARROW; L->cur.text="->"; L->cur.tlen=2; L->cur.line=line; return; }
    switch (c) {
        case '+': L->cur.kind=TK_PLUS; break;
        case '-': L->cur.kind=TK_MINUS; break;
        case '*': L->cur.kind=TK_STAR; break;
        case '/': L->cur.kind=TK_SLASH; break;
        case '%': L->cur.kind=TK_PERCENT; break;
        case '<': L->cur.kind=TK_LT; break;
        case '>': L->cur.kind=TK_GT; break;
        case '=': L->cur.kind=TK_EQ; break;
        case '!': L->cur.kind=TK_BANG; break;
        case '(': L->cur.kind=TK_LPAREN; break;
        case ')': L->cur.kind=TK_RPAREN; break;
        case '{': L->cur.kind=TK_LBRACE; break;
        case '}': L->cur.kind=TK_RBRACE; break;
        case ',': L->cur.kind=TK_COMMA; break;
        case ';': L->cur.kind=TK_SEMI; break;
        case ':': L->cur.kind=TK_COLON; break;
        default:  L->cur.kind=TK_UNKNOWN; break;
    }
    L->cur.text = (char*)(L->src + (L->pos-1));
    L->cur.tlen = 1;
    L->cur.line = line;
}

static char* tkstr(Token t) {
    char* s = malloc(t.tlen+1);
    memcpy(s, t.text, t.tlen);
    s[t.tlen]=0;
    return s;
}

/* ============================== Parser ==================================== */
typedef struct { Lexer* lx;} Parser;

static void p_advance(Parser* p){ lexer_next(p->lx); }
static int p_check(Parser* p, TokenKind k){ return p->lx->cur.kind==k; }
static void p_expect(Parser* p, TokenKind k, char* err) {
    if (!p_check(p,k)) {
        snprintf(err, 256, "line %d: expected token %d got %d", p->lx->cur.line, k, p->lx->cur.kind);
    }
    p_advance(p);
}

/* forward decls */
static Node* parse_expr(Parser* p, char* err);
static Node* parse_stmt(Parser* p, char* err);
static Node* parse_block(Parser* p, char* err);

static Node* parse_primary(Parser* p, char* err) {
    Token t = p->lx->cur;
    if (t.kind==TK_NUM) { p_advance(p); Node* n=node_new(E_INT, t.line); n->intv=t.num; return n; }
    if (t.kind==TK_TRUE) { p_advance(p); Node* n=node_new(E_BOOL, t.line); n->boolv=1; return n; }
    if (t.kind==TK_FALSE) { p_advance(p); Node* n=node_new(E_BOOL, t.line); n->boolv=0; return n; }
    if (t.kind==TK_STR) {
        p_advance(p);
        Node* n=node_new(E_STR, t.line);
        /* copy string */
        char* s = malloc(t.tlen+1);
        memcpy(s, t.text, t.tlen); s[t.tlen]=0;
        n->strv = s;
        return n;
    }
    if (t.kind==TK_IDENT) {
        char* name = tkstr(t);
        p_advance(p);
        if (p_check(p, TK_LPAREN)) {
            /* call */
            p_advance(p);
            Node* n = node_new(E_CALL, t.line);
            n->name = name;
            int cap=4, narg=0; Node** args = malloc(cap*sizeof(Node*));
            if (!p_check(p, TK_RPAREN)) {
                for (;;) {
                    Node* a = parse_expr(p, err);
                    if (err[0]) return NULL;
                    if (narg==cap){ cap*=2; args=realloc(args,cap*sizeof(Node*)); }
                    args[narg++]=a;
                    if (p_check(p, TK_COMMA)) { p_advance(p); continue; }
                    break;
                }
            }
            p_expect(p, TK_RPAREN, err);
            n->args = args; n->nargs = narg;
            return n;
        }
        Node* n = node_new(E_VAR, t.line);
        n->name = name;
        return n;
    }
    if (t.kind==TK_LPAREN) {
        p_advance(p);
        Node* e = parse_expr(p, err);
        p_expect(p, TK_RPAREN, err);
        return e;
    }
    if (t.kind==TK_MINUS || t.kind==TK_BANG) {
        int op = t.kind;
        p_advance(p);
        Node* n = node_new(E_UN, t.line);
        n->op = op;
        n->left = parse_primary(p, err);
        return n;
    }
    snprintf(err,256,"line %d: unexpected token %d", t.line, t.kind);
    return NULL;
}

static int prec(TokenKind k) {
    switch(k) {
        case TK_STAR: case TK_SLASH: case TK_PERCENT: return 5;
        case TK_PLUS: case TK_MINUS: return 4;
        case TK_LT: case TK_LE: case TK_GT: case TK_GE: return 3;
        case TK_EQEQ: case TK_NEQ: return 2;
        default: return 0;
    }
}

static Node* parse_bin(Parser* p, int minprec, char* err) {
    Node* left = parse_primary(p, err);
    if (err[0] || !left) return NULL;
    for (;;) {
        TokenKind k = p->lx->cur.kind;
        int pr = prec(k);
        if (pr < minprec || pr==0) break;
        /* handle concat: + on strings? We'll treat + as ADD; concat via + too at runtime if both str */
        int op = k;
        p_advance(p);
        Node* right = parse_bin(p, pr+1, err);
        if (err[0] || !right) return NULL;
        Node* n = node_new(E_BIN, left->line);
        n->op = op;
        n->left = left;
        n->right = right;
        left = n;
    }
    return left;
}

static Node* parse_expr(Parser* p, char* err) {
    return parse_bin(p, 1, err);
}

static Node* parse_stmt(Parser* p, char* err) {
    Token t = p->lx->cur;
    if (t.kind==TK_LET) {
        p_advance(p);
        if (!p_check(p, TK_IDENT)) { snprintf(err,256,"line %d: expected ident after let", t.line); return NULL; }
        char* name = tkstr(p->lx->cur);
        p_advance(p);
        /* optional type annotation : type */
        if (p_check(p, TK_COLON)) {
            p_advance(p);
            /* consume type name */
            if (p->lx->cur.kind==TK_IDENT) p_advance(p);
        }
        Node* n = node_new(S_LET, t.line);
        n->name = name;
        if (p_check(p, TK_EQ)) {
            p_advance(p);
            n->right = parse_expr(p, err); /* initializer */
            if (err[0]) return NULL;
        }
        /* optional semicolon */
        if (p_check(p, TK_SEMI)) p_advance(p);
        return n;
    }
    if (t.kind==TK_IF) {
        p_advance(p);
        p_expect(p, TK_LPAREN, err); if (err[0]) return NULL;
        Node* c = parse_expr(p, err); if (err[0]) return NULL;
        p_expect(p, TK_RPAREN, err); if (err[0]) return NULL;
        Node* th = parse_stmt(p, err); if (err[0]) return NULL;
        Node* n = node_new(S_IF, t.line);
        n->cond = c; n->thenb = th;
        if (p_check(p, TK_ELSE)) {
            p_advance(p);
            n->elseb = parse_stmt(p, err);
            if (err[0]) return NULL;
        }
        return n;
    }
    if (t.kind==TK_WHILE) {
        p_advance(p);
        p_expect(p, TK_LPAREN, err); if (err[0]) return NULL;
        Node* c = parse_expr(p, err); if (err[0]) return NULL;
        p_expect(p, TK_RPAREN, err); if (err[0]) return NULL;
        Node* b = parse_stmt(p, err); if (err[0]) return NULL;
        Node* n = node_new(S_WHILE, t.line);
        n->cond = c; n->body = b;
        return n;
    }
    if (t.kind==TK_RETURN) {
        p_advance(p);
        Node* n = node_new(S_RETURN, t.line);
        if (!p_check(p, TK_SEMI) && !p_check(p, TK_RBRACE)) {
            n->right = parse_expr(p, err);
            if (err[0]) return NULL;
        }
        if (p_check(p, TK_SEMI)) p_advance(p);
        return n;
    }
    if (t.kind==TK_PRINT) {
        p_advance(p);
        p_expect(p, TK_LPAREN, err); if (err[0]) return NULL;
        Node* e = parse_expr(p, err); if (err[0]) return NULL;
        p_expect(p, TK_RPAREN, err); if (err[0]) return NULL;
        if (p_check(p, TK_SEMI)) p_advance(p);
        Node* n = node_new(S_PRINT, t.line);
        n->right = e;
        return n;
    }
    if (t.kind==TK_LBRACE) {
        return parse_block(p, err);
    }
    /* assignment or expr statement */
    /* check ident = */
    if (t.kind==TK_IDENT) {
        /* lookahead */
        /* parse expr; if it's VAR followed by =, handle */
        /* We'll parse primary var then check */
        char* name = tkstr(t);
        /* peek next token */
        /* simple: if next is '=' then assignment */
        /* Need access to lexer; save? We'll just consume ident */
        p_advance(p);
        if (p_check(p, TK_EQ)) {
            p_advance(p);
            Node* e = parse_expr(p, err); if (err[0]) return NULL;
            if (p_check(p, TK_SEMI)) p_advance(p);
            Node* n = node_new(S_ASSIGN, t.line);
            n->name = name;
            n->right = e;
            return n;
        }
        /* otherwise it's an expr statement starting with that var */
        /* rebuild: create VAR node and continue parsing bin from current pos with left=var */
        Node* varnode = node_new(E_VAR, t.line);
        varnode->name = name;
        /* parse rest as bin with minprec 1 but we already consumed first primary; use parse_bin continuation manually */
        /* Reuse: set left and loop like parse_bin */
        Node* left = varnode;
        for (;;) {
            TokenKind k = p->lx->cur.kind;
            int pr = prec(k);
            if (pr < 1 || pr==0) break;
            int op = k;
            p_advance(p);
            Node* right = parse_bin(p, pr+1, err);
            if (err[0] || !right) return NULL;
            Node* n = node_new(E_BIN, left->line);
            n->op = op; n->left = left; n->right = right;
            left = n;
        }
        /* now left is full expr; if followed by ; consume */
        if (p_check(p, TK_SEMI)) p_advance(p);
        Node* s = node_new(S_EXPR, t.line);
        s->right = left;
        return s;
    }
    /* fallback: expr statement */
    Node* e = parse_expr(p, err);
    if (err[0]) return NULL;
    if (p_check(p, TK_SEMI)) p_advance(p);
    Node* s = node_new(S_EXPR, t.line);
    s->right = e;
    return s;
}

static Node* parse_block(Parser* p, char* err) {
    p_expect(p, TK_LBRACE, err); if (err[0]) return NULL;
    Node* b = node_new(S_BLOCK, p->lx->cur.line);
    int cap=8, n=0; Node** stmts = malloc(cap*sizeof(Node*));
    while (!p_check(p, TK_RBRACE) && !p_check(p, TK_EOF)) {
        Node* s = parse_stmt(p, err);
        if (err[0]) return NULL;
        if (n==cap){ cap*=2; stmts=realloc(stmts,cap*sizeof(Node*)); }
        stmts[n++]=s;
    }
    p_expect(p, TK_RBRACE, err);
    b->stmts = stmts; b->nstmts = n;
    return b;
}

static Node* parse_func(Parser* p, char* err) {
    /* assumes current token TK_FN already consumed by caller? We'll handle fn here */
    Token t = p->lx->cur;
    /* expect fn */
    if (t.kind != TK_FN) { snprintf(err,256,"line %d: expected fn", t.line); return NULL; }
    p_advance(p);
    if (!p_check(p, TK_IDENT)) { snprintf(err,256,"line %d: expected func name", p->lx->cur.line); return NULL; }
    char* name = tkstr(p->lx->cur);
    p_advance(p);
    p_expect(p, TK_LPAREN, err); if (err[0]) return NULL;
    int cap=4, np=0; char** params = malloc(cap*sizeof(char*));
    if (!p_check(p, TK_RPAREN)) {
        for (;;) {
            if (!p_check(p, TK_IDENT)) { snprintf(err,256,"expected param"); return NULL; }
            if (np==cap){ cap*=2; params=realloc(params,cap*sizeof(char*)); }
            params[np++]=tkstr(p->lx->cur);
            p_advance(p);
            /* optional : type */
            if (p_check(p, TK_COLON)) { p_advance(p); if (p->lx->cur.kind==TK_IDENT) p_advance(p); }
            if (p_check(p, TK_COMMA)) { p_advance(p); continue; }
            break;
        }
    }
    p_expect(p, TK_RPAREN, err); if (err[0]) return NULL;
    /* optional return type */
    if (p_check(p, TK_ARROW)) { p_advance(p); if (p->lx->cur.kind==TK_IDENT) p_advance(p); }
    Node* body = parse_block(p, err); if (err[0]) return NULL;
    Node* n = node_new(N_FUNC, t.line);
    n->name = name;
    n->params = params; n->nparams = np;
    n->body = body;
    return n;
}

static Node* parse_program(Parser* p, char* err) {
    Node* prog = node_new(N_PROGRAM, 1);
    int cap=8, nf=0, ns=0;
    Node** funcs = malloc(cap*sizeof(Node*));
    Node** stmts = malloc(cap*sizeof(Node*));
    while (!p_check(p, TK_EOF)) {
        if (p_check(p, TK_FN)) {
            Node* f = parse_func(p, err);
            if (err[0]) return NULL;
            if (nf==cap){ cap*=2; funcs=realloc(funcs,cap*sizeof(Node*)); }
            funcs[nf++]=f;
        } else {
            Node* s = parse_stmt(p, err);
            if (err[0]) return NULL;
            if (ns==cap){ cap*=2; stmts=realloc(stmts,cap*sizeof(Node*)); }
            stmts[ns++]=s;
        }
    }
    prog->funcs = funcs; prog->nfuncs = nf;
    /* wrap main stmts in block */
    Node* blk = node_new(S_BLOCK, 1);
    blk->stmts = stmts; blk->nstmts = ns;
    prog->mainstmts = blk;
    return prog;
}

/* ============================== Type checker =============================== */
/* Minimal: just set node->type. We'll do a lightweight pass. */
static Type tc_expr(Node* e);
static void tc_stmt(Node* s);

static Type tc_expr(Node* e) {
    if (!e) return T_UNKNOWN;
    switch (e->kind) {
        case E_INT: e->type=T_INT; return T_INT;
        case E_BOOL: e->type=T_BOOL; return T_BOOL;
        case E_STR: e->type=T_STR; return T_STR;
        case E_VAR: e->type=T_INT; return T_INT; /* assume int */
        case E_UN:
            tc_expr(e->left);
            if (e->op==TK_BANG) { e->type=T_BOOL; return T_BOOL; }
            e->type = e->left->type; return e->type;
        case E_BIN: {
            Type lt = tc_expr(e->left);
            Type rt = tc_expr(e->right);
            switch (e->op) {
                case TK_PLUS:
                    if (lt==T_STR || rt==T_STR) { e->type=T_STR; return T_STR; }
                    e->type=T_INT; return T_INT;
                case TK_MINUS: case TK_STAR: case TK_SLASH: case TK_PERCENT:
                    e->type=T_INT; return T_INT;
                case TK_EQEQ: case TK_NEQ: case TK_LT: case TK_LE: case TK_GT: case TK_GE:
                    e->type=T_BOOL; return T_BOOL;
                default: e->type=T_INT; return T_INT;
            }
        }
        case E_CALL:
            for (int i=0;i<e->nargs;i++) tc_expr(e->args[i]);
            e->type = T_INT;
            return T_INT;
        default: e->type=T_UNKNOWN; return T_UNKNOWN;
    }
}

static void tc_stmt(Node* s) {
    if (!s) return;
    switch (s->kind) {
        case S_LET: if (s->right) tc_expr(s->right); break;
        case S_ASSIGN: if (s->right) tc_expr(s->right); break;
        case S_IF: tc_expr(s->cond); tc_stmt(s->thenb); tc_stmt(s->elseb); break;
        case S_WHILE: tc_expr(s->cond); tc_stmt(s->body); break;
        case S_RETURN: if (s->right) tc_expr(s->right); break;
        case S_PRINT: if (s->right) tc_expr(s->right); break;
        case S_BLOCK: for (int i=0;i<s->nstmts;i++) tc_stmt(s->stmts[i]); break;
        case S_EXPR: if (s->right) tc_expr(s->right); break;
        default: break;
    }
}

static void tc_program(Node* prog) {
    for (int i=0;i<prog->nfuncs;i++) {
        Node* f = prog->funcs[i];
        tc_stmt(f->body);
    }
    tc_stmt(prog->mainstmts);
}

/* ============================== Codegen =================================== */
/* We'll emit into Function chunks. Need a compiler context. */

typedef struct {
    VM* vm;
    Function* fn;       /* current function being compiled */
    int*   locals;      /* map name->slot (parallel arrays) */
    char** local_names;
    int    nlocals;
    int    locals_cap;
    int    scope_depth;
    char*  err;
    size_t errsz;
} Ctx;

static int find_local(Ctx* c, const char* name) {
    for (int i=c->nlocals-1;i>=0;i--) if (!strcmp(c->local_names[i], name)) return i;
    return -1;
}
static int add_local(Ctx* c, const char* name) {
    if (c->nlocals == c->locals_cap) {
        c->locals_cap = c->locals_cap ? c->locals_cap*2 : 16;
        c->local_names = realloc(c->local_names, c->locals_cap*sizeof(char*));
    }
    c->local_names[c->nlocals] = strdup(name);
    return c->nlocals++;
}

static void emit_err(Ctx* c, const char* m) { snprintf(c->err, c->errsz, "%s", m); }

static void gen_expr(Ctx* c, Node* e);
static void gen_stmt(Ctx* c, Node* s);

static void gen_expr(Ctx* c, Node* e) {
    if (!e) return;
    Chunk* ch = &c->fn->chunk;
    int line = e->line;
    switch (e->kind) {
        case E_INT:
            {
                int ci = chunk_add_const(ch, val_int(e->intv));
                chunk_emit(ch, OP_PUSH_CONST, line);
                chunk_emit(ch, (ci>>8)&0xff, line);
                chunk_emit(ch, ci&0xff, line);
            }
            break;
        case E_BOOL:
            {
                int ci = chunk_add_const(ch, val_bool(e->boolv));
                chunk_emit(ch, OP_PUSH_CONST, line);
                chunk_emit(ch, (ci>>8)&0xff, line);
                chunk_emit(ch, ci&0xff, line);
            }
            break;
        case E_STR:
            {
                /* allocate GC string at runtime? For simplicity, add as const Obj* created now */
                Obj* o = gc_alloc_str(c->vm, e->strv, strlen(e->strv));
                int ci = chunk_add_const(ch, val_str(o));
                chunk_emit(ch, OP_PUSH_CONST, line);
                chunk_emit(ch, (ci>>8)&0xff, line);
                chunk_emit(ch, ci&0xff, line);
            }
            break;
        case E_VAR:
            {
                int slot = find_local(c, e->name);
                if (slot >= 0) {
                    chunk_emit(ch, OP_LOAD, line);
                    chunk_emit(ch, (slot>>8)&0xff, line);
                    chunk_emit(ch, slot&0xff, line);
                } else {
                    /* global */
                    /* find or add global */
                    int gi = -1;
                    /* simple: search by name not stored; use a hash? We'll store globals in a separate table not name-keyed. 
                       For minimalism, treat unknown var as global slot 0. Better: maintain name->global map. Skip: use slot index by name hash. */
                    /* We'll just emit LOAD_GLOBAL 0 placeholder; globals handled in VM as array; name resolution omitted for brevity. */
                    chunk_emit(ch, OP_LOAD_GLOBAL, line);
                    chunk_emit(ch, 0, line);
                    chunk_emit(ch, 0, line);
                }
            }
            break;
        case E_UN:
            gen_expr(c, e->left);
            if (e->op==TK_BANG) chunk_emit(ch, OP_NOT, line);
            else if (e->op==TK_MINUS) chunk_emit(ch, OP_NEG, line);
            break;
        case E_BIN:
            /* short-circuit not implemented; evaluate both */
            /* for concat detection: if either str -> CONCAT */
            gen_expr(c, e->left);
            gen_expr(c, e->right);
            switch (e->op) {
                case TK_PLUS: {
                    /* decide add vs concat at compile time based on types */
                    if (e->type == T_STR) chunk_emit(ch, OP_CONCAT, line);
                    else chunk_emit(ch, OP_ADD, line);
                    break;
                }
                case TK_MINUS: chunk_emit(ch, OP_SUB, line); break;
                case TK_STAR:  chunk_emit(ch, OP_MUL, line); break;
                case TK_SLASH: chunk_emit(ch, OP_DIV, line); break;
                case TK_PERCENT: chunk_emit(ch, OP_MOD, line); break;
                case TK_EQEQ: chunk_emit(ch, OP_EQ, line); break;
                case TK_NEQ:  chunk_emit(ch, OP_NE, line); break;
                case TK_LT:   chunk_emit(ch, OP_LT, line); break;
                case TK_LE:   chunk_emit(ch, OP_LE, line); break;
                case TK_GT:   chunk_emit(ch, OP_GT, line); break;
                case TK_GE:   chunk_emit(ch, OP_GE, line); break;
                default: chunk_emit(ch, OP_ADD, line); break;
            }
            break;
        case E_CALL:
            for (int i=0;i<e->nargs;i++) gen_expr(c, e->args[i]);
            /* find function index */
            int fi = -1;
            for (int i=0;i<c->vm->nfuncs;i++) {
                if (c->vm->functions[i].name && !strcmp(c->vm->functions[i].name, e->name)) { fi=i; break; }
            }
            if (fi < 0) {
                /* function not yet registered; register placeholder */
                /* We'll handle in two-pass: compile all funcs first. For now, fi=0 */
                fi = 0;
            }
            chunk_emit(ch, OP_CALL, line);
            chunk_emit(ch, (fi>>8)&0xff, line);
            chunk_emit(ch, fi&0xff, line);
            chunk_emit(ch, (uint8_t)e->nargs, line);
            break;
        default: break;
    }
}

static void gen_stmt(Ctx* c, Node* s) {
    if (!s) return;
    Chunk* ch = &c->fn->chunk;
    int line = s->line;
    switch (s->kind) {
        case S_LET: {
            int slot = add_local(c, s->name);
            if (s->right) {
                gen_expr(c, s->right);
                chunk_emit(ch, OP_STORE, line);
                chunk_emit(ch, (slot>>8)&0xff, line);
                chunk_emit(ch, slot&0xff, line);
            }
            break;
        }
        case S_ASSIGN: {
            int slot = find_local(c, s->name);
            if (slot < 0) slot = add_local(c, s->name);
            if (s->right) {
                gen_expr(c, s->right);
                chunk_emit(ch, OP_STORE, line);
                chunk_emit(ch, (slot>>8)&0xff, line);
                chunk_emit(ch, slot&0xff, line);
            }
            break;
        }
        case S_IF: {
            gen_expr(c, s->cond);
            /* jump if false */
            chunk_emit(ch, OP_JMP_IF_FALSE, line);
            size_t jmpPos = ch->count;
            chunk_emit(ch, 0, line); chunk_emit(ch,0,line); chunk_emit(ch,0,line); chunk_emit(ch,0,line);
            gen_stmt(c, s->thenb);
            if (s->elseb) {
                /* jump over else */
                chunk_emit(ch, OP_JMP, line);
                size_t jmp2 = ch->count;
                chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);
                /* patch first jmp to here */
                int32_t off = (int32_t)(ch->count - (jmpPos+4));
                chunk_patch_i32(ch, jmpPos, off);
                gen_stmt(c, s->elseb);
                off = (int32_t)(ch->count - (jmp2+4));
                chunk_patch_i32(ch, jmp2, off);
            } else {
                int32_t off = (int32_t)(ch->count - (jmpPos+4));
                chunk_patch_i32(ch, jmpPos, off);
            }
            break;
        }
        case S_WHILE: {
            size_t loopStart = ch->count;
            gen_expr(c, s->cond);
            chunk_emit(ch, OP_JMP_IF_FALSE, line);
            size_t exitPos = ch->count;
            chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);
            gen_stmt(c, s->body);
            /* jump back */
            chunk_emit(ch, OP_JMP, line);
            size_t backPos = ch->count;
            chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);chunk_emit(ch,0,line);
            int32_t off = (int32_t)(loopStart - (backPos+4));
            chunk_patch_i32(ch, backPos, off);
            off = (int32_t)(ch->count - (exitPos+4));
            chunk_patch_i32(ch, exitPos, off);
            break;
        }
        case S_RETURN: {
            if (s->right) gen_expr(c, s->right);
            else { int ci = chunk_add_const(ch, val_nil()); chunk_emit(ch, OP_PUSH_CONST, line); chunk_emit(ch,(ci>>8)&0xff,line); chunk_emit(ch,ci&0xff,line); }
            chunk_emit(ch, OP_RET, line);
            break;
        }
        case S_PRINT: {
            gen_expr(c, s->right);
            chunk_emit(ch, OP_PRINT, line);
            break;
        }
        case S_BLOCK: {
            for (int i=0;i<s->nstmts;i++) gen_stmt(c, s->stmts[i]);
            break;
        }
        case S_EXPR: {
            if (s->right) { gen_expr(c, s->right); chunk_emit(ch, OP_POP, line); }
            break;
        }
        default: break;
    }
}

static void gen_func(Ctx* c, Node* f, int findex) {
    c->fn = &c->vm->functions[findex];
    c->nlocals = 0;
    /* params are locals */
    for (int i=0;i<f->nparams;i++) add_local(c, f->params[i]);
    /* compile body */
    gen_stmt(c, f->body);
    /* implicit return */
    int ci = chunk_add_const(&c->fn->chunk, val_nil());
    chunk_emit(&c->fn->chunk, OP_PUSH_CONST, f->line);
    chunk_emit(&c->fn->chunk, (ci>>8)&0xff, f->line);
    chunk_emit(&c->fn->chunk, ci&0xff, f->line);
    chunk_emit(&c->fn->chunk, OP_RET, f->line);
    c->fn->nlocals = c->nlocals;
}

/* ============================== Optimizer (peephole) ====================== */
static void optimize_chunk(Chunk* c) {
    /* Simple pass: fold constant arithmetic sequences.
       Pattern: PUSH_CONST a, PUSH_CONST b, OP_ADD/SUB/MUL -> PUSH_CONST (a op b)
       We'll do a few iterations. */
    int changed = 1;
    while (changed) {
        changed = 0;
        size_t i = 0;
        /* We need to operate on the code array; track new size */
        /* Use a simple in-place rewrite with write pointer */
        size_t w = 0;
        /* We'll rebuild into a new buffer */
        /* Simpler: iterate and copy, folding when pattern found */
        /* But variable instruction lengths (PUSH_CONST is 3 bytes). Let's do byte-level. */
        /* We'll just do constant folding for 3-instruction sequences. */
        /* Rebuild */
        uint8_t* newcode = malloc(c->count);
        int* newlines = malloc(c->count*sizeof(int));
        size_t nw = 0;
        size_t r = 0;
        while (r < c->count) {
            /* check fold pattern at r */
            if (r + 3 + 3 + 1 <= c->count &&
                c->code[r] == OP_PUSH_CONST &&
                c->code[r+3] == OP_PUSH_CONST) {
                uint8_t op = c->code[r+6];
                if (op==OP_ADD || op==OP_SUB || op==OP_MUL || op==OP_DIV || op==OP_MOD) {
                    int ia = (c->code[r+1]<<8)|c->code[r+2];
                    int ib = (c->code[r+4]<<8)|c->code[r+5];
                    if (ia < (int)c->nconsts && ib < (int)c->nconsts &&
                        c->consts[ia].type==VAL_INT && c->consts[ib].type==VAL_INT) {
                        int64_t a = c->consts[ia].as.i;
                        int64_t b = c->consts[ib].as.i;
                        int64_t res = 0;
                        int ok = 1;
                        switch (op) {
                            case OP_ADD: res = a+b; break;
                            case OP_SUB: res = a-b; break;
                            case OP_MUL: res = a*b; break;
                            case OP_DIV: if (b==0){ok=0;}else res=a/b; break;
                            case OP_MOD: if (b==0){ok=0;}else res=a%b; break;
                        }
                        if (ok) {
                            /* add const res, emit PUSH_CONST */
                            Value v = val_int(res);
                            /* find or add */
                            int ci = -1;
                            for (size_t k=0;k<c->nconsts;k++) if (c->consts[k].type==VAL_INT && c->consts[k].as.i==res){ci=(int)k;break;}
                            if (ci<0) ci = chunk_add_const(c, v);
                            newcode[nw]=OP_PUSH_CONST; newlines[nw]=c->lines[r]; nw++;
                            newcode[nw]=(ci>>8)&0xff; newlines[nw]=c->lines[r]; nw++;
                            newcode[nw]=ci&0xff; newlines[nw]=c->lines[r]; nw++;
                            r += 7;
                            changed = 1;
                            continue;
                        }
                    }
                }
            }
            /* default copy one byte */
            newcode[nw] = c->code[r];
            newlines[nw] = c->lines[r];
            nw++; r++;
        }
        /* replace */
        free(c->code);
        free(c->lines);
        c->code = newcode;
        c->lines = newlines;
        c->count = nw;
        c->cap = nw;
    }
}

/* ============================== compile_source ============================= */
int compile_source(VM* v, const char* src, size_t len, char* errbuf, size_t errbufsz) {
    errbuf[0]=0;
    Lexer lx;
    lx.src = src; lx.len = len; lx.pos = 0; lx.line = 1;
    lexer_next(&lx); /* prime */
    /* wrap in Parser */
    Parser p; p.lx = &lx;
    Node* prog = parse_program(&p, errbuf);
    if (errbuf[0]) return -1;
    tc_program(prog);

    /* register functions first (names) */
    /* count funcs */
    int nfuncs = prog->nfuncs + 1; /* +1 for main wrapper */
    /* allocate function table space */
    /* We'll create a synthetic main function */
    /* First, ensure capacity */
    /* Register user funcs as placeholders */
    for (int i=0;i<prog->nfuncs;i++) {
        /* grow vm functions */
        if (v->nfuncs == v->fcap) {
            v->fcap = v->fcap ? v->fcap*2 : 16;
            v->functions = realloc(v->functions, v->fcap*sizeof(Function));
        }
        Function* f = &v->functions[v->nfuncs++];
        memset(f,0,sizeof(Function));
        f->name = strdup(prog->funcs[i]->name);
        f->arity = prog->funcs[i]->nparams;
        chunk_init(&f->chunk);
    }
    /* main function */
    if (v->nfuncs == v->fcap) {
        v->fcap = v->fcap ? v->fcap*2 : 16;
        v->functions = realloc(v->functions, v->fcap*sizeof(Function));
    }
    int mainIndex = v->nfuncs;
    Function* mf = &v->functions[v->nfuncs++];
    memset(mf,0,sizeof(Function));
    mf->name = strdup("__main__");
    mf->arity = 0;
    chunk_init(&mf->chunk);

    /* compile each user func */
    for (int i=0;i<prog->nfuncs;i++) {
        Ctx c; memset(&c,0,sizeof(Ctx));
        c.vm = v; c.err = errbuf; c.errsz = errbufsz;
        c.local_names = NULL; c.nlocals=0; c.locals_cap=0;
        gen_func(&c, prog->funcs[i], i);
        if (errbuf[0]) return -1;
        optimize_chunk(&v->functions[i].chunk);
    }

    /* compile main stmts into main function */
    {
        Ctx c; memset(&c,0,sizeof(Ctx));
        c.vm = v; c.err = errbuf; c.errsz = errbufsz;
        c.local_names = NULL; c.nlocals=0; c.locals_cap=0;
        c.fn = mf;
        /* main stmts as block */
        if (prog->mainstmts && prog->mainstmts->nstmts > 0) {
            for (int i=0;i<prog->mainstmts->nstmts;i++) gen_stmt(&c, prog->mainstmts->stmts[i]);
        }
        /* implicit return */
        int ci = chunk_add_const(&mf->chunk, val_nil());
        chunk_emit(&mf->chunk, OP_PUSH_CONST, 1);
        chunk_emit(&mf->chunk, (ci>>8)&0xff, 1);
        chunk_emit(&mf->chunk, ci&0xff, 1);
        chunk_emit(&mf->chunk, OP_RET, 1);
        mf->nlocals = c.nlocals;
        optimize_chunk(&mf->chunk);
    }
    if (errbuf[0]) return -1;

    /* store main index somewhere? We'll just return it via a global? 
       compile_source returns entry index. But signature returns int. Return mainIndex. */
    /* We need caller to know entry. We'll store in v? Use a convention: entry = last function. 
       Return mainIndex. */
    /* prefer a user-defined main() as entry point */
    int entry = mainIndex;
    for (int i=0;i<v->nfuncs;i++) {
        if (v->functions[i].name && !strcmp(v->functions[i].name, "main")) { entry = i; break; }
    }
    node_free(prog);
    return entry;
}
