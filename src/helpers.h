#ifndef HELPERS_H
#define HELPERS_H

#include <Rcpp.h>
#include <boost/regex.hpp>

template <typename T>
inline Rcpp::DataFrame matrix_to_dataframe(T& mat, Rcpp::CharacterVector names, Rcpp::CharacterVector id) {

  Rcpp::DataFrame df = Rcpp::DataFrame(mat);
  df.insert(0, id);

  names.insert(0, "id");
  df.names() = names;

  return Rcpp::DataFrame(df);
}

inline std::string collapseStringVec(Rcpp::CharacterVector toCollapseVec, const char* sep) {

  std::vector<std::string> s = Rcpp::as<std::vector<std::string>>(toCollapseVec);

  return std::accumulate(s.begin(), s.end(), std::string(),[&sep](std::string &x, std::string &y) {
    return x.empty() ? y : x + sep + y;});

}

inline boost::regbase::flag_type_ getCaseFlag(bool caseSensitive) {

  boost::regbase::flag_type_ flag;

  if (caseSensitive) {

    flag = boost::regex::optimize;

  } else {

    flag = boost::regex::icase;

  }

  return flag;

}

#endif
