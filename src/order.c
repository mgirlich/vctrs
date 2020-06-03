#include "vctrs.h"
#include "utils.h"

struct order_info {
  SEXP out;
  int* p_out;

  SEXP copy;
  int* p_copy;
};

enum na_last {
  NA_LAST_true,
  NA_LAST_false
};

// -----------------------------------------------------------------------------

#define UINT8_MAX_SIZE (UINT8_MAX + 1)

static inline uint32_t map_from_int32_to_uint32(int32_t x);
static inline uint8_t extract_byte(uint32_t x, uint8_t pass);

static void int_radix_order(SEXP x,
                            const enum na_last na_last,
                            R_xlen_t size,
                            struct order_info* p_info) {
  static const uint8_t n_passes = 4;

  const int* p_x = INTEGER_RO(x);

  SEXP out = p_info->out;
  int* p_out = p_info->p_out;

  SEXP copy = p_info->copy;
  int* p_copy = p_info->p_copy;

  // Tracks the counts of each byte seen.
  // It is a long array that gets broken into `n_passes` parts.
  R_xlen_t* p_counts = (R_xlen_t*) R_alloc(UINT8_MAX_SIZE * n_passes, sizeof(R_xlen_t));
  memset(p_counts, 0, UINT8_MAX_SIZE * n_passes * sizeof(R_xlen_t));

  // Tracks the bytes themselves, since computing them requires some extra
  // work of mapping `x` to `uint32_t` before extracting bytes.
  uint8_t* p_bytes = (uint8_t*) R_alloc(size * n_passes, sizeof(uint8_t));

  // For jumping along the bytes/counts arrays
  R_xlen_t pass_start_bytes[n_passes];
  R_xlen_t pass_start_counts[n_passes];

  for (R_xlen_t pass = 0; pass < n_passes; ++pass) {
    pass_start_bytes[pass] = size * pass;
    pass_start_counts[pass] = UINT8_MAX_SIZE * pass;
  }

  // Rely on `NA_INTEGER == INT_MIN`, which is mapped to `0` as a `uint32_t`.
  // If `na_last = NA_LAST_true`, shift all `uint32_t` values down 1 to be able
  // to map `NA` to the maximum `uint32_t` value.
  const uint32_t na_uint32 = (na_last == NA_LAST_true) ? UINT32_MAX : 0;
  const int32_t na_last_adjustment = (na_last == NA_LAST_true) ? -1 : 0;

  // Build 4 histograms in one pass (one for each byte)
  for (R_xlen_t i = 0; i < size; ++i) {
    const int32_t elt_x = p_x[i];

    uint32_t elt_mapped;

    if (elt_x == NA_INTEGER) {
      elt_mapped = na_uint32;
    } else {
      elt_mapped = map_from_int32_to_uint32(elt_x) + na_last_adjustment;
    }

    for (uint8_t pass = 0; pass < n_passes; ++pass) {
      const uint8_t byte = extract_byte(elt_mapped, pass);
      p_bytes[pass_start_bytes[pass] + i] = byte;
      ++p_counts[pass_start_counts[pass] + byte];
    }
  }

  R_xlen_t p_offsets[UINT8_MAX_SIZE];

  for (uint8_t pass = 0; pass < n_passes; ++pass) {
    const R_xlen_t start_bytes = pass_start_bytes[pass];
    const R_xlen_t start_counts = pass_start_counts[pass];

    R_xlen_t offset = 0;
    bool pass_skip = false;

    for (R_xlen_t i = 0; i < UINT8_MAX_SIZE; ++i) {
      const R_xlen_t count = p_counts[start_counts + i];

      // Skip this pass entirely if all bytes are identical, which happens
      // when a bucket holds a count equal to the number of values.
      if (count == size) {
        pass_skip = true;
        break;
      }

      p_offsets[i] = offset;
      offset += count;
    }

    if (pass_skip) {
      continue;
    }

    for (R_xlen_t i = 0; i < size; ++i) {
      const int32_t elt = p_out[i];
      const uint8_t loc = p_bytes[start_bytes + elt - 1];

      p_copy[p_offsets[loc]++] = elt;
    }

    // Pointer swap before next pass
    SEXP temp = out;
    out = copy;
    copy = temp;
    p_out = INTEGER(out);
    p_copy = INTEGER(copy);
  }

  // Update pointers after all the swapping
  p_info->out = out;
  p_info->p_out = p_out;
  p_info->copy = copy;
  p_info->p_copy = p_copy;
}

#undef UINT8_MAX_SIZE


#define HEX_UINT32_SIGN_BIT 0x80000000u

// [INT32_MIN, INT32_MAX] => [0, UINT32_MAX]
static inline uint32_t map_from_int32_to_uint32(int32_t x) {
  return x ^ HEX_UINT32_SIGN_BIT;
}

#undef HEX_UINT32_SIGN_BIT


static inline uint8_t extract_byte(uint32_t x, uint8_t pass) {
  return (x >> (8 * pass)) & UINT8_MAX;
}

// -----------------------------------------------------------------------------

static void vec_radix_order_switch(SEXP x,
                                   const enum na_last na_last,
                                   R_xlen_t size,
                                   struct order_info* p_info);


static void df_radix_order(SEXP x,
                           const enum na_last na_last,
                           R_xlen_t size,
                           struct order_info* p_info) {
  R_xlen_t n_cols = Rf_xlength(x);

  // Iterate over columns backwards to sort correctly
  for (R_xlen_t i = 0; i < n_cols; ++i) {
    R_xlen_t j = n_cols - 1 - i;
    SEXP col = VECTOR_ELT(x, j);
    vec_radix_order_switch(col, na_last, size, p_info);
  }
}

// -----------------------------------------------------------------------------

static void vec_radix_order_switch(SEXP x,
                                   const enum na_last na_last,
                                   R_xlen_t size,
                                   struct order_info* p_info) {
  switch (vec_proxy_typeof(x)) {
  case vctrs_type_integer: int_radix_order(x, na_last, size, p_info); return;
  case vctrs_type_dataframe: df_radix_order(x, na_last, size, p_info); return;
  default: Rf_errorcall(R_NilValue, "This type is not supported by `vec_radix_order()`");
  }
}

// -----------------------------------------------------------------------------

static SEXP vec_radix_order(SEXP x, const enum na_last na_last);

// [[ register() ]]
SEXP vctrs_radix_order(SEXP x, SEXP na_last) {
  if (!r_is_bool(na_last)) {
    Rf_errorcall(R_NilValue, "`na_last` must be either `TRUE` or `FALSE`.");
  }

  enum na_last c_na_last = (LOGICAL(na_last)[0]) ? NA_LAST_true : NA_LAST_false;

  return vec_radix_order(x, c_na_last);
}

static SEXP vec_radix_order(SEXP x, const enum na_last na_last) {
  // TODO:
  // x = PROTECT(vec_proxy_compare(x));

  // TODO:
  // Should proxy-compare flatten df-cols?
  // How to track vector of `decreasing` if so?

  R_xlen_t size = vec_size(x);

  SEXP out = PROTECT(Rf_allocVector(INTSXP, size));
  int* p_out = INTEGER(out);

  // Initialize `out` with sequential 1-based ordering
  for (R_xlen_t i = 0; i < size; ++i) {
    p_out[i] = i + 1;
  }

  // To store intermediate results after each pass
  SEXP copy = PROTECT(Rf_allocVector(INTSXP, size));
  int* p_copy = INTEGER(copy);

  struct order_info info = {
    .out = out,
    .p_out = p_out,
    .copy = copy,
    .p_copy = p_copy
  };

  vec_radix_order_switch(x, na_last, size, &info);

  UNPROTECT(2);
  return info.out;
}
