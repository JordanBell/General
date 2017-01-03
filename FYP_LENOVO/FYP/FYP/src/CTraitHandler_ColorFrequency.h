using namespace std;
using namespace cv;

struct CTraitHandler_ColorFrequency : public CTraitHandler
{
	vector<pair<float, Vec3b>> m_vMostFrequent;

	struct CFrequencyCounter
	{
		vector<pair<Vec3b, float>> m_frequencies;

		void addColor(const Vec3b& i_vColor, const float i_fWeight)
		{
			// Add frequency to existing entry
			for(pair<Vec3b, float>& tPair : m_frequencies)
			{
				if(tPair.first == i_vColor)
				{
					tPair.second += i_fWeight;
					return;
				}
			}

			// Create new entry with this weight
			m_frequencies.push_back(pair<Vec3b, float>(i_vColor, i_fWeight));
		}

		void finalise(vector<pair<float, Vec3b>>& o_vMostFrequent)
		{
			// Build a sorted map of the frequencies
			multimap<float, Vec3b> mCounterColors;
			for(pair<Vec3b, float>& tPair : m_frequencies)
			{
				mCounterColors.emplace(tPair.second, tPair.first);
			}

			// Add the n most frequent values
			multimap<float, Vec3b>::iterator it = mCounterColors.end();

			// Build the sorted vector
			o_vMostFrequent.clear();
			bool bFirst = true;
			float fMaxFreq;
			do
			{
				--it;
				if(bFirst)
				{
					fMaxFreq = it->first;
					bFirst = false;
				}

				pair<float, Vec3b> tPair = *it;
				tPair.first /= fMaxFreq; // Set first to be a percentage of the max frequency
				o_vMostFrequent.push_back(tPair);
			} while(it != mCounterColors.begin());
		}
	};

	void getMostFrequentColors(vector<Vec3b>& o_vMostFrequent, int i_iNumToGet, const Mat& i_tSrc)
	{
		// Track how many times each color has been encountered
		CFrequencyCounter tCounter;

		// Build the frequency map
		const int k_iCenterX = i_tSrc.cols / 2.f;
		const int k_iCenterY = i_tSrc.rows / 2.f;
		for(int i = 0; i < i_tSrc.cols; i++)
		{
			for(int j = 0; j < i_tSrc.rows; j++)
			{
				float fDistanceFromCenter = sqrt(pow(i - k_iCenterX, 2) + pow(j - k_iCenterY, 2));
				float fWeighting = getRadialWeight(fDistanceFromCenter, max(i_tSrc.cols, i_tSrc.rows));

				tCounter.addColor(i_tSrc.at<Vec3b>(j, i), fWeighting);
			}
		}

		tCounter.finalise(m_vMostFrequent);

		for(int i = 0; i < i_iNumToGet && i < m_vMostFrequent.size(); ++i)
		{
			o_vMostFrequent.push_back(m_vMostFrequent[i].second);
		}
	}

	Mat getFreqGraph()
	{
		const float fGraphHeight = 400.f;
		const float fColumnWidth = 32;
		Mat tFreqGraph = Mat((int)fGraphHeight, m_vMostFrequent.size() * fColumnWidth, CV_8UC3);

		for(int i = 0; i < m_vMostFrequent.size(); ++i)
		{
			pair<float, Vec3b>& tPair = m_vMostFrequent[i];

			line(tFreqGraph, 
				Point(i * fColumnWidth + fColumnWidth / 2, 0),
				Point(i * fColumnWidth + fColumnWidth / 2, tPair.first * fGraphHeight),
				cvScalar(tPair.second[0], tPair.second[1], tPair.second[2]), 
				fColumnWidth / 2, 4);
		}

		return tFreqGraph;
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
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
		getMostFrequentColors(tMostFrequentColors, k_iNumFrequencies, getColorSnapped());

		for(unsigned int i = 0; i < k_iNumFrequencies && i < tMostFrequentColors.size(); i++)
		{
			tImgFrequencies.at<Vec3b>(0, i)= tMostFrequentColors[i];
		}

		assertbr(m_vMostFrequent.size() <= 32, "Too many colors to make a graph.");

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
			if(m_vMostFrequent[1].first < 0.5f)
			{
				// First color is definitely the most prominent.
				o_sID += "freq.most_hue: " + to_string(toHLS(m_vMostFrequent[0].second)[0]) + " | "; 
			}
			else if(m_vMostFrequent.back().first > 0.75f)
			{
				o_sID += "freq.equal | ";
			}
			else
			{
				o_sID += "freq.misc | ";
			}
		}
	}
};
