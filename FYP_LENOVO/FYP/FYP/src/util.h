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

template <typename T>
void clamp(T& value, const T& min, const T& max)
{
	if(value < min)
	{
		value = min;
	}
	else if(value > max)
	{
		value = max;
	}
}

float lerp(float a, float b, float mmin, float mmax, float m)
{
	// TODO
	float factor = (m - mmin) / (mmax - mmin);
	return a + (b - a) * factor;
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

inline bool operator<(const Vec3b& lhs, const Vec3b& rhs)
{
	return (lhs[0] + lhs[1] + lhs[2]) < (rhs[0] + rhs[1] + rhs[2]);
}

inline bool operator>(const Vec3b& lhs, const Vec3b& rhs)
{
	return (lhs[0] + lhs[1] + lhs[2]) > (rhs[0] + rhs[1] + rhs[2]);
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

float getRadialWeight(float i_fDistanceFromCenter, float i_fDiameter)
{
	float fPercentageDistanceFromCentre = (i_fDistanceFromCenter / (i_fDiameter / 2.f));
	float fWeight = 1.f - pow(fPercentageDistanceFromCentre, RADIAL_WEIGHTING_EXPONENT);
	clamp(fWeight, 0.f, 1.f);
	fWeight *= 1.f - RADIAL_WEIGHTING_MIN;
	fWeight += RADIAL_WEIGHTING_MIN;
	return fWeight;
}

// i_iChannelDivisions: The number of fractions of 255 we will snap each channel to
void snapColorsHLS(Vec3b& io_vColor, const int i_iChannelDivisions)
{
	Vec3b tStart = io_vColor;
	float H = io_vColor[0];
	float L = io_vColor[1];
	float S = io_vColor[2];

	// Determine if we're black/grey/white
	if(S < 50)
	{
		// Definitely desaturated
		if(L < 50)
		{
			// Set to black
			io_vColor[0] = 0;
			io_vColor[1] = 0;
			io_vColor[2] = 0;
			return;
		}
		else if (L > 200)
		{
			// Set to white
			io_vColor[0] = 0;
			io_vColor[1] = 255;
			io_vColor[2] = 0;
			return;
		}
		else
		{
			// Set to grey
			io_vColor[0] = 0;
			io_vColor[1] = 127;
			io_vColor[2] = 0;
			return;
		}
	}

	// Saturated, but may still be black/white 45S 15L
	if(L < 50) // Very dark
	{
		// Is black if below S threshold
		float fSThreshold = lerp(25, 255, 50, 18, L);
		if(S < fSThreshold)
		{
			// Set to black
			io_vColor[0] = 0;
			io_vColor[1] = 0;
			io_vColor[2] = 0;
			return;
		}
	}
	
	if(L > 205) // Very light
	{
		// Is white if below S threshold
		float fSThreshold = lerp(25, 255, 205, 237, L);
		if(S < fSThreshold)
		{
			// Set to white
			io_vColor[0] = 0;
			io_vColor[1] = 255;
			io_vColor[2] = 0;
			return;
		}
	}
	
	// Normal color; snap normally
	{
		// Hue
		io_vColor[0] = io_vColor[0] - (char)(io_vColor[0] % i_iChannelDivisions);

		// Lightness
		io_vColor[1] = 127;

		// Saturation
		io_vColor[2] = 255;
	}
}

void cvtColor(Vec3b& io_tColor, const int code)
{
	Mat tImgColor(1, 1, CV_8UC3);
	tImgColor.at<Vec3b>(0, 0) = io_tColor;
	cvtColor(tImgColor, tImgColor, code);
	io_tColor = tImgColor.at<Vec3b>(0, 0);
}

void colorSnap(Mat& io_tImage)
{
	cvtColor(io_tImage, io_tImage, CV_BGR2HLS);
	for_each(io_tImage.begin<Vec3b>(), io_tImage.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, CS_SEGMENT_SIZE); });
	cvtColor(io_tImage, io_tImage, CV_HLS2BGR);
}