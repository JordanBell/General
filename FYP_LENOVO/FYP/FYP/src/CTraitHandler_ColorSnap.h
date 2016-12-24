struct CTraitHandler_ColorSnap : public CTraitHandler
{
	Mat m_tOut;

	CTraitHandler_ColorSnap()
	{
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Create the sub-images and displays
		Mat tNormalised = s_tImage.clone();

		// Normalise each 
		for_each(tNormalised.begin<Vec3b>(), tNormalised.end<Vec3b>(), [] (Vec3b& io_vColor) { normalise(io_vColor); });

		string sResultDirectory = "Results/Normalisation/";
		ensureDirectory(sResultDirectory);
		imwrite(sResultDirectory + i_sImgName + ".bmp", tNormalised);
		imwrite(s_sResultDirectory + "/Normalised.bmp", tNormalised);

		// Snap each to the nearest hue
		Mat tColorSnapped = tNormalised.clone();
		for_each(tColorSnapped.begin<Vec3b>(), tColorSnapped.end<Vec3b>(), [] (Vec3b& io_vColor) { snapToHue(io_vColor); });

		sResultDirectory = "Results/ColorSnap/";
		ensureDirectory(sResultDirectory);
		imwrite(sResultDirectory + i_sImgName + ".bmp", tColorSnapped);
		imwrite(s_sResultDirectory + "/ColorSnap.bmp", tColorSnapped);

		if (i_bDisplay)
		{
			displayImage(i_sImgName + ": Normalised", tNormalised); 
			displayImage(i_sImgName + ": Color Snapped", tColorSnapped); 
			waitKey(0); 
		}
	}
};
