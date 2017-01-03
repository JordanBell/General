struct CTraitHandler_DimensionAverages : public CTraitHandler
{
	Mat m_tVert;
	Mat m_tHori;

	CTraitHandler_DimensionAverages()
	{
	}

	// Perhaps now deprecated
	static void setXYAsMeanOf(Mat& dst, int x, int y, const Mat& src, float i_fAltDimDist, bool i_bVert)
	{
		// Get the average pixel value in src
		Vec3f vAverage = 0;
		float fTotalWeight = 0.f;

		// How to get BGR value
		for(int i = 0; i < src.cols; i++)
		{
			for(int j = 0; j < src.rows; j++)
			{
				float fWeight;
				if(i_bVert)
				{
					float fDistToCenter = sqrt(pow(i_fAltDimDist, 2) + pow(j - src.rows / 2, 2));
					fWeight = getRadialWeight(fDistToCenter, src.rows);
				}
				else
				{
					float fDistToCenter = sqrt(pow(i_fAltDimDist, 2) + pow(i - src.cols / 2, 2));
					fWeight = getRadialWeight(fDistToCenter, src.cols);
				}

				vAverage += src.at<Vec3b>(j, i) * fWeight;
				fTotalWeight += fWeight;
			}
		}

		vAverage /= fTotalWeight;

		// Set the pixel at x y in dst as that average value
		dst.at<Vec3b>(y, x) = vAverage;
	}

	void init()
	{
		Mat tResized;

		// Resize the color-snapped image
		Size tNewSize((int)s_tImage.cols / 8, (int)s_tImage.rows / 8);
		resize(getColorSnapped(), tResized, tNewSize); 

		// Create the sub-images
		m_tVert = tResized.col(0);
		m_tHori = tResized.row(0);

		// Construct vert
		for(int i = 0; i < tResized.rows; i++)
		{
			setXYAsMeanOf(m_tVert, 0, i, tResized.row(i), i - tResized.cols / 2, false);
		}

		// Construct hori
		for(int i = 0; i < tResized.cols; i++)
		{
			setXYAsMeanOf(m_tHori, i, 0, tResized.col(i), i - tResized.rows / 2, true);
		}
	}

	// Returns the Channel Delta Count for a given BGR image for a given HLS given channel index.
	static int getCDC(const Mat i_tSrc, const int i_iChannelIndex)
	{
		// A channel delta occurs when a consecutive pixel's value along a certain channel differs beyond a certain threshold from the value at the last occurrence of a channel delta. If no previous occurrence, the value of the first pixel.

		// Convert to HLS
		cvtColor(i_tSrc, i_tSrc, CV_BGR2HLS);

		int r_iCDC = 0;
		unsigned char iLast = i_tSrc.at<Vec3b>(0, 0)[0];
		
		for (auto it = i_tSrc.begin<Vec3b>(); it != i_tSrc.end<Vec3b>(); it++)
		{
			const unsigned char iThis = (*it)[i_iChannelIndex];
			if(fabsf(iThis - iLast) > CDC_THRESHOLD)
			{
				++r_iCDC;
				iLast = iThis;
			}
		}

		return r_iCDC;
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		if(s_tImage.cols < 8 || s_tImage.rows < 8)
		{
			o_sID += "VH.nop | ";
			return;
		}

		init();

		if (i_bDisplay)
		{
			displayImage(i_sImgName + ": Horizontal Averages", m_tHori, 64.f, 8.f); 
			displayImage(i_sImgName + ": Vertical Averages", m_tVert, 8.f, 64.f);  
			waitKey(0); 
		}


		// Calculate the evaluation string
		{
			// Do vertical
			{
				cvtColor(m_tVert, m_tVert, CV_BGR2HLS);

				o_sID += "V.hdc: " + to_string(getCDC(m_tVert, 0));
				o_sID += " | ";
			}

			// Do horizontal
			{

				o_sID += "H.hdc: " + to_string(getCDC(m_tHori, 0));
				o_sID += " | ";
			}
		}
	}
};
