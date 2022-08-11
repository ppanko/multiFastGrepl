//' ---
//' title:  'grepl in parallel with Rcpp'
//' author: 'Pavel Panko'
//' date:   '03/31/2022'
//' ---

#include "helpers.h"
#include <Rcpp.h>
#include <boost/regex.hpp>
#include <RcppParallel.h>

using namespace RcppParallel;

std::vector<std::string> join_string_list(const Rcpp::List& stringList) {

  int k = stringList.size();
  int n = Rcpp::as<Rcpp::CharacterVector>(stringList[0]).size();

  std::vector<std::string> out(n);

  for (int i=0; i<n; i++) {

    for (int j=0; j<k; j++) {

      out[i] += " " + Rcpp::as<Rcpp::CharacterVector>(stringList[j])[i];

    }
  }

  return out;

}

std::vector<std::string> concatenate_list_of_strings(Rcpp::List myList) {

  std::vector<std::string> out(myList.size());

  for (int i=0; i<myList.size(); i++) {

    out[i] = collapseStringVec(myList[i], "|");

  }

  return out;

}

// [[Rcpp::depends(RcppParallel)]]
struct RegexSearch : public Worker
{
  // source objects
  const std::vector<std::string> stringVec;
  const std::vector<std::string> regexVec;

  // destination matrix
  RMatrix<int> output;

  // case sensitive flag
  boost::regbase::flag_type_ flag;

  // initialize with source and destination
  RegexSearch(const std::vector<std::string> stringVec, const std::vector<std::string> regexVec, Rcpp::IntegerMatrix output, boost::regbase::flag_type_ flag)
    : stringVec(stringVec), regexVec(regexVec), output(output) {}

  // Perform operation
  void operator()(std::size_t begin, std::size_t end) {

    for (std::size_t j = 0; j < regexVec.size(); j++) {

      boost::regex re(regexVec[j], flag);

      for (std::size_t i = begin; i < end; i++) {

	output(i,j) = boost::regex_search(stringVec[i], re);
      }

    }

  }

};

// [[Rcpp::export]]
Rcpp::DataFrame greplParallel(Rcpp::CharacterVector idVec, Rcpp::List stringList, Rcpp::List keyWordList, bool caseSensitive = false) {

  Rcpp::IntegerMatrix output(idVec.length(), keyWordList.length());

  std::vector<std::string> stringVec = stringList.size() > 1 ? join_string_list(stringList) : Rcpp::as<std::vector<std::string>>(stringList[0]);

  std::vector<std::string> regexVec = concatenate_list_of_strings(keyWordList);

  boost::regbase::flag_type_ flag = getCaseFlag(caseSensitive);

  RegexSearch RegexSearch(stringVec, regexVec, output, flag);

  parallelFor(0, stringVec.size(), RegexSearch);

  return matrix_to_dataframe(output, keyWordList.names(), idVec);

}
