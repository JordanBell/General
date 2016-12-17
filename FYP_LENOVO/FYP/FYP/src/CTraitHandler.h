using namespace std;
using namespace cv;

struct CTraitHandler
{
	static vector<CTraitHandler*> s_lpAllHandlers;
	static cv::Mat s_tImage;
	static string s_sResultDirectory;

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

		const string sImageFilename = i_sFilepath.substr(i_sFilepath.rfind('/') + 1);
		i_sFilepath = sImageFilename.substr(0, sImageFilename.rfind('.'));

		// Make a directory for this in the results directory
		s_sResultDirectory = "Results/" + i_sFilepath + "/";

		ENSURE_DIR(s_sResultDirectory);

		return 0;
	}

	// Returns a string result for the subject image
	virtual void evaluate(string& o_sID, const std::string& i_sImgName) = 0;
};

string CTraitHandler::s_sResultDirectory = "";
cv::Mat CTraitHandler::s_tImage;
