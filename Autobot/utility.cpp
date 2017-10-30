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


/* In-place reduceDepth*/
void reduceDepth(cv::Mat* M, uchar bit_flags) {
	if (!M) {
		return;
	}
	size_t size = M->total() * M->elemSize();
	uchar* ptr = M->data;
	for (int i = 0; i < size; i++) {
		(*(ptr++)) &= bit_flags;
	}
}

bool applyPriority(boost::thread *pThread, uint8_t priority) {
#ifdef WIN32
	if (!pThread)
		return false;

	BOOL res;
	HANDLE th = pThread->native_handle();

	switch (priority)
	{
	case 5: res = SetPriorityClass(th, REALTIME_PRIORITY_CLASS);      break;
	case 4: res = SetPriorityClass(th, HIGH_PRIORITY_CLASS);          break;
	case 3: res = SetPriorityClass(th, ABOVE_NORMAL_PRIORITY_CLASS);  break;
	case 2: res = SetPriorityClass(th, NORMAL_PRIORITY_CLASS);            break;
	case 1: res = SetPriorityClass(th, BELOW_NORMAL_PRIORITY_CLASS);  break;
	case 0: res = SetPriorityClass(th, IDLE_PRIORITY_CLASS);          break;
	default: return false;
	}

	return res;

#endif
}