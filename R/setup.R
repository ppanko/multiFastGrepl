install.packages(c("stringi", "devtools", "roxygen2", "rcmdcheck", "pkgload", "remotes", "rhub", "revdepcheck"))

devtools::install_github("r-lib/devtools")
devtools::install_github("r-lib/usethis")

options(
  usethis.full_name = "Pavel Panko",
  usethis.description = list(
    `Authors@R` = 'person("Pavel", "Panko", email = "pavelppanko@gmail.com", role = c("aut", "cre")',
    Version = "0.0.0.9000"
  ),
  usethis.protocol  = "ssh"
)

RcppParallel::RcppParallel.package.skeleton(".")
usethis::use_gpl_license()
usethis::use_version("0.0.0.9000")

## usethis::create_package(".")
##
## usethis::use_build_ignore()
## usethis::use_rcpp()

usethis::use_build_ignore(
  "^.*\\.Rproj$         # Designates the directory as an RStudio Project
^\\.Rproj\\.user$     # Used by RStudio for temporary files
^README\\.Rmd$       # An Rmd file used to generate README.md
^LICENSE\\.md$       # Full text of the license
^cran-comments\\.md$ # Comments for CRAN submission
^data-raw$          # Code used to create data included in the package
^pkgdown$           # Resources used for the package website
^_pkgdown\\.yml$     # Configuration info for the package website
^\\.github$          # Contributing guidelines, CoC, issue templates, etc.
R/setup.R"
)



Rcpp::compileAttributes()

devtools::check_win_devel()
rhub::check_for_cran()
## revdepcheck::revdep_check(num_workers = 4)


library(Rcpp)
library(RcppParallel)

sourceCpp("src/rcppGrepSearch.cpp")

df <- data.frame(
  id       = 1:2,
  title    = c("1", "2"),
  abstract = c("a", "b")
)

regexList <- list(
  a = c("1", "2", "3"),
  b = c("4", "5", "6")
)

regexDemo(df, regexList)
