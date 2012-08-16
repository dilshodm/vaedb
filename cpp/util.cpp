
using namespace std;

#include <cmath>
#include <boost/lexical_cast.hpp>

#include "util.h"
 
#define EPS 1e-10

string& str_replace(const string &search, const string &replace, string &subject) {
  string buffer;
  int sealeng = search.length();
  int strleng = subject.length();
  if (sealeng==0) return subject;
  for (int i = 0, j = 0; i < strleng; j = 0) {
    while (i+j<strleng && j<sealeng && subject[i+j]==search[j]) j++;
    if (j == sealeng) {
      buffer.append(replace);
      i += sealeng;
    } else {
      buffer.append(&subject[i++], 1);
    }
  }
  subject = buffer;
  return subject;
}

bool is_numeric_c(char c) {
  return isdigit(c) || c == '.';
}

ptrdiff_t token_end(string const &s, ptrdiff_t b, bool strmode) {
  while(b < s.size() && (is_numeric_c(s.at(b)) ^ strmode))
    ++b;
  return b;
}

int str_compare_nat(string const & s1, string const & s2) {
  ptrdiff_t e1 = 0;
  ptrdiff_t e2 = 0;
  ptrdiff_t comp_len = min(s1.size(), s2.size());
  ptrdiff_t b1, b2;
  bool mstr = false;

  while(max(e1, e2) < comp_len) {
    mstr = !mstr;
    b1 = e1;
    b2 = e2;
    e1 = token_end(s1, b1, mstr);
    e2 = token_end(s2, b2, mstr);

    string tok1 = s1.substr(b1, e1-b1);
    string tok2 = s2.substr(b2, e2-b2);

    if(!mstr) {
      try {
        double d1 = boost::lexical_cast<double>(tok1);
        double d2 = boost::lexical_cast<double>(tok2);

        if(abs(d1-d2) < EPS) continue;
        return d1 < d2 ? 1 : -1;
      } catch (boost::bad_lexical_cast &) {};

    }

    if(tok1 == tok2) continue;
    return tok1 < tok2 ? 1 : -1;
  }

  return 0; // equality
}

