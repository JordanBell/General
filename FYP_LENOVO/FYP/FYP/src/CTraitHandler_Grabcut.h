struct CTraitHandler_Grabcut : public CTraitHandler
{
	CTraitHandler_Grabcut()
	{
	}

	static void makeDefinite(unsigned char& io_iIntensity)
	{
		if(io_iIntensity == GC_PR_BGD)
		{
			io_iIntensity = GC_BGD;
		}
		else if(io_iIntensity == GC_PR_FGD)
		{
			io_iIntensity = GC_FGD;
		}
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Create the sub-images and displays

		// Get a color-snapped clone of the image
		Mat tMask(s_tImage.rows, s_tImage.cols, CV_8UC1);
		Mat tImage(s_tImage);
		Mat tBgd;
		Mat tFgd;

		int iRectDim = 20;

		if(iRectDim >= tImage.cols || iRectDim >= tImage.rows)
		{
			return;
		}

		Rect tRect(iRectDim / 2, iRectDim / 2, tImage.cols - iRectDim, tImage.rows - iRectDim);

		// Set the mask as an arbitrary circular proportion of the foreground
		//circle(tMask, Point(32, 32), 10, GC_FGD, 10.f);

		grabCut(tImage, tMask, tRect, tBgd, tFgd, 1, GC_INIT_WITH_RECT);

		// Modify mask to make possible pixels definite
		for_each(tMask.begin<unsigned char>(), tMask.end<unsigned char>(), [] (unsigned char& io_iIntensity) { makeDefinite(io_iIntensity); });

		// Display
		if (i_bDisplay)
		{
			displayImage(i_sImgName + ": Grabcut mask", tMask * 255);
			waitKey(0); 
		}

		// Save to images
		string sResultDirectory = "Results/GCMasks/";
		ensureDirectory(sResultDirectory);
		imwrite(sResultDirectory + i_sImgName + ".bmp", tMask);
		imwrite(s_sResultDirectory + "GCMask.bmp", tMask);

		// Calculate the evaluation string
		{
			// Arbitrary general aspect: Some foreground found
			o_sID += "gc_fgd:";
			if(countNonZero(tMask) > 0)
			{
				o_sID += "y";
			}
			else
			{
				o_sID += "n";
			}
		}
	}
};
