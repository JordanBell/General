#if defined(DISPLAY_RESULTS)
//#define RECONSTRUCT
#endif

struct CTraitHandler_Color : public CTraitHandler
{
	Mat m_tOut;

	CTraitHandler_Color()
	{
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName) override
	{
		// Create the sub-images and displays
		m_tOut = Mat(1, 4, CV_8UC3);
		const int k_iNumFrequencies = 4; // The top n colors we will consider as frequent
		
		vector<Vec3b> tMostFrequentColors;
		getMostFrequentColors(tMostFrequentColors, k_iNumFrequencies, s_tImage);

		for(unsigned int i = 0; i < k_iNumFrequencies; i++)
		{
			m_tOut.at<Vec3b>(0, i)= tMostFrequentColors[i];
		}

#ifdef DISPLAY_RESULTS
		const int k_iMagnification = 100;
		displayImage(i_sImgName + ": Most Frequent Colors", m_tOut, k_iMagnification, k_iMagnification); 
		waitKey(0);
#endif
		// Ensure directories for the result directory
		std::string sResultDirectory = s_sResultDirectory;
		ensureDirectory(sResultDirectory);

		// Ensure directory for this ITH's results
		sResultDirectory += "Color/";
		ensureDirectory(sResultDirectory);

		// Save the images to this ITH's directory
		std::string sOutFilepath;
		sOutFilepath = sResultDirectory + "mostFrequent.bmp";
		imwrite(sOutFilepath.c_str(), m_tOut);

		// Calculate the evaluation string
		{
			// TODO
		}
	}
};
