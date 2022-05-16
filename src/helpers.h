#ifndef HELPERS_H
#define HELPERS_H

#include <Rcpp.h>

template <typename T>
Rcpp::DataFrame matrix_to_dataframe(T& mat, Rcpp::CharacterVector names, Rcpp::CharacterVector id) {

  Rcpp::DataFrame df = Rcpp::DataFrame(mat);
  df.insert(0, id);

  names.insert(0, "id");
  df.names() = names;

  return Rcpp::DataFrame(df);
}

std::string collapseStringVec(Rcpp::CharacterVector toCollapseVec, std::string sep) {

  std::vector<std::string> s = Rcpp::as<std::vector<std::string>>(toCollapseVec);

  return std::accumulate(s.begin(), s.end(), std::string(),[&sep](std::string &x, std::string &y) {
    return x.empty() ? y : x + sep + y;});

}

#endif
