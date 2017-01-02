struct CTraitHandler_ColorFrequency : public CTraitHandler
{
	Mat m_tOut;

	CTraitHandler_ColorFrequency()
	{
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Create the sub-images and displays
		m_tOut = Mat(1, 4, CV_8UC3);
		const int k_iNumFrequencies = 4; // The top n colors we will consider as frequent

		saveTo("ColorSnap", i_sImgName, getColorSnapped());

		Mat tBlurBox = s_tImage.clone();
		Mat tBlurMedian = s_tImage.clone();

		boxFilter(tBlurBox, tBlurBox, tBlurBox.type(), Size(3, 3));
		medianBlur(tBlurMedian, tBlurMedian, 5);

		saveTo("Blur", i_sImgName, tBlurBox);
		saveTo("BlurMedian", i_sImgName, tBlurMedian);

		Mat tBlurBoxCS = getColorSnapped().clone();
		Mat tBlurMedianCS = getColorSnapped().clone();

		boxFilter(tBlurBoxCS, tBlurBoxCS, tBlurBoxCS.type(), Size(3, 3));
		medianBlur(tBlurMedianCS, tBlurMedianCS, 5);

		saveTo("BlurCS", i_sImgName, tBlurBoxCS);
		saveTo("BlurMedianCS", i_sImgName, tBlurMedianCS);

		if (i_bDisplay)
		{
			displayImage("ColorSnap", getColorSnapped());
			displayImage("Box Blur", tBlurBox);
			displayImage("Median Blur", tBlurMedian); 
			displayImage("Box Blur CS", tBlurBoxCS);
			displayImage("Median Blur CS", tBlurMedianCS); 

			waitKey(0);
		}

		//vector<Vec3b> tMostFrequentColors;
		//getMostFrequentColors(tMostFrequentColors, k_iNumFrequencies, getColorSnapped());

		//for(unsigned int i = 0; i < k_iNumFrequencies && i < tMostFrequentColors.size(); i++)
		//{
		//	m_tOut.at<Vec3b>(0, i)= tMostFrequentColors[i];
		//}

		//if (i_bDisplay)
		//{
		//	const int k_iMagnification = 100;
		//	displayImage(i_sImgName + ": Most Frequent Colors", m_tOut, (float)k_iMagnification, (float)k_iMagnification); 
		//	waitKey(0); 
		//}

		//// Ensure directories for the result directory
		//std::string sResultDirectory = s_sResultDirectory;
		//ensureDirectory(sResultDirectory);

		//// Ensure directory for this ITH's results
		//sResultDirectory += "Color/";
		//ensureDirectory(sResultDirectory);

		//// Save the images to this ITH's directory
		//imwrite(sResultDirectory + "mostFrequent.bmp", m_tOut);

		//// Calculate the evaluation string
		//{
		//	// TODO
		//}
	}
};
