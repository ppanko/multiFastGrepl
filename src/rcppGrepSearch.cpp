//' ---
//' title:  'grepl in parallel with Rcpp'
//' author: 'Pavel Panko'
//' date:   '03/31/2022'
//' ---

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

    std::vector<std::string> s = myList[i];

    out[i] = std::accumulate(s.begin(), s.end(), std::string(),[](std::string &x, std::string &y) {
	      return x.empty() ? y : x + "|" + y;});

  }

  return out;

}

Rcpp::DataFrame matrix_to_dataframe(Rcpp::IntegerMatrix& mat, Rcpp::CharacterVector names, Rcpp::CharacterVector id) {

  Rcpp::DataFrame df = Rcpp::DataFrame(mat);
  df.insert(0, id);

  names.insert(0, "id");
  df.names() = names;

  return Rcpp::DataFrame(df);
}

// [[Rcpp::depends(RcppParallel)]]
struct RegexSearch : public Worker
{
   // source objects
  const std::vector<std::string> stringVec;
  const std::vector<std::string> regexVec;

   // destination matrix
   RMatrix<int> output;

   // initialize with source and destination
  RegexSearch(const std::vector<std::string> stringVec, const std::vector<std::string> regexVec, Rcpp::IntegerMatrix output)
    : stringVec(stringVec), regexVec(regexVec), output(output) {}

  // Perform operation
  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t j = 0; j < regexVec.size(); j++) {
      boost::regex re(regexVec[j], boost::regex::icase);
      for (std::size_t i = begin; i < end; i++) {
	output(i,j) = boost::regex_search(stringVec[i], re);
      }
    }
  }
};

// [[Rcpp::export]]
Rcpp::DataFrame greplParallel(Rcpp::CharacterVector idVec, Rcpp::List stringList, Rcpp::List keyWordList) {

  Rcpp::IntegerMatrix output(idVec.length(), keyWordList.length());

   std::vector<std::string> stringVec = stringList.size() > 1 ? join_string_list(stringList) : Rcpp::as<std::vector<std::string>>(stringList[0]);

  std::vector<std::string> regexVec = concatenate_list_of_strings(keyWordList);

   RegexSearch RegexSearch(stringVec, regexVec, output);

   parallelFor(0, stringVec.size(), RegexSearch);

   return matrix_to_dataframe(output, keyWordList.names(), idVec);
}
