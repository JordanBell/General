using namespace std;
using namespace cv;

struct CTraitHandler
{
	static vector<CTraitHandler*> s_lpAllHandlers;
	static Mat s_tImage;
	static Mat s_tImageNormalised;
	static Mat s_tImageColorSnapped;
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

		// 2. Posterize (TODO)
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

#ifdef RAW_INPUT
		makeInputImageValid(s_tImage);
#endif

#ifdef COPY_RAW_TO_QUICK
		string sFilepathNew = i_sFilepath.substr(i_sFilepath.find('/') + 1);
		sFilepathNew = TEST_IMAGES_DIR_QUICK + sFilepathNew;
		string sDir = sFilepathNew.substr(0, sFilepathNew.rfind('/') + 1);
		ENSURE_DIR(sFilepathNew.substr(0, sFilepathNew.find('/') + 1));
		ENSURE_DIR(sFilepathNew.substr(0, sFilepathNew.rfind('/') + 1)); // Note: Assumes no folders deeper than two levels
		imwrite(sFilepathNew, s_tImage);
#endif

		// Save copies of the image in certain forms that may be used by many sub-class trait handlers
		s_tImageNormalised = s_tImage.clone();
		for_each(s_tImageNormalised.begin<Vec3b>(), s_tImageNormalised.end<Vec3b>(), [] (Vec3b& io_vColor) { normalise(io_vColor); });

		s_tImageColorSnapped = s_tImage.clone();
		for_each(s_tImageColorSnapped.begin<Vec3b>(), s_tImageColorSnapped.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColors(io_vColor, 1); });

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
cv::Mat CTraitHandler::s_tImageNormalised;
cv::Mat CTraitHandler::s_tImageColorSnapped;
Size CTraitHandler::s_tInputSizeLimit = Size(64, 64);