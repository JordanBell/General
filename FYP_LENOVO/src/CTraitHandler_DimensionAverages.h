struct CTraitHandler_DimensionAverages : public CTraitHandler
{
  CImg<double> m_tVert;
  CImg<double> m_tHori;

#ifdef DISPLAY_RESULTS
  CImgDisplay disp_vert;
  CImgDisplay disp_hori;
#endif

  CTraitHandler_DimensionAverages()
  {

  }

  void evaluate(std::string& o_sID) override
  {
    // Create the sub-images and displays
    m_tVert = CImg<double>(1, s_pImage->height(), s_pImage->depth(), s_pImage->spectrum());
    m_tHori = CImg<double>(s_pImage->width(), 1, s_pImage->depth(), s_pImage->spectrum());

#ifdef DISPLAY_RESULTS
    disp_vert = CImgDisplay(m_tVert,"Vertical Averages");
    disp_hori = CImgDisplay(m_tHori,"Horizontal Averages");
#endif

    // Construct vert
    for(int i = 0; i < s_pImage->height(); i++)
    {
      CImg<double> tRow = s_pImage->get_row(i);
      m_tVert.set_xy_as_mean_of(0, i, tRow);
    }

    // Construct hori
    for(int i = 0; i < s_pImage->width(); i++)
    {
      CImg<double> tColumn = s_pImage->get_column(i);
      m_tHori.set_xy_as_mean_of(i, 0, tColumn);
    }

#ifdef DISPLAY_RESULTS
    const int slice_scale = 8;

    disp_vert = CImgDisplay(m_tVert,"Vertical Averages");
    disp_hori = CImgDisplay(m_tHori,"Horizontal Averages");
    disp_vert.resize(64, s_pImage->height() * slice_scale);
    disp_hori.resize(s_pImage->width() * slice_scale, 64);
#endif

    CImg<double> reconstructed(s_pImage->width(), s_pImage->height(), s_pImage->depth(), s_pImage->spectrum());
    for(int j = 0; j < reconstructed.height(); ++j)
    {
      for(int i = 0; i < reconstructed.width(); ++i)
      {
        for(int ci = 0; ci < reconstructed.spectrum(); ++ci)
        {
          reconstructed(i, j, 0, ci) = (m_tVert(0, j, 0, ci) + m_tVert(i, 0, 0, ci)) * 0.5f;
        }
      }
    }

#ifdef DISPLAY_RESULTS
    CImgDisplay disp_recon(reconstructed);
    disp_recon.resize(disp_recon.width() * 8, disp_recon.height() * 8);
#endif

    // Save the resulting images
    struct stat st;
    std::string sResultDirectory = s_sResultDirectory;
    if(stat(sResultDirectory.c_str(), &st) == -1)
    {
      int mkdirres = mkdir(sResultDirectory.c_str(), 0777);
      if(mkdirres != 0)
      {
        printf("Failed to make directory: %s", sResultDirectory.c_str());
        perror("");
        return;
      }
    }

    sResultDirectory += "DimAvg/";
    if(stat(sResultDirectory.c_str(), &st) == -1)
    {
      int mkdirres = mkdir(sResultDirectory.c_str(), 0777);
      if(mkdirres != 0)
      {
        printf("Failed to make directory: %s", sResultDirectory.c_str());
        perror("");
        return;
      }
    }

    std::string sOutFilepath;

    sOutFilepath = sResultDirectory + "reconstructed.bmp";
    reconstructed.save(sOutFilepath.c_str());
    sOutFilepath = sResultDirectory + "x.bmp";
    m_tVert.save(sOutFilepath.c_str());
    sOutFilepath = sResultDirectory + "y.bmp";
    m_tHori.save(sOutFilepath.c_str());

#ifdef DISPLAY_RESULTS
    while(!disp_recon.is_closed())
    {
      disp_recon.wait();
    }
#endif

    // Calculate the evaluation string
    {
      // Do vertical
      {
        o_sID += "v\n";
        for(int i = 0; i < m_tVert.height(); ++i)
        {
          CImg<double> tPix = m_tVert.get_row(i); // This will be a 1x1 image

          // Add each spectrum value
          for(int ci = 0; ci < tPix.spectrum(); ++ci)
          {
            const int iAsInt = (int)tPix(0, 0, 0, ci);
            o_sID += std::to_string(iAsInt);
            o_sID += ',';
          }
        }
      }

      // Do horizontal
      {
        o_sID += "h\n";
        for(int i = 0; i < m_tHori.width(); ++i)
        {
          CImg<double> tPix = m_tHori.get_column(i); // This will be a 1x1 image

          // Add each spectrum value
          for(int ci = 0; ci < tPix.spectrum(); ++ci)
          {
            const int iAsInt = (int)tPix(0, 0, 0, ci);
            o_sID += std::to_string(iAsInt);
            o_sID += '.';
          }
        }
      }
    }
  }
};
