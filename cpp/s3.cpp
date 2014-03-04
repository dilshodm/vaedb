#include "s3.h"

#include "libs3.h"
#include <memory.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/scoped_array.hpp>

using std::string;

std::string _access_key;
std::string _secret_key;
std::string _bucket("private-data.vaeplatform.com");

bool initialize_s3(string const & access_key, string const & secret_key) {
  _access_key = access_key;
  _secret_key = secret_key;
  return S3_initialize("s3", S3_INIT_ALL, _bucket.c_str()) == S3StatusOK;
}

void shutdown_s3() {
  S3_deinitialize();
}

S3Status responsePropertiesCallback(const S3ResponseProperties *properties,
                		     void *callbackData) {
  return S3StatusOK;
}

void responseCompleteCallback(
       S3Status status,
       const S3ErrorDetails *error,
       void *callbackData) {
  if (error && error->message)
    std::cout << error->message << std::endl;
  if (error && error->resource)
    std::cout << error->resource << std::endl;
  if (error && error->furtherDetails)
    std::cout << error->furtherDetails << std::endl;
  return;
}

S3ResponseHandler responseHandler = {
  &responsePropertiesCallback,
  &responseCompleteCallback
};

S3Status getObjectDataCallback(int bufferSize, const char *buffer, void *callbackData)
{
  boost::scoped_array<char> cbfr(new char[bufferSize+1]);
  memcpy(&cbfr[0], buffer, bufferSize);
  cbfr[bufferSize] = 0;

  std::stringstream * p_ss = static_cast<std::stringstream *>(callbackData);
  *p_ss << cbfr.get();
  return S3StatusOK;
}

S3GetObjectHandler getObjectHandler = {
  responseHandler,
  &getObjectDataCallback
};


string read_s3(string const & filename) {
  S3BucketContext bucketContext = {
    "s3.amazonaws.com",
    _bucket.c_str(),
    S3ProtocolHTTP,
    S3UriStylePath,
    _access_key.c_str(),
    _secret_key.c_str()
  };

  std::stringstream ss;

  S3_get_object(&bucketContext, filename.c_str(),
                   NULL, 0, 0, NULL, &getObjectHandler, &ss);

  return ss.str();
}
