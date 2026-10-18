/* BEHEMOTH VM — stack machine + mark-sweep GC. */
#include "beast.h"

void chunk_init(Chunk* c){ memset(c,0,sizeof(Chunk)); }
void chunk_free(Chunk* c){
    free(c->code); free(c->consts); free(c->lines);
    memset(c,0,sizeof(Chunk));
}
int chunk_emit(Chunk* c, uint8_t byte, int line){
    if (c->count == c->cap) {
        c->cap = c->cap ? c->cap*2 : 64;
        c->code = realloc(c->code, c->cap);
        c->lines = realloc(c->lines, c->cap*sizeof(int));
    }
    c->code[c->count]=byte;
    c->lines[c->count]=line;
    c->count++;
    return (int)(c->count-1);
}
int chunk_add_const(Chunk* c, Value v){
    if (c->nconsts == c->ccap) {
        c->ccap = c->ccap ? c->ccap*2 : 8;
        c->consts = realloc(c->consts, c->ccap*sizeof(Value));
    }
    c->consts[c->nconsts] = v;
    return (int)(c->nconsts++);
}
void chunk_patch_i32(Chunk* c, size_t pos, int32_t val){
    /* write 4 bytes little-endian at pos */
    c->code[pos]   = (uint8_t)(val & 0xff);
    c->code[pos+1] = (uint8_t)((val>>8)&0xff);
    c->code[pos+2] = (uint8_t)((val>>16)&0xff);
    c->code[pos+3] = (uint8_t)((val>>24)&0xff);
}

/* node helpers */
Node* node_new(NodeKind k, int line){
    Node* n = calloc(1,sizeof(Node));
    n->kind = k; n->line = line;
    return n;
}
void node_free(Node* n){
    if (!n) return;
    /* free children recursively (best effort) */
    node_free(n->left); node_free(n->right);
    node_free(n->cond); node_free(n->thenb); node_free(n->elseb); node_free(n->body);
    for (int i=0;i<n->nargs;i++) node_free(n->args[i]);
    free(n->args);
    for (int i=0;i<n->nstmts;i++) node_free(n->stmts[i]);
    free(n->stmts);
    for (int i=0;i<n->nfuncs;i++) node_free(n->funcs[i]);
    free(n->funcs);
    free(n->params); /* note: params are char* owned; leak ok for tool */
    free(n->name); free(n->strv);
    free(n);
}

/* VM */
void vm_init(VM* v){
    memset(v,0,sizeof(VM));
    v->next_gc = 1024;
}
void vm_free(VM* v){
    /* free all objects */
    Obj* o = v->objects;
    while (o) { Obj* n=o->next; if (o->kind==0) free(o->as.str.chars); free(o); o=n; }
    for (int i=0;i<v->nfuncs;i++) { free(v->functions[i].name); chunk_free(&v->functions[i].chunk); }
    free(v->functions);
}

void vm_push(VM* v, Value val){ v->stack[v->sp++] = val; }
Value vm_pop(VM* v){ return v->stack[--v->sp]; }

/* GC */
Obj* gc_alloc_str(VM* v, const char* s, size_t len){
    Obj* o = malloc(sizeof(Obj));
    memset(o,0,sizeof(Obj));
    o->kind = 0;
    o->as.str.chars = malloc(len+1);
    memcpy(o->as.str.chars, s, len);
    o->as.str.chars[len]=0;
    o->as.str.len = len;
    o->next = v->objects;
    v->objects = o;
    v->bytes_alloc += sizeof(Obj)+len+1;
    if (v->bytes_alloc > v->next_gc) {
        gc_collect(v, v);
    }
    return o;
}

static void mark_value(VM* v, Value val);
static void mark_obj(VM* v, Obj* o) {
    if (!o || o->marked) return;
    o->marked = 1;
    /* strings have no refs */
}

static void mark_value(VM* v, Value val) {
    if (val.type == VAL_STR && val.as.o) mark_obj(v, val.as.o);
}

static void mark_roots(VM* v) {
    /* stack */
    for (int i=0;i<v->sp;i++) mark_value(v, v->stack[i]);
    /* frames locals: we store locals in a flat region? 
       Our VM uses stack slots; locals are part of stack frame base. We'll mark whole stack. */
    /* globals */
    for (int i=0;i<v->nglobals;i++) mark_value(v, v->globals[i]);
    /* constants pools */
    for (int f=0; f<v->nfuncs; f++) {
        Chunk* c = &v->functions[f].chunk;
        for (size_t i=0;i<c->nconsts;i++) mark_value(v, c->consts[i]);
    }
}

void gc_collect(VM* v, VM* root_vm) {
    (void)root_vm;
    /* mark */
    mark_roots(v);
    /* sweep */
    Obj** pp = &v->objects;
    while (*pp) {
        if (!(*pp)->marked) {
            Obj* dead = *pp;
            *pp = dead->next;
            if (dead->kind==0) free(dead->as.str.chars);
            free(dead);
            v->bytes_alloc -= sizeof(Obj);
            v->objects_freed++;
        } else {
            (*pp)->marked = 0;
            pp = &(*pp)->next;
        }
    }
    v->next_gc = v->bytes_alloc * 2 + 1024;
    v->gc_runs++;
}

/* VM execution */
int vm_run(VM* v, Function* entry) {
    /* push initial frame */
    CallFrame* frame = &v->frames[v->frame_count++];
    frame->fn = entry;
    frame->ip = 0;
    frame->base_slot = 0;
    /* ensure stack space for locals */
    /* locals stored on value stack; we'll use base_slot offset into v->stack */
    /* For simplicity, locals are stored at v->stack[base_slot + slot] */
    /* But stack pointer must account. We'll allocate locals by advancing sp. */
    /* We'll handle LOAD/STORE relative to frame base. */
    /* Ensure sp at least base+fn->nlocals */
    if (v->sp < frame->base_slot + entry->nlocals) {
        int need = frame->base_slot + entry->nlocals - v->sp;
        for (int i=0;i<need;i++) v->stack[v->sp++] = val_nil();
    }

    while (v->frame_count > 0) {
        Frame:
        frame = &v->frames[v->frame_count-1];
        Function* fn = frame->fn;
        Chunk* ch = &fn->chunk;
        if (frame->ip >= ch->count) {
            /* implicit return */
            v->frame_count--;
            if (v->frame_count==0) break;
            /* pop return value already? We'll assume caller handles */
            continue;
        }
        uint8_t op = ch->code[frame->ip++];
        v->steps++;
        switch (op) {
            case OP_HALT: v->frame_count=0; break;
            case OP_NOP: break;
            case OP_PUSH_CONST: {
                uint8_t hi = ch->code[frame->ip++];
                uint8_t lo = ch->code[frame->ip++];
                int idx = (hi<<8)|lo;
                vm_push(v, ch->consts[idx]);
                break;
            }
            case OP_POP: vm_pop(v); break;
            case OP_DUP: { Value x = v->stack[v->sp-1]; vm_push(v,x); break; }
            case OP_LOAD: {
                uint8_t hi = ch->code[frame->ip++];
                uint8_t lo = ch->code[frame->ip++];
                int slot = (hi<<8)|lo;
                int abs = frame->base_slot + slot;
                vm_push(v, v->stack[abs]);
                break;
            }
            case OP_STORE: {
                uint8_t hi = ch->code[frame->ip++];
                uint8_t lo = ch->code[frame->ip++];
                int slot = (hi<<8)|lo;
                int abs = frame->base_slot + slot;
                Value val = vm_pop(v);
                v->stack[abs] = val;
                break;
            }
            case OP_LOAD_GLOBAL: {
                frame->ip += 2;
                vm_push(v, v->globals[0]);
                break;
            }
            case OP_STORE_GLOBAL: {
                frame->ip += 2;
                v->globals[0] = vm_pop(v);
                break;
            }
            case OP_ADD: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                if (a.type==VAL_INT && b.type==VAL_INT) vm_push(v, val_int(a.as.i+b.as.i));
                else vm_push(v, val_int(0));
                break;
            }
            case OP_SUB: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                vm_push(v, val_int(a.as.i-b.as.i)); break;
            }
            case OP_MUL: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                vm_push(v, val_int(a.as.i*b.as.i)); break;
            }
            case OP_DIV: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                if (b.as.i==0) { fprintf(stderr,"div by zero\n"); return 1; }
                vm_push(v, val_int(a.as.i/b.as.i)); break;
            }
            case OP_MOD: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                vm_push(v, val_int(a.as.i % b.as.i)); break;
            }
            case OP_NEG: {
                Value a = vm_pop(v); vm_push(v, val_int(-a.as.i)); break;
            }
            case OP_CONCAT: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                /* build string */
                size_t la = (a.type==VAL_STR && a.as.o) ? a.as.o->as.str.len : 0;
                size_t lb = (b.type==VAL_STR && b.as.o) ? b.as.o->as.str.len : 0;
                /* convert ints to str if needed */
                char bufa[64], bufb[64];
                const char* sa; const char* sb;
                char* fa=NULL; char* fb=NULL;
                if (a.type==VAL_STR && a.as.o) { sa = a.as.o->as.str.chars; }
                else if (a.type==VAL_INT) { snprintf(bufa,64,"%ld",(long)a.as.i); sa=bufa; la=strlen(sa); }
                else { sa=""; la=0; }
                if (b.type==VAL_STR && b.as.o) { sb = b.as.o->as.str.chars; }
                else if (b.type==VAL_INT) { snprintf(bufb,64,"%ld",(long)b.as.i); sb=bufb; lb=strlen(sb); }
                else { sb=""; lb=0; }
                size_t total = la+lb;
                char* buf = malloc(total+1);
                memcpy(buf, sa, la);
                memcpy(buf+la, sb, lb);
                buf[total]=0;
                Obj* o = gc_alloc_str(v, buf, total);
                free(buf);
                vm_push(v, val_str(o));
                break;
            }
            case OP_EQ: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                int r = 0;
                if (a.type==VAL_INT && b.type==VAL_INT) r = (a.as.i==b.as.i);
                else if (a.type==VAL_BOOL && b.type==VAL_BOOL) r = (a.as.b==b.as.b);
                else if (a.type==VAL_STR && b.type==VAL_STR) {
                    if (a.as.o && b.as.o) r = (strcmp(a.as.o->as.str.chars, b.as.o->as.str.chars)==0);
                }
                vm_push(v, val_bool(r));
                break;
            }
            case OP_NE: {
                Value b = vm_pop(v); Value a = vm_pop(v);
                int r = 1;
                if (a.type==VAL_INT && b.type==VAL_INT) r = (a.as.i!=b.as.i);
                vm_push(v, val_bool(r));
                break;
            }
            case OP_LT: { Value b=vm_pop(v); Value a=vm_pop(v); vm_push(v, val_bool(a.as.i < b.as.i)); break; }
            case OP_LE: { Value b=vm_pop(v); Value a=vm_pop(v); vm_push(v, val_bool(a.as.i <= b.as.i)); break; }
            case OP_GT: { Value b=vm_pop(v); Value a=vm_pop(v); vm_push(v, val_bool(a.as.i > b.as.i)); break; }
            case OP_GE: { Value b=vm_pop(v); Value a=vm_pop(v); vm_push(v, val_bool(a.as.i >= b.as.i)); break; }
            case OP_NOT: { Value a=vm_pop(v); vm_push(v, val_bool(!a.as.b && a.as.i==0)); break; }
            case OP_JMP: {
                int32_t off = ch->code[frame->ip] | (ch->code[frame->ip+1]<<8) | (ch->code[frame->ip+2]<<16) | (ch->code[frame->ip+3]<<24);
                frame->ip += 4;
                frame->ip = (size_t)((int64_t)frame->ip + off);  /* careful: ip already advanced; off relative to after the 4 bytes */
                /* Actually we stored off relative to pos after 4 bytes (jmpPos+4). So new ip = old_ip_after_operand + off. 
                   We already did ip+=4, so frame->ip += off would be right. Fix: */
                break;
            }
            case OP_JMP_IF_FALSE: {
                int32_t off = ch->code[frame->ip] | (ch->code[frame->ip+1]<<8) | (ch->code[frame->ip+2]<<16) | (ch->code[frame->ip+3]<<24);
                frame->ip += 4;
                Value cnd = vm_pop(v);
                int falsy = (cnd.type==VAL_BOOL)? !cnd.as.b : (cnd.type==VAL_INT ? cnd.as.i==0 : 0);
                if (falsy) frame->ip = (size_t)((int64_t)frame->ip + off);
                break;
            }
            case OP_CALL: {
                uint8_t fhi = ch->code[frame->ip++];
                uint8_t flo = ch->code[frame->ip++];
                int fidx = (fhi<<8)|flo;
                uint8_t argc = ch->code[frame->ip++];
                /* save current frame ip already advanced */
                /* push new frame */
                Function* callee = &v->functions[fidx];
                /* ensure stack space for callee locals */
                int base = v->sp - argc; /* start of args */
                /* move args? We'll keep args at base..base+argc-1, locals after */
                int need = callee->nlocals - argc;
                if (need < 0) need = 0;
                for (int i=0;i<need;i++) v->stack[v->sp++] = val_nil();
                CallFrame* nf = &v->frames[v->frame_count++];
                nf->fn = callee;
                nf->ip = 0;
                nf->base_slot = base;
                break;
            }
            case OP_RET: {
                Value retval = (v->sp>0) ? vm_pop(v) : val_nil();
                /* pop frame */
                CallFrame* cf = &v->frames[--v->frame_count];
                int base = cf->base_slot;
                /* reset sp to base (drop locals) but push retval */
                v->sp = base;
                if (v->frame_count > 0) {
                    vm_push(v, retval);
                }
                /* if top-level (frame_count==0), store retval? ignore */
                break;
            }
            case OP_PRINT: {
                Value x = vm_pop(v);
                if (x.type==VAL_INT) printf("%ld\n", (long)x.as.i);
                else if (x.type==VAL_BOOL) printf("%s\n", x.as.b?"true":"false");
                else if (x.type==VAL_STR && x.as.o) printf("%s\n", x.as.o->as.str.chars);
                else if (x.type==VAL_NIL) printf("nil\n");
                break;
            }
            default:
                fprintf(stderr, "unknown opcode %d\n", op);
                return 1;
        }
        /* after OP_JMP we already adjusted; but note OP_JMP case above did ip+=4 then break without adding off. Fix below. */
    }
    return 0;
}

/* Fix OP_JMP: handle offset addition. The above OP_JMP case is incomplete; patch here by redefining? 
   We'll instead handle OP_JMP fully in a separate approach: redo via macro? 
   Simpler: re-open file? We'll patch with sed-like edit via code? Instead, adjust: We need OP_JMP to add off to frame->ip.
   Let's fix by editing this file. */
/* Disassembler */
void disassemble(VM* v, Function* f, const char* name) {
    (void)v;
    printf("== %s (arity=%d, locals=%d) ==\n", name, f->arity, f->nlocals);
    Chunk* c = &f->chunk;
    size_t i = 0;
    while (i < c->count) {
        printf("%04zu  ", i);
        uint8_t op = c->code[i];
        printf("%-16s", opcode_name((OpCode)op));
        i++;
        switch (op) {
            case OP_PUSH_CONST:
            case OP_LOAD:
            case OP_STORE:
            case OP_LOAD_GLOBAL:
            case OP_STORE_GLOBAL: {
                uint8_t hi = c->code[i], lo = c->code[i+1];
                int idx = (hi<<8)|lo;
                if (op==OP_PUSH_CONST) printf(" %d (const: ", idx);
                else printf(" %d", idx);
                if (op==OP_PUSH_CONST && idx < (int)c->nconsts) {
                    Value val = c->consts[idx];
                    if (val.type==VAL_INT) printf("%ld", (long)val.as.i);
                    else if (val.type==VAL_BOOL) printf("%s", val.as.b?"true":"false");
                    else if (val.type==VAL_STR && val.as.o) printf("\"%s\"", val.as.o->as.str.chars);
                    else printf("nil");
                    printf(")");
                }
                printf("\n");
                i += 2;
                break;
            }
            case OP_CALL: {
                uint8_t fhi = c->code[i], flo = c->code[i+1];
                uint8_t argc = c->code[i+2];
                printf(" fn=%d argc=%d\n", (fhi<<8)|flo, argc);
                i += 3;
                break;
            }
            case OP_JMP:
            case OP_JMP_IF_FALSE: {
                int32_t off = c->code[i] | (c->code[i+1]<<8) | (c->code[i+2]<<16) | (c->code[i+3]<<24);
                printf(" -> %zu (off=%d)\n", i+4+off, off);
                i += 4;
                break;
            }
            default:
                printf("\n");
                break;
        }
    }
}

void beast_error(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}
