//#define RECONSTRUCT

struct CTraitHandler_DimensionAverages : public CTraitHandler
{
	Mat m_tVert;
	Mat m_tHori;

	CTraitHandler_DimensionAverages()
	{
	}

	void evaluate(std::string& o_sID, const std::string& i_sImgName, const bool i_bDisplay) override
	{
		// Create the sub-images and displays
		m_tVert = s_tImage.col(0);
		m_tHori = s_tImage.row(0);

		// Construct vert
		for(int i = 0; i < s_tImage.rows; i++)
		{
			//m_tVert.at<Vec3b>(0, i) = mean(s_tImage.row(i)); // TODO: Maybe do something like this?
			setXYAsMeanOf(m_tVert, 0, i, s_tImage.row(i));
		}

		// Construct hori
		for(int i = 0; i < s_tImage.cols; i++)
		{
			setXYAsMeanOf(m_tHori, i, 0, s_tImage.col(i));
		}

#ifdef RECONSTRUCT
		Mat reconstructed = Mat(s_tImage.cols, s_tImage.rows, CV_8UC3);
		for(int i = 0; i < reconstructed.cols; ++i)
		{
			for(int j = 0; j < reconstructed.rows; ++j)
			{
				reconstructed.at<Vec3b>(j, i) = (m_tVert.at<Vec3b>(j, 0) + m_tHori.at<Vec3b>(0, i)) * 0.5f;
			}
		}
#endif

		if (i_bDisplay)
		{
			displayImage(i_sImgName + ": Horizontal Averages", m_tHori); 
			displayImage(i_sImgName + ": Vertical Averages", m_tVert);  
#ifdef RECONSTRUCT
			displayImage(i_sImgName + ": Reconstructed", reconstructed); 
#endif
			waitKey(0); 
		}


		// Ensure directories for the result directory
		std::string sResultDirectory = s_sResultDirectory;
		ensureDirectory(sResultDirectory);

		// Ensure directory for this ITH's results
		sResultDirectory += "DimAvg/";
		ensureDirectory(sResultDirectory);
		
		// Save the images to this ITH's directory
		imwrite(sResultDirectory + "x.bmp", m_tVert);
		imwrite(sResultDirectory + "y.bmp", m_tHori);
#ifdef RECONSTRUCT
		// Save the image to the reconstructed file
		imwrite(sResultDirectory + "reconstructed.bmp", reconstructed);
#else
		// Delete the reconstruction file (if one exists)
		DeleteFile((sResultDirectory + "reconstructed.bmp").c_str());
#endif

		// Calculate the evaluation string
		{
			//// Do vertical
			//{
			//	o_sID += "v: ";
			//	for(int i = 0; i < m_tVert.rows; ++i)
			//	{
			//		Vec3b vPixVal = m_tVert.at<Vec3b>(i, 0);

			//		// Add the vector values as strings
			//		o_sID += std::to_string(vPixVal[0]);
			//		o_sID += ',';
			//		o_sID += std::to_string(vPixVal[1]);
			//		o_sID += ',';
			//		o_sID += std::to_string(vPixVal[2]);
			//		o_sID += ',';
			//	}
			//}

			//// Do horizontal
			//{
			//	o_sID += "h: ";
			//	for(int i = 0; i < m_tHori.cols; ++i)
			//	{
			//		Vec3b vPixVal = m_tHori.at<Vec3b>(0, i);

			//		// Add the vector values as strings
			//		o_sID += std::to_string(vPixVal[0]);
			//		o_sID += ',';
			//		o_sID += std::to_string(vPixVal[1]);
			//		o_sID += ',';
			//		o_sID += std::to_string(vPixVal[2]);
			//		o_sID += ',';
			//	}
			//}
		}
	}
};
