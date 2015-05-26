#include <cmath>
#include <boost/lexical_cast.hpp>

#include <vector>
#include "util.h"
 
#define EPS 1e-10

using namespace std;

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

typedef vector<string> strvec_t;

ptrdiff_t token_end(string const &s, ptrdiff_t b, bool strmode) {
  size_t l = s.size();
  while(b < l && (isdigit(s.at(b))^strmode)) ++b;
  return b;
}

strvec_t & tokenize_string(string const &s, strvec_t & tokens) {
  ptrdiff_t e = 0;
  ptrdiff_t b = 0;
  ptrdiff_t l = s.size();
  bool mstr = false;

  do {
    e = token_end(s, b=e, mstr = !mstr);
    string tok(s.substr(b, e-b));
    tokens.push_back(tok);
  } while (b < l);

  return tokens;
}

void merge_floats(strvec_t const & tokens_in, strvec_t & tokens_out) {
  ptrdiff_t tokcnt = tokens_in.size();
  ptrdiff_t i = 0;
  
  for(; i < tokcnt - 2; ++i) {
    string const & l = tokens_in[i];
    string const & c = tokens_in[i+1];
    string const & r = tokens_in[i+2];

    if(c == "." && l != "" && r != "") {
      tokens_out.push_back(l+c+r);
      i += 2;
    } else
      tokens_out.push_back(l);
  }

  for(;i < tokcnt; ++i) {
    tokens_out.push_back(tokens_in[i]);
  }
}

int str_compare_nat(string const &s1, string const & s2) {
  strvec_t tok1;
  strvec_t tok2;
  strvec_t comp_tok1;
  strvec_t comp_tok2;

  merge_floats(tokenize_string(s1, tok1), comp_tok1);
  merge_floats(tokenize_string(s2, tok2), comp_tok2);

  size_t l1 = comp_tok1.size();
  size_t l2 = comp_tok2.size();
  size_t comp_len = min(l1, l2);

  bool mstr=false;
  for(size_t i = 0; i < comp_len; ++i) {
    mstr = !mstr;
    string const & stok1 = comp_tok1[i]; 
    string const & stok2 = comp_tok2[i];
    size_t slen1 = stok1.size();
    size_t slen2 = stok2.size();

    if((slen1 == slen2) && (slen1 == 0))
        continue;

    if(!slen1 || ! slen2) {
      return slen1 < slen2 ? 1 : -1;
    }

    if(!mstr) {
      try {
        double d1 = boost::lexical_cast<double>(stok1);
        double d2 = boost::lexical_cast<double>(stok2);
        if(abs(d1-d2) < EPS) continue;
        return d1 < d2 ? 1 : -1;
      } catch (boost::bad_lexical_cast &) {}
    }

    if(stok1 == stok2) continue; 
    return stok1 < stok2 ? 1 : -1;

  }

  if(l1 == l2) return 0;
  return l1 < l2 ? 1 : -1;
}
