#ifndef _VAE_THRIFT_S3_H_
#define _VAE_THRIFT_S3_H_

#include <string>
#include <boost/filesystem.hpp>

bool initialize_s3(std::string const & access_key,
                   std::string const & secret_key,
                   std::string const & cache_path);

void shutdown_s3();

boost::filesystem::path object_cache_path (std::string const & objectname);

std::string read_cache(std::string const & filename);
std::string read_s3(std::string const & filename);

#endif // #ifndef _VAE_THRIFT_S3_H_
