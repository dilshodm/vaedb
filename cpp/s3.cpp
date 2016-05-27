
#include <memory.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <libs3.h>

#include "logger.h"
#include "s3.h"

using std::ifstream;
using std::fstream;
using std::string;
using std::stringstream;

namespace boostfs = boost::filesystem;

struct transfer {
  transfer(string const & objectname)
    : failed(false),
      cachename(object_cache_path(objectname)),
      tempname(boostfs::unique_path("/tmp/vaedb-%%%%-%%%%-%%%%-%%%%")),
      error_message("unknown error."),
      objectname(objectname) {

    tempfs.open(tempname.c_str(), fstream::out);
    if (!tempfs.is_open()) {
        L(warning) << "Unable to create temporary file " << tempname << " for object " << objectname;
    }
  }

  string get_error() const {
    stringstream sb;
    sb << "Error reading " << objectname;
    sb << ": " << error_message;
    return sb.str();
  }

  bool commit_buffer(int bfrsize, char const * bfr) {
    string cbfr(bfr, bfrsize);

    ss << cbfr;
    if(tempfs.is_open())
        tempfs << cbfr;

    return true;
  }

  void finalize() {
    if(!tempfs.is_open())
        return;

    tempfs.close();

    if(failed) {
      boostfs::remove(tempname);
      return;
    }

    boostfs::remove(cachename);

    try {
      boostfs::rename(tempname, cachename);
    } catch (boostfs::filesystem_error const & e) {
      L(error) << "Unable to move cache for object " << objectname << " into place.";
    }
  }

  string get_value() {
    if(!failed)
      return ss.str();

    if(boostfs::exists(cachename)) {
      L(warning) << "Pulling object from cache: " + objectname;
      return read_cache(objectname);
    }

    L(error) << "Fetch of object failed and cache unavailable: " + objectname;
    return "";
  }

  bool failed;
  fstream tempfs;
  boostfs::path cachename;
  boostfs::path tempname;
  stringstream ss;
  string error_message;
  string objectname;
};

string _access_key;
string _secret_key;
string _bucket;
boostfs::path _cache_path;

bool initialize_s3(string const & access_key,
                   string const & secret_key,
                   string const & bucket,
                   string const & cache_path) {
  _access_key = access_key;
  _secret_key = secret_key;
  _bucket = bucket;
  _cache_path = cache_path;

  return S3_initialize("s3", S3_INIT_ALL, _bucket.c_str()) == S3StatusOK;
}

void shutdown_s3() {
  S3_deinitialize();
}

S3Status responsePropertiesCallback(const S3ResponseProperties *properties,
                		     void *data) {
  return S3StatusOK;
}

void responseCompleteCallback(
       S3Status status,
       const S3ErrorDetails *s3error,
       void *data) {
  transfer * ts = static_cast<transfer *>(data);
  if(status != S3StatusOK)
    ts->failed = true;

  if (s3error && s3error->message) {
    ts->error_message = s3error->message;
    L(error) << ts->get_error();
  }

  return;
}

S3Status readObjectDataCallback(int bufferSize, const char *buffer, void *data)
{
  transfer * p_ts = static_cast<transfer *>(data);
  return p_ts->commit_buffer(bufferSize, buffer) ?
      S3StatusOK : S3StatusAbortedByCallback;
}

boostfs::path object_cache_path(string const & objectname) {
  return (_cache_path/boostfs::path(objectname));
}

string read_cache(string const & objectname) {
  return static_cast<stringstream const&>(
    stringstream() << ifstream(object_cache_path(objectname).c_str()).rdbuf()
  ).str();
}

string read_s3(string const & objectname) {
  S3ResponseHandler responseHandler = {
    &responsePropertiesCallback,
    &responseCompleteCallback
  };

  S3GetObjectHandler readObjectHandler = {
    responseHandler,
    &readObjectDataCallback
  };

  S3BucketContext bucketContext = {
    "s3.amazonaws.com",
    _bucket.c_str(),
    S3ProtocolHTTP,
    S3UriStylePath,
    _access_key.c_str(),
    _secret_key.c_str()
  };

  transfer ts(objectname);

  S3_get_object(&bucketContext, objectname.c_str(),
                   NULL, 0, 0, NULL, &readObjectHandler, &ts);

  ts.finalize();
  return ts.get_value();
}
