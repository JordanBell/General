using namespace cv;
using namespace std;

void onAssertTriggered(const string& msg)
{
	printf("%s\n", msg.c_str());
}

bool assertbr(const bool i_b, const string& msg)
{
	if(!i_b) onAssertTriggered(msg);
	return i_b;
}

// Ensure that a directory exists!
static int ensureDirectory(const string& i_sFilename)
{
#if defined(_WIN32)

	const bool bDirectoryNowExists = CreateDirectory(i_sFilename.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
	if(!bDirectoryNowExists)
	{
		assertbr(0, "Failed to create directory");
		return -1;
	}

	return 0;

#elif defined(__linux__)

	struct stat st;
	if(stat(i_sFilename.c_str(), &st) == -1)
	{
		printf("Failed to find dir: %s. Making one now.\n", i_sFilename.c_str());
		if(mkdir(i_sFilename.c_str(), 0777) != 0)
		{
			perror("Failed to create directory");
			return -1;
		}
	}

	return 0;
#else
	assertbr(0, "Unsupported platform");
	return -1;
#endif
}

void displayImage(const string& winname, const Mat& img, const float sx = MAGNIFY_FACTOR, const float sy = MAGNIFY_FACTOR)
{
	const int w = int(sx * img.cols);
	const int h = int(sy * img.rows);

	namedWindow(winname, WINDOW_NORMAL);
	resizeWindow(winname, w, h);
	imshow(winname, img);
}

bool operator<(const Vec3b& lhs, const Vec3b& rhs)
{
	return (lhs[0] + lhs[1] + lhs[2]) < (rhs[0] + rhs[1] + rhs[2]);
}

bool operator>(const Vec3b& lhs, const Vec3b& rhs)
{
	return true;
}

string toString(const Vec3b& v)
{
	string r_s = "";

	r_s += "(";
	r_s += std::to_string(v[0]);
	r_s += ", ";
	r_s += std::to_string(v[1]);
	r_s += ", ";
	r_s += std::to_string(v[2]);
	r_s += ")";

	return r_s;
}

template<typename A, typename B>
inline pair<B, A> flipPair(const pair<A, B>& i_pair)
{
	return std::pair<B,A>(i_pair.second, i_pair.first);
}

template<typename A, typename B>
multimap<B, A> flipMap(const map<A, B>& src)
{
	multimap<B,A> dst;
	transform(src.begin(), src.end(), inserter(dst, dst.begin()), flipPair<A, B>);
	return dst;
}

// Perhaps now deprecated
void setXYAsMeanOf(Mat& dst, int x, int y, const Mat& src)
{
	// Get the average pixel value in src
	Vec3f vAverage = 0;

	// How to get BGR value
	for(int i = 0; i < src.cols; i++)
	{
		for(int j = 0; j < src.rows; j++)
		{
			vAverage += src.at<Vec3b>(j, i);
		}
	}

	vAverage /= src.rows * src.cols;

	// Set the pixel at x y in dst as that average value
	dst.at<Vec3b>(y, x) = vAverage;
}

void getMostFrequentColors(vector<Vec3b>& o_vMostFrequent, int i_iNumToGet, const Mat& i_imgSrc)
{
	o_vMostFrequent.push_back(Vec3b(0, 0, 0));
	o_vMostFrequent.push_back(Vec3b(0, 0, 0));
	o_vMostFrequent.push_back(Vec3b(0, 0, 0));
	o_vMostFrequent.push_back(Vec3b(0, 0, 0));
	return;

	//// Track how many times each color has been encountered
	//map<Vec3b, int> mColorCounters;

	//// Build the frequency map
	//for(int i = 0; i < i_imgSrc.cols; i++)
	//{
	//	for(int j = 0; j < i_imgSrc.rows; j++)
	//	{
	//		Vec3b vColor = i_imgSrc.at<Vec3b>(j, i);
	//		pair<map<Vec3b, int>::iterator, bool> r_emplace = mColorCounters.emplace(vColor);
	//		if(!r_emplace.second)
	//		{
	//			// If there already exists an object, increase the counter
	//			r_emplace.first->second++;
	//		}
	//	}
	//}

	//multimap<int, Vec3b> mCounterColors = flipMap(mColorCounters);

	//int i = 0;
	//multimap<int, Vec3b>::iterator it = mCounterColors.begin();
	//while(i < i_iNumToGet && it != mCounterColors.end())
	//{
	//	o_vMostFrequent.push_back(it->second); // Add this value to the vector of Most Frequent colors

	//	// Increase counter and iterator
	//	i++;
	//	it++;
	//}
}