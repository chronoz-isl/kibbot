#include "cv.h"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/make_shared.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

std::string to_string(cv::Mat& M);
std::string _base64_encode(const std::string &s);
std::string string_compress_encode(const std::string &data);
void reduceDepth(cv::Mat* M, uchar bit_flags);
bool applyPriority(boost::thread *pThread, uint8_t priority);