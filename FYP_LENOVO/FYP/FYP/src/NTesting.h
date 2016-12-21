#define CONFIG_FILEPATH "test_config.xml"
#define OUTPUT_LOG_FILENAME "results.txt"

using namespace rapidxml;
using namespace std;

namespace NTesting
{

	struct CGroup : public vector<string>
	{
		enum EFlag
		{
			EFlag_Display,
		};

		// A collection of Images linked to an ID
		struct CIDImgs
		{
			string m_sImgID;
			vector<string> m_sImgNames;
		};

		vector<CIDImgs> m_tResults;

		static int s_iUnnamedGroupCounter;

		string m_sName;
		CBitset m_tFlags;

		void addResult(const string& i_sImgName, const string& i_sImgID)
		{
			for(CIDImgs& tResultCollection : m_tResults)
			{
				if(tResultCollection.m_sImgID == i_sImgID)
				{
					// Add to existing collection
					tResultCollection.m_sImgNames.push_back(i_sImgName);
					return;
				}
			}

			// Add a new result collection
			CIDImgs tResultCollection;
			tResultCollection.m_sImgID = i_sImgID;
			tResultCollection.m_sImgNames.push_back(i_sImgName);
			m_tResults.push_back(tResultCollection);
		}

		void printResults(FILE* i_pOutFile)
		{
			if(m_tResults.empty())
			{
				fprintf(i_pOutFile, "\t-----\tEMPTY\t-----\n");
				fprintf(i_pOutFile, "Group ID: %s\n", m_sName.c_str());
				fprintf(i_pOutFile, "\n");
			}
			else
			{
				if(m_tResults.size() == 1)
				{
					CIDImgs& tResultCollection = m_tResults[0];
					fprintf(i_pOutFile, "\t+++++\t%s\t+++++\n", m_sName.c_str());
					fprintf(i_pOutFile, "\tCount: %ld\n", tResultCollection.m_sImgNames.size());
					fprintf(i_pOutFile, "\tID: %s\n", tResultCollection.m_sImgID.c_str());
					fprintf(i_pOutFile, "\n");
				}
				else
				{
					fprintf(i_pOutFile, "\t-----\t%s [~Fragmented]\t-----\n", m_sName.c_str());
					fprintf(i_pOutFile, "\tSuccess Rating: %.0f%%\n", 1.f - ((float)m_tResults.size() / (float)size()) );
					fprintf(i_pOutFile, "\tImage Count: %ld\n", size());
					fprintf(i_pOutFile, "\tUnique Count: %ld\n", m_tResults.size());
					fprintf(i_pOutFile, "\tUnique IDs:\n");
					for(CIDImgs& tResultCollection : m_tResults)
					{
						fprintf(i_pOutFile, "\t\tID: %s\n", tResultCollection.m_sImgID.c_str());
						for(unsigned int i = 0; i < tResultCollection.m_sImgNames.size(); ++i)
						{
							string& sImgName = tResultCollection.m_sImgNames[i];
							fprintf(i_pOutFile, "\t\t\t%d. %s\n", i+1, sImgName.c_str());
						}
						fprintf(i_pOutFile, "\n");
					}
					fprintf(i_pOutFile, "\n");
				}
			}
		}
	};

	struct CConfig
	{
		enum EFlag
		{
			EFlag_PrintResults,
		};

		vector<CGroup> m_tGroups;
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
					tGroup.m_sName += to_string(CGroup::s_iUnnamedGroupCounter++);
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
		FILE* m_pResultsOutFile;

		void init()
		{
			// Determine the file we will print the results to
#if defined(OUTPUT_LOG_FILENAME)
			errno_t err = fopen_s(&m_pResultsOutFile, OUTPUT_LOG_FILENAME, "w");
			assertbr(err == 0, "Failed to open file: " + string(OUTPUT_LOG_FILENAME));
#else
			m_pResultsOutFile = stdout;
#endif
		}

		void run()
		{
			init();

			// Test every group
			for(CGroup& tGroup : m_tConfig.m_tGroups)
			{
				testGroup(tGroup);
			}

			cleanup();
		}

		void cleanup()
		{
#if defined(OUTPUT_LOG_FILENAME)
			// Close the log file
			fclose(m_pResultsOutFile);
#endif
		}

		void testGroup(CGroup& i_tGroup)
		{
			// Create the trait handlers
			CTraitHandler_DimensionAverages ithDimensionAverages;
			CTraitHandler_Color ithColors;

			for(string sMemberName : i_tGroup)
			{
				if(CTraitHandler::setProcessingImage(sMemberName) < 0)
				{
					assertbr(0, "Error: Could not load image: \"" + sMemberName + "\"\n");
					continue;
				}

#ifdef DISPLAY_RESULTS
				displayImage(sMemberName + ": Original Image", CTraitHandler::s_tImage);
				waitKey(0);
#endif

				// Apply the trait handlers to build the Image ID
				string sImgID;
				//ithDimensionAverages.evaluate(sImgID, sMemberName);
				ithColors.evaluate(sImgID, sMemberName);
				i_tGroup.addResult(sMemberName, sImgID);
			}

			if(m_tConfig.m_tFlags.get(CConfig::EFlag_PrintResults))
			{
				i_tGroup.printResults(m_pResultsOutFile);
			}
		}
	};

	int CGroup::s_iUnnamedGroupCounter = 0;
}
