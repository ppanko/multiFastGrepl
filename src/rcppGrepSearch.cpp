//' ---
//' title:  'grepl in parallel with Rcpp'
//' author: 'Pavel Panko'
//' date:   '03/31/2022'
//' ---

#include <Rcpp.h>
#include <boost/regex.hpp>
#include <RcppParallel.h>
#include <boost/xpressive/xpressive.hpp>

using namespace RcppParallel;

std::vector<std::string> join_string_vectors(const std::vector<std::string>& v1, const std::vector<std::string>& v2) {

  int n = v1.size();
  std::vector<std::string> out(n);

  for (int i=0; i<n; i++) {
    out[i] = v1[i] + " " + v2[i];
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
    for (std::size_t i = begin; i < end; i++) {
      for (std::size_t j = 0; j < regexVec.size(); j++) {
	boost::regex re(regexVec[j], boost::regex::icase);
	output(i,j) = boost::regex_search(stringVec[i], re);
      }
    }
  }
};

// [[Rcpp::export]]
Rcpp::DataFrame greplParallel(Rcpp::DataFrame df, Rcpp::List keyWordList) {

  Rcpp::IntegerMatrix output(df.nrows(), keyWordList.length());

  std::vector<std::string> stringVec = join_string_vectors(df["title"], df["abstract"]);
  std::vector<std::string> regexVec = concatenate_list_of_strings(keyWordList);

  RegexSearch RegexSearch(stringVec, regexVec, output);

  parallelFor(0, stringVec.size(), RegexSearch);

  return matrix_to_dataframe(output, keyWordList.names(), df["id"]);
}
