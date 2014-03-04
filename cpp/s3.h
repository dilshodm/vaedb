#ifndef _VAE_THRIFT_S3_H_
#define _VAE_THRIFT_S3_H_

#include <string>

bool initialize_s3(std::string const & access_key, std::string const & secret_key);
void shutdown_s3();
std::string read_s3(std::string const & filename);

#endif // #ifndef _VAE_THRIFT_S3_H_
