using namespace cimg_library;

struct CTraitHandler
{
  static std::vector<CTraitHandler*> s_lpAllHandlers;
  static CImg<double>* s_pImage;

  int m_iInstanceIndex;

  CTraitHandler()
    //: m_iInstanceIndex(s_lpAllHandlers.size())
  {
    // Add this to the list of all instances
    //s_lpAllHandlers.push_back(this);
  }

  ~CTraitHandler()
  {
    // Remove this from the list of all instances
    //s_lpAllHandlers.erase(s_lpAllHandlers.begin() + m_iInstanceIndex);
  }

  static void setProcessingImage(const std::string& i_sFilepath)
  {
    // Set the static processing image
    if(s_pImage)
    {
      delete s_pImage;
      s_pImage = NULL;
    }

    s_pImage = new CImg<double>(i_sFilepath.c_str());
  }

  virtual void handle() = 0;
  virtual double getSerialisation() = 0;
};

CImg<double>* CTraitHandler::s_pImage = NULL;
