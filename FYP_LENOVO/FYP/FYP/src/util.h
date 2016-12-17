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

#if defined(DISPLAY_RESULTS)
void displayImage(const string& winname, const Mat& img, const float sx = MAGNIFY_FACTOR, const float sy = MAGNIFY_FACTOR)
{
	const int w = int(sx * img.cols);
	const int h = int(sy * img.rows);

	namedWindow(winname, WINDOW_NORMAL);
	resizeWindow(winname, w, h);
	imshow(winname, img);
}
#endif