using namespace std;
using namespace cv;

struct CTraitHandler
{
	static vector<CTraitHandler*> s_lpAllHandlers;
	static cv::Mat s_tImage;
	static string s_sResultDirectory;

	string m_sResultDirectory;

	static Size s_tInputSizeLimit;

	int m_iInstanceIndex;

	CTraitHandler()
		//: m_iInstanceIndex(s_lpAllHandlers.size())
	{
		// Add this to the list of all instances
		//s_lpAllHandlers.push_back(this);
	}

	~CTraitHandler()
	{
		// Remove this from the list of all instances
		//s_lpAllHandlers.erase(s_lpAllHandlers.begin() + m_iInstanceIndex);
	}

	static void makeInputImageValid(Mat& io_tImage)
	{
		// 1. Resize
		float fWidth = io_tImage.cols;
		float fHeight = io_tImage.rows;

		if(fWidth > s_tInputSizeLimit.width)
		{
			const float fAdjustment = (float)s_tInputSizeLimit.width / (float)fWidth;
			fWidth *= fAdjustment;
			fHeight *= fAdjustment;
		}
		
		if (fHeight > s_tInputSizeLimit.height)
		{
			const float fAdjustment = (float)s_tInputSizeLimit.height / (float)fHeight;
			fWidth *= fAdjustment;
			fHeight *= fAdjustment;
		}

		Mat tResized;
		Size tNewSize((int)fWidth, (int)fHeight);
		resize(io_tImage, tResized, tNewSize); 
		io_tImage = tResized;

		// 2. Posterize
	}

	static int setProcessingImage(string i_sFilepath)
	{
		// Set the static processing image
		i_sFilepath = TEST_IMAGES_DIR + i_sFilepath;

		s_tImage = imread(i_sFilepath, CV_LOAD_IMAGE_COLOR);
		if(!s_tImage.data)
		{
			// Failed to load image
			return -1;
		}

		makeInputImageValid(s_tImage);

		const string sImageFilename = i_sFilepath.substr(i_sFilepath.rfind('/') + 1);
		i_sFilepath = sImageFilename.substr(0, sImageFilename.rfind('.'));

		// Make a directory for this in the results directory
		s_sResultDirectory = "Results/" + i_sFilepath + "/";

		ENSURE_DIR(s_sResultDirectory);
		imwrite(s_sResultDirectory + "Original.bmp", s_tImage);

		return 0;
	}

	// Returns a string result for the subject image
	virtual void evaluate(string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) = 0;
};

string CTraitHandler::s_sResultDirectory = "";
cv::Mat CTraitHandler::s_tImage;
Size CTraitHandler::s_tInputSizeLimit = Size(64, 64);