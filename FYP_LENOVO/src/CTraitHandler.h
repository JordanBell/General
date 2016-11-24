using namespace cimg_library;

struct CTraitHandler
{
  static std::vector<CTraitHandler*> s_lpAllHandlers;
  static CImg<double>* s_pImage;
  static std::string s_sResultDirectory;

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

  static int setProcessingImage(std::string i_sFilepath)
  {
    // Set the static processing image
    if(s_pImage)
    {
      delete s_pImage;
      s_pImage = NULL;
    }

    i_sFilepath = TEST_IMAGES_DIR + i_sFilepath;

    s_pImage = new CImg<double>(i_sFilepath.c_str());
    if(s_pImage == NULL)
    {
      // Failed to load image
      return -1;
    }

    i_sFilepath = i_sFilepath.substr(i_sFilepath.rfind('/') + 1, i_sFilepath.rfind('.'));

    // Make a directory for this in the results directory
    s_sResultDirectory = "Results/" + i_sFilepath + "/";
    struct stat st;
    if(stat(s_sResultDirectory.c_str(), &st) == -1)
    {
      printf("Failed to find dir: %s. Making one now.\n", s_sResultDirectory.c_str());
      if(mkdir(s_sResultDirectory.c_str(), 0777) != 0)
      {
        perror("Failed to make directory");
        return -1;
      }
    }

    return 0;
  }

  // Returns a string result for the subject image
  virtual void evaluate(std::string& o_sID) = 0;
};

std::string CTraitHandler::s_sResultDirectory = "";
CImg<double>* CTraitHandler::s_pImage = NULL;
