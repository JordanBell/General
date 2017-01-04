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

	// Utility static functions
	static void getColors(Mat i_tSrc, vector<pair<float, Vec3b>>& i_vColors)
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

		tCounter.finalise(i_vColors);
	}

	static void getMostFrequentColors(vector<Vec3b>& o_vMostFrequent, unsigned int i_iNumToGet, const Mat& i_tSrc, vector<pair<float, Vec3b>>& i_vColors)
	{
		getColors(i_tSrc, i_vColors);

		for(unsigned int i = 0; i < i_iNumToGet && i < i_vColors.size(); ++i)
		{
			o_vMostFrequent.push_back(i_vColors[i].second);
		}
	}
};