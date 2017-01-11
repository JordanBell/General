struct CTraitHandler_CannyCount : public CTraitHandler
{
	static const bool s_bCullRadius = false;
	static const int s_iThreshold = 50;

	void evaluate(CIDData& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Perform canny on the image with different thresholds.
		{
			Mat tCannyResult;
			Canny(s_tImage, tCannyResult, s_iThreshold, s_iThreshold * 2);
			o_sID.addFloat("CannyCount_Threshold", countNonZero(tCannyResult), ALLOWANCE_CANNY_COUNT);
			saveTo("Canny", i_sImgName, tCannyResult);
		}
	}
};