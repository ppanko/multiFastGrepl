checkNames <- function(namesVec) {
  namesDf <- data.frame(old_names = namesVec, stringsAsFactors = FALSE)
  if (any(grepl("\\.|\\s", namesVec))) {
    namesDf$new_names <- janitor::make_clean_names(namesVec)
  }
  namesDf
}

multiGrepl <- function(df, id, searchCols, patternMatches) {

  if (!is.data.frame(df)) {
    stop('"df" must be a data.frame')
  }

  if (length(id) < 1 | !is.character(id) | !id %in% names(df)) {
    stop('"id" must be a character vector specifying a name of column present in the data')
  }

  if (length(searchCols) < 1 | !is.character(searchCols) | !all(searchCols %in% names(df))) {
    stop('"searchCols" must be a character vector specifying a name of column present in the data')
  }

  if (!is.list(patternMatches) | !all(sapply(patternMatches, is.character))) {
    stop('"patternMatches" must be a list of character vectors')
  }

  if (is.null(names(patternMatches))) {
    warning("No ")
  } else {
    namesDf <- checkNames(names(patternMatches))
    if (ncol(namesDf) > 1) {
      names(patternMatches) <- namesDf$new_names
    }

  }

  greplDf <- greplParallel(
    idVec       = id,
    stringList  = as.list(df[searchCols]),
    keyWordList = patternMatches
  )

  if (ncol(namesDf) > 1) {
    names(greplDf)[which(names(greplDf) %in% namesDf$new_names)] <- namesDf$old_names
  }

  greplDf

}
