using namespace std;
using namespace cv;

struct CTraitHandler_ColorFrequency : public CTraitHandler
{
	vector<pair<float, Vec3b>> m_vColors;
	
	CTraitHandler_ColorFrequency()
		: CTraitHandler()
	{
		m_bIsFloatBased = false;
	}

	Mat getFreqGraph()
	{
		const float fGraphHeight = 400.f;
		const float fColumnWidth = 32;
		Mat tFreqGraph = Mat((int)fGraphHeight, m_vColors.size() * fColumnWidth, CV_8UC3);

		for(unsigned int i = 0; i < m_vColors.size(); ++i)
		{
			pair<float, Vec3b>& tPair = m_vColors[i];

			line(tFreqGraph, 
				Point(i * fColumnWidth + fColumnWidth / 2, 0),
				Point(i * fColumnWidth + fColumnWidth / 2, tPair.first * fGraphHeight),
				cvScalar(tPair.second[0], tPair.second[1], tPair.second[2]), 
				fColumnWidth / 2, 4);
		}

		return tFreqGraph;
	}

	int numColorsAboveFrequency(const float i_fThreshold) const
	{
		for(unsigned int i = 0; i < m_vColors.size(); ++i)
		{
			if(m_vColors[i].first < i_fThreshold)
			{
				return 0;
			}
		}
	}

	void evaluate(CIDData& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Create the sub-images and displays
		Mat tImgFrequencies = Mat(1, 4, CV_8UC3);
		const int k_iNumFrequencies = 4; // The top n colors we will consider as frequent

		saveTo("ColorSnap", i_sImgName, getColorSnapped());

		if (i_bDisplay)
		{
			displayImage("ColorSnap", getColorSnapped());
			waitKey(0);
		}

		vector<Vec3b> tMostFrequentColors;
		CFrequencyCounter::getMostFrequentColors(tMostFrequentColors, k_iNumFrequencies, getColorSnapped(), m_vColors);

		for(unsigned int i = 0; i < k_iNumFrequencies && i < tMostFrequentColors.size(); i++)
		{
			tImgFrequencies.at<Vec3b>(0, i)= tMostFrequentColors[i];
		}

		assertbr(m_vColors.size() <= 32, "Too many colors to make a graph.");

		// Create the fequency graph
		Mat tFreqGraph = getFreqGraph();

		if (i_bDisplay)
		{
			const float k_fMagnification = 100.f;
			displayImage(i_sImgName + ": Most Frequent Colors", tImgFrequencies, k_fMagnification, k_fMagnification);
			displayImage("Freqencies", tFreqGraph, 1.f, 1.f);
			waitKey(0); 
		}

		// Ensure directories for the result directory
		std::string sResultDirectory = s_sResultDirectory;
		ensureDirectory(sResultDirectory);

		// Ensure directory for this ITH's results
		saveTo("FrequentColors", i_sImgName, tImgFrequencies);
		saveTo("FrequentColorsGraph", i_sImgName, tFreqGraph);

		// Calculate the evaluation string
		{
			// Save out the most frequent color
			o_sID.setCustom_ColorFrequencies(m_vColors);
		}
	}
};
