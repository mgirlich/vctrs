context("test-type-tibble")

test_that("tibble beats data frame", {
  df <- new_data_frame()
  dt <- tibble::tibble()

  expect_s3_class(vec_ptype_common(dt, df), "tbl_df")
  expect_s3_class(vec_ptype_common(df, dt), "tbl_df")
})

test_that("can cast tibble to df and vice versa", {
  df <- new_data_frame()
  dt <- tibble::tibble()

  expect_equal(vec_cast(df, dt), dt)
  expect_equal(vec_cast(dt, df), df)
})

test_that("can't cast vector to tibble", {
  dt <- tibble::tibble()
  v <- logical()

  expect_error(vec_ptype2(v, dt), class = "vctrs_error_incompatible_type")
  expect_error(vec_ptype2(dt, v), class = "vctrs_error_incompatible_type")
  expect_error(vec_cast(v, dt), class = "vctrs_error_incompatible_cast")
})

test_that("no common type between list and tibble", {
  dt <- tibble::tibble()
  l <- list()

  expect_error(vec_ptype2(l, dt), class = "vctrs_error_incompatible_type")
  expect_error(vec_ptype2(dt, l), class = "vctrs_error_incompatible_type")
})

test_that("can cast a list of 1 or 0 row tibbles to a tibble", {
  dt1 <- tibble::tibble(x = numeric())
  dt2 <- tibble::tibble(x = 1)
  lst <- list(dt1, dt2)

  expect <- tibble::tibble(x = c(NA, 1))

  expect_equal(vec_cast(lst, dt1), expect)
})

test_that("vec_restore restores tibbles", {
  df1 <- tibble::tibble(x = 1:4)
  df2 <- vec_restore(vec_data(df1), df1)

  expect_s3_class(df2, "tbl_df")
})

test_that("the type of a tibble with an unspecified column retains unspecifiedness", {
  df1 <- tibble::tibble(x = 1, y = NA)
  df2 <- tibble::tibble(x = 1, y = unspecified(1))
  expect <- tibble::tibble(x = numeric(), y = unspecified())

  expect_identical(vec_ptype(df1), expect)
  expect_identical(vec_ptype(df2), expect)
})

test_that("vec_ptype_finalise() works recursively over tibbles", {
  df <- tibble(x = numeric(), y = unspecified())
  expect <- tibble(x = numeric(), y = logical())

  expect_identical(vec_ptype_finalise(df), expect)
})

test_that("vec_ptype_finalise() can handle tibble df columns", {
  df <- tibble(x = numeric(), y = tibble(z = unspecified()))
  expect <- tibble(x = numeric(), y = tibble(z = logical()))

  expect_identical(vec_ptype_finalise(df), expect)
})
