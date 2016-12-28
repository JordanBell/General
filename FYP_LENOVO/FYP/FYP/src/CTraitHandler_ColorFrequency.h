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
		
		// Get a color-snapped clone of the image
		//grabCut( *image, mask, rect, bgdModel, fgdModel, 1 );
		Mat tImage = s_tImage.clone();
		cvtColor(tImage, tImage, CV_BGR2HLS);
		for_each(tImage.begin<Vec3b>(), tImage.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 1); });

		// Create a histogram of the image
		Mat tHistImage;
		makeHistogramHue(tHistImage, tImage);

		/// Display
		if(i_bDisplay)
		{
			namedWindow(i_sImgName + ": Hue Hist", CV_WINDOW_AUTOSIZE );
			imshow(i_sImgName + ": Hue Hist", tHistImage);
			waitKey(0); 
		}

		// Save to histogram images
		string sResultDirectoryHist = "Results/Histogram_Hue/";
		ensureDirectory(sResultDirectoryHist);
		imwrite(sResultDirectoryHist + i_sImgName + ".bmp", tHistImage);
		imwrite(s_sResultDirectory + "/Histogram_Hue.bmp", tHistImage);

		vector<Vec3b> tMostFrequentColors;
		getMostFrequentColors(tMostFrequentColors, k_iNumFrequencies, tImage);

		for(unsigned int i = 0; i < k_iNumFrequencies && i < tMostFrequentColors.size(); i++)
		{
			m_tOut.at<Vec3b>(0, i)= tMostFrequentColors[i];
		}

		if (i_bDisplay)
		{
			const int k_iMagnification = 100;
			displayImage(i_sImgName + ": Most Frequent Colors", m_tOut, (float)k_iMagnification, (float)k_iMagnification); 
			waitKey(0); 
		}

		// Ensure directories for the result directory
		std::string sResultDirectory = s_sResultDirectory;
		ensureDirectory(sResultDirectory);

		// Ensure directory for this ITH's results
		sResultDirectory += "Color/";
		ensureDirectory(sResultDirectory);

		// Save the images to this ITH's directory
		imwrite(sResultDirectory + "mostFrequent.bmp", m_tOut);

		// Calculate the evaluation string
		{
			// TODO
		}
	}
};
