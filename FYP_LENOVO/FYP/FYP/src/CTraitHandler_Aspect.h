struct CTraitHandler_Aspect : public CTraitHandler
{
	void evaluate(CIDData& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		const float fAspect = s_tImage.cols - s_tImage.rows;

		o_sID.addFloat("wh_diff", fAspect, ALLOWANCE_WIDTH_HEIGHT_DIFF);
	}
};