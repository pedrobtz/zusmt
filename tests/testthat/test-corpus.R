# Runs every .smt2 file shipped in inst/smt2. Each file declares its own logic
# and expected result in header comments, so adding a regression case is one
# file and no edit here -- and a file whose header is missing or malformed
# fails rather than being silently skipped.

corpus_files <- function() {
  dir <- system.file("smt2", package = "zusmt")
  skip_if(dir == "", "corpus not installed")
  list.files(dir, pattern = "\\.smt2$", full.names = TRUE)
}

corpus_header <- function(path, field) {
  lines <- readLines(path, warn = FALSE)
  hit <- grep(paste0("^; *", field, " *:"), lines, value = TRUE)
  if (length(hit) != 1L) {
    stop("file ", basename(path), " must declare exactly one '; ", field, ":' header")
  }
  trimws(sub(paste0("^; *", field, " *:"), "", hit[1]))
}

test_that("the corpus is present and every file declares a logic and a result", {
  files <- corpus_files()
  expect_gt(length(files), 0)

  for (f in files) {
    expect_true(corpus_header(f, "logic") %in%
                  c("QF_UF", "QF_LIA", "QF_LRA", "QF_UFLIA", "QF_UFLRA",
                    "QF_IDL", "QF_RDL", "QF_AX"),
                info = basename(f))
    expect_true(corpus_header(f, "expect") %in% c("sat", "unsat", "unknown"),
                info = basename(f))
  }
})

test_that("every corpus file gets the result it declares", {
  for (f in corpus_files()) {
    logic <- corpus_header(f, "logic")
    expected <- corpus_header(f, "expect")

    s <- smt_solver(logic)
    smt_assert(s, paste(readLines(f, warn = FALSE), collapse = "\n"))

    expect_identical(smt_check(s), expected, info = basename(f))
  }
})

test_that("a satisfiable corpus file yields a model that satisfies its bounds", {
  # Spot-check that "sat" is backed by real values rather than just a status:
  # lia-bounds.smt2 asserts 3 < x < 7 and y = 2x.
  f <- file.path(system.file("smt2", package = "zusmt"), "lia-bounds.smt2")
  skip_if(!file.exists(f), "corpus not installed")

  s <- smt_solver("QF_LIA")
  smt_assert(s, paste(readLines(f, warn = FALSE), collapse = "\n"))
  expect_identical(smt_check(s), "sat")

  m <- smt_model(s)
  expect_gt(m$x, 3)
  expect_lt(m$x, 7)
  expect_equal(m$y, 2 * m$x, ignore_attr = TRUE)
})
