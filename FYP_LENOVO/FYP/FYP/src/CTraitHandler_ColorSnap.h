struct CTraitHandler_ColorSnap : public CTraitHandler
{
	Mat m_tOut;

	CTraitHandler_ColorSnap()
	{
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Snap each to the nearest hue
		Mat tColorSnapped = s_tImage.clone();
		cvtColor(tColorSnapped, tColorSnapped, CV_BGR2HLS);
		//for_each(tColorSnapped.begin<Vec3b>(), tColorSnapped.end<Vec3b>(), [] (Vec3b& io_vColor) { snapColorsHLS(io_vColor, 1); });
		cvtColor(tColorSnapped, tColorSnapped, CV_HLS2BGR);

		string sResultDirectory = "Results/ColorSnap/";
		ensureDirectory(sResultDirectory);
		imwrite(sResultDirectory + i_sImgName + ".bmp", tColorSnapped);
		imwrite(s_sResultDirectory + "/ColorSnap.bmp", tColorSnapped);

		// Normalise each 
		Mat tNormalised = s_tImage.clone();
		cvtColor(tNormalised, tColorSnapped, CV_BGR2HLS);
		for_each(tNormalised.begin<Vec3b>(), tNormalised.end<Vec3b>(), [] (Vec3b& io_vColor) { normaliseHLS(io_vColor); });
		cvtColor(tColorSnapped, tColorSnapped, CV_HLS2BGR);

		sResultDirectory = "Results/Normalisation/";
		ensureDirectory(sResultDirectory);
		imwrite(sResultDirectory + i_sImgName + ".bmp", tNormalised);
		imwrite(s_sResultDirectory + "/Normalised.bmp", tNormalised);

		if (i_bDisplay)
		{
			displayImage(i_sImgName + ": Normalised", tNormalised); 
			displayImage(i_sImgName + ": Color Snapped", tColorSnapped); 
			waitKey(0); 
		}
	}
};
