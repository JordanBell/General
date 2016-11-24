#define CONFIG_FILEPATH "test_config.xml"

using namespace rapidxml;

namespace NTesting
{
  struct CGroup : public std::vector<std::string>
  {
    enum EFlag
    {
      EFlag_Display,
    };

    // A collection of Images linked to an ID
    struct CIDImgs
    {
      std::string m_sImgID;
      std::vector<std::string> m_sImgNames;
    };

    std::vector<CIDImgs> m_tResults;

    static int s_iUnnamedGroupCounter;

    std::string m_sName;
    CBitset m_tFlags;

    void addResult(const std::string& i_sImgName, const std::string& i_sImgID)
    {
      for(CIDImgs& tResultCollection : m_tResults)
      {
        if(tResultCollection.m_sImgID == i_sImgID)
        {
          // Add to existing collection
          tResultCollection.m_sImgNames.push_back(i_sImgName);
          break;
        }
      }

      // Add a new result collection
      CIDImgs tResultCollection;
      tResultCollection.m_sImgID = i_sImgID;
      tResultCollection.m_sImgNames.push_back(i_sImgName);
      m_tResults.push_back(tResultCollection);
    }

    void printResults()
    {
      if(m_tResults.empty())
      {
        printf("\t-----\tEMPTY\t-----\n");
        printf("Group ID: %s\n", m_sName.c_str());
        printf("\n");
        return;
      }

      if(m_tResults.size() == 1)
      {
        CIDImgs& tResultCollection = m_tResults[0];
        printf("\t+++++\tSUCCESS!\t+++++\n");
        printf("\tGroup ID: %s\n", m_sName.c_str());
        printf("\tCount: %ld\n", tResultCollection.m_sImgNames.size());
        printf("\tID: %s\n", tResultCollection.m_sImgID.c_str());
        printf("\n");
      }
      else
      {
        printf("\t-----\tFRAGMENTED\t-----\n");
        printf("\tGroup ID: %s\n", m_sName.c_str());
        printf("\tSuccess Rating: %.0f%%\n", 1.f - ((float)m_tResults.size() / (float)size()) );
        printf("\tImage Count: %ld\n", size());
        printf("\tUnique Count: %ld\n", m_tResults.size());
        printf("\tUnique IDs:\n");
        for(CIDImgs& tResultCollection : m_tResults)
        {
          printf("\t\tID: %s\n", tResultCollection.m_sImgID.c_str());
          for(unsigned int i = 0; i < tResultCollection.m_sImgNames.size(); ++i)
          {
            std::string& sImgName = tResultCollection.m_sImgNames[i];
            printf("\t\t\t%d. %s\n", i+1, sImgName.c_str());
          }
          printf("\n");
        }
        printf("\n");
      }
    }
  };

  struct CConfig
  {
    enum EFlag
    {
      EFlag_PrintResults,
    };

    std::vector<CGroup> m_tGroups;
    CBitset m_tFlags;

    CConfig()
    {
      // Load the XML file
      file<> xmlFile(CONFIG_FILEPATH);
      xml_document<> m_tDoc;
      m_tDoc.parse<0>(xmlFile.data());

      xml_node<>* pRoot = m_tDoc.first_node("test_config");
      m_tFlags.set(EFlag_PrintResults, pRoot->first_attribute("printResults") != NULL);

      // Loop through groups
      for (xml_node<>* pGroup = pRoot->first_node("group"); pGroup; pGroup = pGroup->next_sibling())
      {
        CGroup tGroup;

        if(pGroup->first_attribute("name"))
        {
          tGroup.m_sName = pGroup->first_attribute("name")->value();
        }
        else
        {
          // Set a default name for the group
          tGroup.m_sName = "Group #";
          tGroup.m_sName += std::to_string(CGroup::s_iUnnamedGroupCounter++);
        }

        // Loop through members
        for (xml_node<>* pMember = pGroup->first_node("member"); pMember; pMember = pMember->next_sibling())
        {
          // Set attribute data
          tGroup.m_tFlags.set(CGroup::EFlag_Display, pMember->first_attribute("doDisplay") != NULL);

          // Add the value to the group
          tGroup.push_back(pMember->value());
        }

        m_tGroups.push_back(tGroup);
      }
    }
  };

  struct CTester
  {
    CConfig m_tConfig;

    void run()
    {
      for(CGroup& tGroup : m_tConfig.m_tGroups)
      {
        testGroup(tGroup);
      }
    }

    void testGroup(CGroup& i_tGroup)
    {
      // Create the trait handlers
      CTraitHandler_DimensionAverages hda;

      for(std::string sMemberName : i_tGroup)
      {
        if(CTraitHandler::setProcessingImage(sMemberName) < 0)
        {
          printf("Warning: Could not load image %s\n", sMemberName.c_str());
          continue;
        }

#ifdef DISPLAY_RESULTS
        CImgDisplay main_disp(*CTraitHandler::s_pImage, "Original Image");
        main_disp.resize(CTraitHandler::s_pImage->width() * 8, CTraitHandler::s_pImage->height() * 8);
#endif

        // Apply the trait handlers to build the Image ID
        std::string sImgID;
        hda.evaluate(sImgID);
        i_tGroup.addResult(sMemberName, sImgID);

        // Display the results until closes
#ifdef DISPLAY_RESULTS
        while (!main_disp.is_closed())
        {
          main_disp.wait();
        }
#endif
      }

      if(m_tConfig.m_tFlags.get(CConfig::EFlag_PrintResults))
      {
        i_tGroup.printResults();
      }
    }
  };

  int CGroup::s_iUnnamedGroupCounter = 0;
}
