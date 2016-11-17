struct CTraitHandler_DimensionAverages : public CTraitHandler
{
  CImg<double> m_tVert;
  CImg<double> m_tHori;

  CImgDisplay disp_vert;
  CImgDisplay disp_hori;

  CTraitHandler_DimensionAverages()
    : m_tVert(1, s_pImage->height(), s_pImage->depth(), s_pImage->spectrum())
    , m_tHori(s_pImage->width(), 1, s_pImage->depth(), s_pImage->spectrum())
    , disp_vert(m_tVert,"Vertical Averages")
    , disp_hori(m_tHori,"Horizontal Averages")
  {

  }

  void handle() override
  {
    printf("Handling da (%d, %d)", s_pImage->width(), s_pImage->height());

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

    const int slice_scale = 8;
    disp_vert = CImgDisplay(m_tVert,"Vertical Averages");
    disp_hori = CImgDisplay(m_tHori,"Horizontal Averages");
    disp_vert.resize(64, s_pImage->height() * slice_scale);
    disp_hori.resize(s_pImage->width() * slice_scale, 64);

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

    CImgDisplay disp_recon(reconstructed);
    disp_recon.resize(disp_recon.width() * 8, disp_recon.height() * 8);

    while(!disp_recon.is_closed())
    {
      disp_recon.wait();
    }
  }

  double getSerialisation() override
  {
    return 0;
  }
};
