//' ---
//' title:  'Extract first string match with Rcpp'
//' author: 'Pavel Panko'
//' date:   '05/15/2022'
//' ---

#include "helpers.h"
#include <Rcpp.h>
#include <boost/regex.hpp>


// [[Rcpp::export]]
Rcpp::DataFrame getRegexMatches(Rcpp::CharacterMatrix stringMat, Rcpp::CharacterVector id, Rcpp::List patternList, bool caseSensitive = true) {

  //
  int n = stringMat.nrow();
  int k = patternList.size();

  boost::regbase::flag_type_ flag = getCaseFlag(caseSensitive);

  Rcpp::CharacterMatrix returnMat(n, k);

  //
  for (int j = 0; j < k; j++) {

    //
    std::string pattern = collapseStringVec(patternList[j], "|");
    boost::regex re(pattern, flag);

    //
    for (int i = 0; i < n; i++) {

      //
      std::string matchString = collapseStringVec(stringMat(i, Rcpp::_ ), " ");
      boost::smatch match;

      //
      if (boost::regex_search(matchString, match, re)) {

	returnMat(i,j) = std::string(match[0].first, match[0].second);

      }

    }

  }

  return matrix_to_dataframe(returnMat, patternList.names(), id);

}
