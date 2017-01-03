using namespace std;
using namespace cv;

struct CTraitHandler
{
	static vector<CTraitHandler*> s_lpAllHandlers;
	static Mat s_tImage;
#ifdef SAVE_INTO_LOCAL_FOLDER
	static string s_sResultDirectory;
#endif

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
		{
			/*Mat t0 = io_tImage.clone();
			cvtColor(t0, t0, CV_BGR2HLS);
			for_each(t0.begin<Vec3b>(), t0.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 32); });
			cvtColor(t0, t0, CV_HLS2BGR);
			Mat t1 = io_tImage.clone();
			cvtColor(t1, t1, CV_BGR2HLS);
			for_each(t1.begin<Vec3b>(), t1.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 32); });
			cvtColor(t1, t1, CV_HLS2BGR);
			Mat t2 = io_tImage.clone();
			cvtColor(t2, t2, CV_BGR2HLS);
			for_each(t2.begin<Vec3b>(), t2.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 16); });
			cvtColor(t2, t2, CV_HLS2BGR);
			Mat t3 = io_tImage.clone();
			cvtColor(t3, t3, CV_BGR2HLS);
			for_each(t3.begin<Vec3b>(), t3.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 8); });
			cvtColor(t3, t3, CV_HLS2BGR);
			Mat t4 = io_tImage.clone();
			cvtColor(t4, t4, CV_BGR2HLS);
			for_each(t4.begin<Vec3b>(), t4.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 4); });
			cvtColor(t4, t4, CV_HLS2BGR);
			Mat t5 = io_tImage.clone();
			cvtColor(t5, t5, CV_BGR2HLS);
			for_each(t5.begin<Vec3b>(), t5.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 2); });
			cvtColor(t5, t5, CV_HLS2BGR);

			displayImage("64", t0, 4, 4);
			displayImage("32", t1, 4, 4);
			displayImage("16", t2, 4, 4);
			displayImage("8", t3, 4, 4);
			displayImage("4", t4, 4, 4);
			displayImage("2", t5, 4, 4);*/
		}
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
#else
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

		// Clear the cached normalised and colorsnapped versions of previously processed images
		if(s_pImageColorSnapped)
		{
			delete s_pImageColorSnapped;
			s_pImageColorSnapped = NULL;
		}

		const string sImageFilename = i_sFilepath.substr(i_sFilepath.rfind('/') + 1);
		i_sFilepath = sImageFilename.substr(0, sImageFilename.rfind('.'));

		// Make sure the result directory exists
		ENSURE_DIR("Results/");

#ifdef SAVE_INTO_LOCAL_FOLDER
		// Create a folder containing all processed version of a particular image.
		s_sResultDirectory = "Results/" + i_sFilepath + "/";
		ENSURE_DIR(s_sResultDirectory);
		imwrite(s_sResultDirectory + "Original.bmp", s_tImage);
#endif

		return 0;
	}

	// Returns a string result for the subject image
	virtual void evaluate(string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) = 0;

	void saveTo(string i_sFolderName, string i_sImgName, const Mat& i_tImageToSave)
	{
		ensureDirectory("Results/123_Categories/");
		string sCategoryDirectory = "Results/123_Categories/" + i_sFolderName + "/";
		ensureDirectory(sCategoryDirectory);
		imwrite(sCategoryDirectory + i_sImgName + ".bmp", i_tImageToSave);

#ifdef SAVE_INTO_LOCAL_FOLDER
		imwrite(s_sResultDirectory + "/" + i_sFolderName + ".bmp", i_tImageToSave);
#endif
	}

	static const Mat& getColorSnapped()
	{
		if(s_pImageColorSnapped == NULL)
		{
			s_pImageColorSnapped = new Mat(s_tImage.clone());

			// Perform color snap
			cvtColor(*s_pImageColorSnapped, *s_pImageColorSnapped, CV_BGR2HLS);
			for_each(s_pImageColorSnapped->begin<Vec3b>(), s_pImageColorSnapped->end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, CS_SEGMENT_SIZE); });
			cvtColor(*s_pImageColorSnapped, *s_pImageColorSnapped, CV_HLS2BGR);

#if defined(CS_BLUR_SIZE)
			// Perform median blur to remove unimportant details
			medianBlur(*s_pImageColorSnapped, *s_pImageColorSnapped, CS_BLUR_SIZE);  
#endif 
		}

		return *s_pImageColorSnapped;
	}

private:

	static Mat* s_pImageColorSnapped;
};

#ifdef SAVE_INTO_LOCAL_FOLDER
string CTraitHandler::s_sResultDirectory = "";
#endif
Mat CTraitHandler::s_tImage;
Mat* CTraitHandler::s_pImageColorSnapped = NULL;
Size CTraitHandler::s_tInputSizeLimit = Size(64, 64);