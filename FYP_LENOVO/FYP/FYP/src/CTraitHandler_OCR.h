struct CTraitHandler_OCR : public CTraitHandler
{
	void evaluate(CIDData& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		string sDetected;

		o_sID.addString("OCR", sDetected);
	}
};