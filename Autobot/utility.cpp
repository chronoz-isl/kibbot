#include "utility.h"

std::string to_string(cv::Mat& M) {
	int rows = M.rows, cols = M.cols, pixels = rows * cols * 3;
	std::string out = std::string((char*) M.data, pixels);
	return out;
}

typedef
boost::archive::iterators::base64_from_binary<
	boost::archive::iterators::transform_width<const char *, 6, 8>
> base64_text;

std::string _base64_encode(const std::string &s) {

	std::stringstream os;

	std::copy(
		base64_text(s.c_str()),
		base64_text(s.c_str() + s.size()),
		boost::archive::iterators::ostream_iterator<char>(os)
	);

	if (s.length() % 3 == 1) {
		os << "==";
	}
	else if (s.length() % 3 == 2) {
		os << "=";
	}

	return os.str();
}

std::string string_compress_encode(const std::string &data)
{
	std::stringstream compressed;
	std::stringstream original;
	original << data;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
	out.push(boost::iostreams::zlib_compressor());
	out.push(original);
	boost::iostreams::copy(out, compressed);

	/**need to encode here **/
	std::string compressed_encoded = _base64_encode(compressed.str());
	//std::string compressed_encoded = compressed.str();

	return compressed_encoded;
}
