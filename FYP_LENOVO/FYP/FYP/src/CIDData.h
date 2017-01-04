using namespace std;

template <typename T>
struct CIDMember
{
	string m_sID;
	T m_tValue;
	T m_tAllowanceAbs;

	CIDMember(const string& i_sID, const T& i_tValue, const T& i_tAllowanceAbs)
		: m_sID(i_sID)
		, m_tValue(i_tValue)
		, m_tAllowanceAbs(i_tAllowanceAbs)
	{}

	bool isLike(const CIDMember<T>& i_tOther) const;
};

bool CIDMember<float>::isLike(const CIDMember<float>& i_tOther) const
{
	return fabsf(i_tOther.m_tValue - m_tValue) < m_tAllowanceAbs;
}

bool CIDMember<bool>::isLike(const CIDMember<bool>& i_tOther) const
{
	return i_tOther.m_tValue == m_tValue;
}

struct CIDMember_ColorFrequencies
{
	static const int sk_iAllowanceSize = 2;
	static const int sk_iAllowanceMutual = 2;
	vector<pair<float, Vec3b>> m_vSortedColorFrequencies;

	static int getNumMutual(vector<pair<float, Vec3b>> i_v0, vector<pair<float, Vec3b>> i_v1)
	{
		int r_i = 0;

		for(auto p0 : i_v0)
		{
			for(auto p1 : i_v1)
			{
				if(p0.second == p1.second)
				{
					r_i++;
					break;
				}
			}
		}

		return r_i;
	}

	int getMatches(const CIDMember_ColorFrequencies& i_tOther) const
	{
		int r_iNumMatches = 0;

		const auto& v0 = m_vSortedColorFrequencies;
		const auto& v1 = i_tOther.m_vSortedColorFrequencies;

		if(fabsf((float)v0.size() - (float)v1.size()) <= sk_iAllowanceMutual)
		{
			// Their sizes are within the size threshold
			++r_iNumMatches;
		}

		const int iNumMutual = getNumMutual(v0, v1);
		if((int)min(v0.size(), v1.size()) - iNumMutual <= sk_iAllowanceMutual)
		{
			// The number of shared colors is within threshold
			++r_iNumMatches;
		}

		// Frequency matches - check that each color appears roughly the same amount
		int iColorFreqMatches = 0;
		for(auto p0 : v0)
		{
			float f0 = p0.first;
			for(auto p1 : v1)
			{
				if(p0.second == p1.second)
				{
					// Check that this color has a similar frequency
					float f1 = p1.first;
					float fDiff = fabsf(f0 - f1);
					if(fDiff <= ALLOWANCE_COLOR_FREQUENCY)
					{
						// Within the color frequency similarity threshold
						++iColorFreqMatches;
						break;
					}
				}
			}
		}


		if(fabsf(iColorFreqMatches - iNumMutual) < ALLOWANCE_COLOR_FREQUENCY_MATCHES)
		{
			++r_iNumMatches;
		}

		return r_iNumMatches;
	}

	int getMatches() const
	{
		// The 3 comes from the 2 matches for the size, mutual color and color frequency checks
		return 3;
	}
};

struct CIDData
{
	vector<CIDMember<float>> m_tFloats; 
	vector<CIDMember<bool>> m_tBools; 
	CIDMember_ColorFrequencies m_tCustom_CF; 

	CIDData()
	{
	}

	CIDData(vector<CIDData> i_vAverageOf)
	{
		// Set this data as the average of all above values
		for(CIDData& tID : i_vAverageOf)
		{
			// TODO
		}

		m_tFloats = i_vAverageOf[0].m_tFloats;
		m_tBools = i_vAverageOf[0].m_tBools;
		m_tCustom_CF = i_vAverageOf[0].m_tCustom_CF;
	}

	void addFloat(const string& i_sID, const float i_fValue, const float i_fAllowanceAbs)
	{
		m_tFloats.push_back(CIDMember<float>(i_sID, i_fValue, i_fAllowanceAbs));
	}

	void addBool(const string& i_sID, const bool i_bValue = true)
	{
		m_tBools.push_back(CIDMember<bool>(i_sID, i_bValue, false));
	}

	void setCustom_ColorFrequencies(vector<pair<float, Vec3b>>& i_vSortedColorFrequencies)
	{
		m_tCustom_CF.m_vSortedColorFrequencies.clear();
		for(auto thing : i_vSortedColorFrequencies)
		{
			m_tCustom_CF.m_vSortedColorFrequencies.push_back(thing);
		}
	}

	template <typename T>
	static const CIDMember<T>* getMemFrom(const vector<CIDMember<T>>& i_tMemList, const string& i_sID)
	{
		for(const CIDMember<T>& tMem : i_tMemList)
		{
			if(tMem.m_sID == i_sID)
			{
				return &tMem;
			}
		}

		return NULL;
	}

	const CIDMember<float>* getFMem(const string& i_sID) const { return getMemFrom(m_tFloats, i_sID);	}
	const CIDMember<bool>* getBMem(const string& i_sID) const	{ return getMemFrom(m_tBools, i_sID); }

	template <typename T>
	static int getNumMatches(const vector<CIDMember<T>>& i_tThis, const vector<CIDMember<T>>& i_tOther)
	{
		int r_iNumMatches = 0;
		for(const CIDMember<T>& tMem : i_tThis)
		{
			const CIDMember<T>* pfCorresponding = getMemFrom(i_tOther, tMem.m_sID);
			if(pfCorresponding && tMem.isLike(*pfCorresponding))
			{
				++r_iNumMatches;
			}
		}

		return r_iNumMatches;
	}

	int getTotalPossibleMatches() const
	{
		return m_tFloats.size() + m_tBools.size() + m_tCustom_CF.getMatches();
	}

	bool operator==(const CIDData& i_tOther) const
	{
		// Floats
		int numMatches = 0;
		numMatches += m_tCustom_CF.getMatches(i_tOther.m_tCustom_CF);
		numMatches += getNumMatches(m_tFloats, i_tOther.m_tFloats);
		numMatches += getNumMatches(m_tBools, i_tOther.m_tBools);
		return fabsf(numMatches - getTotalPossibleMatches()) <= ALLOWANCE_TOTAL_MATCHES;
	}

	string toString() const
	{
		// TODO: Include typing. Allow for perfect reconstruction from the serialised string
		string r_s;
		r_s += "[Color Frequencies not displayed]\n";
		for(CIDMember<float> tbMem : m_tFloats)
		{
			r_s += tbMem.m_sID + ": " + to_string(tbMem.m_tValue) + "\n";
		}
		for(CIDMember<bool> tbMem : m_tBools)
		{
			r_s += tbMem.m_sID + ": " + (tbMem.m_tValue ? "y" : "n") + "\n";
		}

		return r_s;
	}
};