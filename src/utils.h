#ifndef VCTRS_UTILS_H
#define VCTRS_UTILS_H

#include "arg-counter.h"


#define SWAP(T, x, y) do {                      \
    T tmp = x;                                  \
    x = y;                                      \
    y = tmp;                                    \
  } while (0)

#define PROTECT_N(x, n) (++*n, PROTECT(x))
#define PROTECT2(x, y) (PROTECT(x), PROTECT(y))

enum vctrs_class_type {
  vctrs_class_list,
  vctrs_class_list_of,
  vctrs_class_data_frame,
  vctrs_class_bare_data_frame,
  vctrs_class_bare_tibble,
  vctrs_class_bare_factor,
  vctrs_class_bare_ordered,
  vctrs_class_rcrd,
  vctrs_class_bare_date,
  vctrs_class_bare_posixct,
  vctrs_class_bare_posixlt,
  vctrs_class_posixlt,
  vctrs_class_unknown,
  vctrs_class_none
};

bool r_is_bool(SEXP x);

SEXP vctrs_eval_mask_n(SEXP fn,
                       SEXP* syms, SEXP* args,
                       SEXP env);
SEXP vctrs_eval_mask1(SEXP fn,
                      SEXP x_sym, SEXP x,
                      SEXP env);
SEXP vctrs_eval_mask2(SEXP fn,
                      SEXP x_sym, SEXP x,
                      SEXP y_sym, SEXP y,
                      SEXP env);
SEXP vctrs_eval_mask3(SEXP fn,
                      SEXP x_sym, SEXP x,
                      SEXP y_sym, SEXP y,
                      SEXP z_sym, SEXP z,
                      SEXP env);
SEXP vctrs_eval_mask4(SEXP fn,
                      SEXP x1_sym, SEXP x1,
                      SEXP x2_sym, SEXP x2,
                      SEXP x3_sym, SEXP x3,
                      SEXP x4_sym, SEXP x4,
                      SEXP env);
SEXP vctrs_eval_mask5(SEXP fn,
                      SEXP x1_sym, SEXP x1,
                      SEXP x2_sym, SEXP x2,
                      SEXP x3_sym, SEXP x3,
                      SEXP x4_sym, SEXP x4,
                      SEXP x5_sym, SEXP x5,
                      SEXP env);

SEXP vctrs_dispatch_n(SEXP fn_sym, SEXP fn,
                      SEXP* syms, SEXP* args);
SEXP vctrs_dispatch1(SEXP fn_sym, SEXP fn,
                     SEXP x_sym, SEXP x);
SEXP vctrs_dispatch2(SEXP fn_sym, SEXP fn,
                     SEXP x_sym, SEXP x,
                     SEXP y_sym, SEXP y);
SEXP vctrs_dispatch3(SEXP fn_sym, SEXP fn,
                     SEXP x_sym, SEXP x,
                     SEXP y_sym, SEXP y,
                     SEXP z_sym, SEXP z);
SEXP vctrs_dispatch4(SEXP fn_sym, SEXP fn,
                     SEXP w_sym, SEXP w,
                     SEXP x_sym, SEXP x,
                     SEXP y_sym, SEXP y,
                     SEXP z_sym, SEXP z);

SEXP map(SEXP x, SEXP (*fn)(SEXP));
SEXP map_with_data(SEXP x, SEXP (*fn)(SEXP, void*), void* data);
SEXP df_map(SEXP df, SEXP (*fn)(SEXP));
SEXP bare_df_map(SEXP df, SEXP (*fn)(SEXP));

enum vctrs_class_type class_type(SEXP x);
bool is_data_frame(SEXP x);
bool is_bare_data_frame(SEXP x);
bool is_bare_tibble(SEXP x);
bool is_record(SEXP x);

SEXP vec_unique_names(SEXP x, bool quiet);
SEXP vec_unique_colnames(SEXP x, bool quiet);

// Returns S3 method for `generic` suitable for the class of `x`. The
// inheritance hierarchy is explored except for the default method.
SEXP s3_find_method(const char* generic, SEXP x, SEXP table);
bool vec_implements_ptype2(SEXP x);

SEXP list_first_non_null(SEXP xs, R_len_t* non_null_i);
bool list_is_s3_homogeneous(SEXP xs);

// Destructive compacting
SEXP node_compact_d(SEXP xs);

extern struct vctrs_arg* args_empty;
SEXP arg_validate(SEXP arg, const char* arg_nm);

void never_reached(const char* fn) __attribute__((noreturn));

enum vctrs_type2 vec_typeof2_impl(enum vctrs_type type_x, enum vctrs_type type_y, int* left);
enum vctrs_type2_s3 vec_typeof2_s3_impl(SEXP x, SEXP y, enum vctrs_type type_x, enum vctrs_type type_y, int* left);

enum vctrs_class_type class_type(SEXP x);

SEXP new_list_of(SEXP x, SEXP ptype);
void init_list_of(SEXP x, SEXP ptype);

SEXP new_empty_factor(SEXP levels);
SEXP new_empty_ordered(SEXP levels);

bool list_has_inner_vec_names(SEXP x, R_len_t size);

void init_compact_seq(int* p, R_len_t start, R_len_t size, bool increasing);
SEXP compact_seq(R_len_t start, R_len_t size, bool increasing);
bool is_compact_seq(SEXP x);

void init_compact_rep(int* p, R_len_t i, R_len_t n);
SEXP compact_rep(R_len_t i, R_len_t n);
bool is_compact_rep(SEXP x);

bool is_compact(SEXP x);
SEXP compact_materialize(SEXP x);
R_len_t vec_subscript_size(SEXP x);

bool is_integer64(SEXP x);

SEXP apply_name_spec(SEXP name_spec, SEXP outer, SEXP inner, R_len_t n);
SEXP outer_names(SEXP names, SEXP outer, R_len_t n);
SEXP vec_set_names(SEXP x, SEXP names);
SEXP colnames(SEXP x);

R_len_t size_validate(SEXP size, const char* arg);

extern bool (*rlang_is_splice_box)(SEXP);
extern SEXP (*rlang_unbox)(SEXP);
extern SEXP (*rlang_env_dots_values)(SEXP);
extern SEXP (*rlang_env_dots_list)(SEXP);

void* r_vec_deref(SEXP x);
const void* r_vec_const_deref(SEXP x);

void r_vec_ptr_inc(SEXPTYPE type, void** p, R_len_t i);
void r_vec_fill(SEXPTYPE type, void* p, const void* value_p, R_len_t value_i, R_len_t n);

R_len_t r_lgl_sum(SEXP lgl, bool na_true);
SEXP r_lgl_which(SEXP x, bool na_true);

void r_lgl_fill(SEXP x, int value, R_len_t n);
void r_int_fill(SEXP x, int value, R_len_t n);
void r_chr_fill(SEXP x, SEXP value, R_len_t n);

void r_int_fill_seq(SEXP x, int start, R_len_t n);
SEXP r_seq(R_len_t from, R_len_t to);
bool r_int_any_na(SEXP x);

R_len_t r_chr_find(SEXP x, SEXP value);

#define r_resize Rf_xlengthgets

int r_chr_max_len(SEXP x);
SEXP r_chr_iota(R_len_t n, char* buf, int len, const char* prefix);

SEXP r_new_environment(SEXP parent, R_len_t size);
SEXP r_new_function(SEXP formals, SEXP body, SEXP env);
SEXP r_as_function(SEXP x, const char* arg);

SEXP r_protect(SEXP x);
bool r_is_true(SEXP x);
bool r_is_string(SEXP x);
bool r_is_number(SEXP x);
SEXP r_peek_option(const char* option);
SEXP r_maybe_duplicate(SEXP x);

SEXP r_pairlist(SEXP* tags, SEXP* cars);
SEXP r_call(SEXP fn, SEXP* tags, SEXP* cars);

static inline SEXP r_names(SEXP x) {
  return Rf_getAttrib(x, R_NamesSymbol);
}
static inline SEXP r_poke_names(SEXP x, SEXP names) {
  return Rf_setAttrib(x, R_NamesSymbol, names);
}
static inline SEXP r_class(SEXP x) {
  return Rf_getAttrib(x, R_ClassSymbol);
}
static inline SEXP r_poke_class(SEXP x, SEXP names) {
  return Rf_setAttrib(x, R_ClassSymbol, names);
}

bool r_has_name_at(SEXP names, R_len_t i);
bool r_is_names(SEXP names);
bool r_is_minimal_names(SEXP x);
bool r_is_empty_names(SEXP x);
SEXP r_env_get(SEXP env, SEXP sym);
bool r_is_function(SEXP x);
bool r_chr_has_string(SEXP x, SEXP str);

static inline const char* r_chr_get_c_string(SEXP chr, R_len_t i) {
  return CHAR(STRING_ELT(chr, i));
}

static inline void r__vec_get_check(SEXP x, R_len_t i, const char* fn) {
  if ((Rf_length(x) - 1) < i) {
    Rf_error("Internal error in `%s()`: Vector is too small", fn);
  }
}
static inline int r_lgl_get(SEXP x, R_len_t i) {
  r__vec_get_check(x, i, "r_lgl_get");
  return LOGICAL(x)[i];
}
static inline int r_int_get(SEXP x, R_len_t i) {
  r__vec_get_check(x, i, "r_int_get");
  return INTEGER(x)[i];
}
static inline double r_dbl_get(SEXP x, R_len_t i) {
  r__vec_get_check(x, i, "r_dbl_get");
  return REAL(x)[i];
}
#define r_chr_get STRING_ELT

static inline void* r_vec_unwrap(SEXPTYPE type, SEXP x) {
  switch (type) {
  case INTSXP: return (void*) INTEGER(x);
  default: Rf_error("Internal error: Unimplemented type in `r_vec_unwrap()`.");
  }
}

#define r_lgl Rf_ScalarLogical
#define r_int Rf_ScalarInteger
#define r_str Rf_mkChar
#define r_sym Rf_install

static inline SEXP r_list(SEXP x) {
  SEXP out = Rf_allocVector(VECSXP, 1);
  SET_VECTOR_ELT(out, 0, x);
  return out;
}

#define r_str_as_character Rf_ScalarString

static inline SEXP r_sym_as_character(SEXP x) {
  return r_str_as_character(PRINTNAME(x));
}
// This unserialises ASCII Unicode tags of the form `<U+xxxx>`
extern SEXP (*rlang_sym_as_character)(SEXP x);

SEXP r_as_data_frame(SEXP x);

static inline void r_dbg_save(SEXP x, const char* name) {
  Rf_defineVar(Rf_install(name), x, R_GlobalEnv);
}

ERR r_try_catch(void (*fn)(void*),
                void* fn_data,
                SEXP cnd_sym,
                void (*hnd)(void*),
                void* hnd_data);

extern SEXP vctrs_ns_env;
extern SEXP syms_cnd_signal;
static inline void r_cnd_signal(SEXP cnd) {
  SEXP call = PROTECT(Rf_lang2(syms_cnd_signal, cnd));
  Rf_eval(call, vctrs_ns_env);
  UNPROTECT(1);
}

extern SEXP result_attrib;

static inline SEXP r_result(SEXP x, ERR err) {
  if (!err) {
    err = R_NilValue;
  }

  SEXP result = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(result, 0, x);
  SET_VECTOR_ELT(result, 1, err);

  SET_ATTRIB(result, result_attrib);
  SET_OBJECT(result, 1);

  UNPROTECT(1);
  return result;
}

static inline SEXP r_result_get(SEXP x, ERR err) {
  if (err) {
    r_cnd_signal(err);
  }

  return x;
}

static inline struct vctrs_arg vec_as_arg(SEXP x) {
  if (x == R_NilValue) {
    return *args_empty;
  } else {
    return new_wrapper_arg(NULL, r_chr_get_c_string(x, 0));
  }
}

extern SEXP fns_quote;
static inline SEXP expr_protect(SEXP x) {
  switch (TYPEOF(x)) {
  case SYMSXP:
  case LANGSXP:
    return Rf_lang2(fns_quote, x);
  default:
    return x;
  }
}


extern SEXP vctrs_ns_env;
extern SEXP vctrs_shared_empty_str;
extern SEXP vctrs_shared_na_lgl;
extern SEXP vctrs_shared_zero_int;

extern SEXP classes_data_frame;
extern SEXP classes_factor;
extern SEXP classes_ordered;
extern SEXP classes_date;
extern SEXP classes_posixct;
extern SEXP classes_tibble;
extern SEXP classes_list_of;
extern SEXP classes_vctrs_group_rle;

extern SEXP strings_dots;
extern SEXP strings_empty;
extern SEXP strings_tbl;
extern SEXP strings_tbl_df;
extern SEXP strings_data_frame;
extern SEXP strings_vctrs_rcrd;
extern SEXP strings_date;
extern SEXP strings_posixct;
extern SEXP strings_posixlt;
extern SEXP strings_posixt;
extern SEXP strings_factor;
extern SEXP strings_ordered;
extern SEXP strings_vctrs_vctr;
extern SEXP strings_vctrs_list_of;
extern SEXP strings_list;
extern SEXP strings_none;
extern SEXP strings_minimal;
extern SEXP strings_unique;
extern SEXP strings_universal;
extern SEXP strings_check_unique;
extern SEXP strings_key;
extern SEXP strings_loc;
extern SEXP strings_val;
extern SEXP strings_group;
extern SEXP strings_length;

extern SEXP chrs_subset;
extern SEXP chrs_extract;
extern SEXP chrs_assign;
extern SEXP chrs_rename;
extern SEXP chrs_remove;
extern SEXP chrs_negate;
extern SEXP chrs_numeric;
extern SEXP chrs_character;
extern SEXP chrs_empty;
extern SEXP chrs_cast;
extern SEXP chrs_error;

extern SEXP syms_i;
extern SEXP syms_n;
extern SEXP syms_x;
extern SEXP syms_y;
extern SEXP syms_to;
extern SEXP syms_dots;
extern SEXP syms_bracket;
extern SEXP syms_arg;
extern SEXP syms_x_arg;
extern SEXP syms_y_arg;
extern SEXP syms_to_arg;
extern SEXP syms_subscript_arg;
extern SEXP syms_out;
extern SEXP syms_value;
extern SEXP syms_quiet;
extern SEXP syms_dot_name_spec;
extern SEXP syms_outer;
extern SEXP syms_inner;
extern SEXP syms_tilde;
extern SEXP syms_dot_environment;
extern SEXP syms_ptype;
extern SEXP syms_missing;
extern SEXP syms_size;
extern SEXP syms_subscript_action;
extern SEXP syms_subscript_type;
extern SEXP syms_repair;
extern SEXP syms_tzone;
extern SEXP syms_data;
extern SEXP syms_vctrs_error_incompatible_type;
extern SEXP syms_vctrs_error_cast_lossy;
extern SEXP syms_cnd_signal;
extern SEXP syms_logical;
extern SEXP syms_numeric;
extern SEXP syms_character;
extern SEXP syms_body;
extern SEXP syms_parent;

#define syms_names R_NamesSymbol

extern SEXP fns_bracket;
extern SEXP fns_quote;
extern SEXP fns_names;


extern SEXP vctrs_method_table;
extern SEXP base_method_table;


#endif
