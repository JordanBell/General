using namespace rapidxml;
using namespace std;
using namespace std::chrono;

namespace NTesting
{
	struct CGroup : public vector<string>
	{
		enum EFlag
		{
			EFlag_Display,
		};

		// A collection of Images linked to an ID
		struct CResultGroup
		{
			CIDData m_tGroupImgID;
			vector<string> m_sImgNames;
			vector<CIDData> m_tImgIDs;

			void addImage(const string& i_sImgName, const CIDData& i_tImgID)
			{
				// Add string to names
				m_sImgNames.push_back(i_sImgName);
				m_tImgIDs.push_back(i_tImgID);

				// Set the group img ID as the new average
				m_tGroupImgID = CIDData(m_tImgIDs);
			}
		};

		vector<CResultGroup> m_tResults;

		static int s_iUnnamedGroupCounter;

		string m_sName;
		CBitset m_tFlags;

		bool m_bFullyCongruent;

		void addResult(const string& i_sImgName, const CIDData& i_tImgID)
		{
			for(CResultGroup& tResultGroup : m_tResults)
			{
				if(tResultGroup.m_tGroupImgID == i_tImgID)
				{
					// Add to existing collection
					tResultGroup.addImage(i_sImgName, i_tImgID);
					return;
				}
			}

			// Add a new result collection
			CResultGroup tResultGroup;
			tResultGroup.addImage(i_sImgName, i_tImgID);
			m_tResults.push_back(tResultGroup);
		}

		void getMinMaxResults(float& o_fMin, float& o_fMax)
		{
			o_fMin = FLT_MAX;
			o_fMax = -FLT_MAX;
			for(const CResultGroup& tResultGroup : m_tResults)
			{
				for(unsigned int i = 0; i < tResultGroup.m_tImgIDs.size(); ++i)
				{
					const CIDData& tImageID = tResultGroup.m_tImgIDs[i];
					for(auto& tMem : tImageID.m_tFloats)
					{
						if(tMem.m_tValue > o_fMax)
						{
							o_fMax = tMem.m_tValue;
						}

						if(tMem.m_tValue < o_fMin)
						{
							o_fMin = tMem.m_tValue;
						}
					}
				}
			}
		}

		float getStandardDeviation()
		{
			float fMean;
			float fTotal = 0.f;
			float r_fStandardDeviation = 0.f;

			// Get the total
			int iCount = 0;
			for(const CResultGroup& tResultGroup : m_tResults)
			{
				for(unsigned int i = 0; i < tResultGroup.m_tImgIDs.size(); ++i)
				{
					const CIDData& tImageID = tResultGroup.m_tImgIDs[i];
					for(auto& tMem : tImageID.m_tFloats)
					{
						fTotal += tMem.m_tValue;
						++iCount;
					}
				}
			}

			// Get Mean
			fMean = fTotal / iCount;

			// Build up the total SD
			for(const CResultGroup& tResultGroup : m_tResults)
			{
				for(unsigned int i = 0; i < tResultGroup.m_tImgIDs.size(); ++i)
				{
					const CIDData& tImageID = tResultGroup.m_tImgIDs[i];
					for(auto& tMem : tImageID.m_tFloats)
					{
						r_fStandardDeviation += pow(tMem.m_tValue - fMean, 2);
					}
				}
			}

			// SD is sqrt of mean deviation
			return sqrt(r_fStandardDeviation / iCount);
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
					CResultGroup& tResultCollection = m_tResults[0];
					fprintf(i_pOutFile, "\t+++++\t%s\t+++++\n", m_sName.c_str());
					fprintf(i_pOutFile, "\tCount: %ld\n", tResultCollection.m_sImgNames.size());
					fprintf(i_pOutFile, "\tID: %s\n", tResultCollection.m_tGroupImgID.toString().c_str());
					fprintf(i_pOutFile, "\n");
				}
				else
				{
					fprintf(i_pOutFile, "\t-----\t%s [~Fragmented]\t-----\n", m_sName.c_str());
					fprintf(i_pOutFile, "\tSuccess Rating: %.0f%%\n", 100.f * (1.f - ((float)m_tResults.size() / (float)size())) );
					fprintf(i_pOutFile, "\tImage Count: %ld\n", size());
					fprintf(i_pOutFile, "\tUnique Count: %ld\n", m_tResults.size());
					fprintf(i_pOutFile, "\tUnique IDs:\n");
					for(CResultGroup& tResultCollection : m_tResults)
					{
						fprintf(i_pOutFile, "\t\tID: %s\n", tResultCollection.m_tGroupImgID.toString().c_str());
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
		vector<CGroup> m_tGroups;

		CConfig(const string& filepath = CONFIG_FILEPATH)
		{
			// Load the XML file
			file<> xmlFile(filepath.c_str());
			xml_document<> m_tDoc;
			m_tDoc.parse<0>(xmlFile.data());

			xml_node<>* pRoot = m_tDoc.first_node("testConfig");

			// Loop through groups
			for (xml_node<>* pGroup = pRoot->first_node("group"); pGroup; pGroup = pGroup->next_sibling())
			{
				CGroup tGroup;

				// Will we display this group's results?
				tGroup.m_tFlags.set(CGroup::EFlag_Display, pGroup->first_attribute("doDisplay") != NULL && strcmp(pGroup->first_attribute("doDisplay")->value(), "true") == 0);

				// Set the group name
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

				// Add any members
				for (xml_node<>* pMember = pGroup->first_node("member"); pMember; pMember = pMember->next_sibling("member"))
				{
					// Add the value to the group
					tGroup.push_back(pMember->value());
				} 
				
				// Activate any member generators
				if (pGroup->first_node("memberGenerator"))
				{
					const char* sAttributePrefix = "prefix";
					const char* sAttributeCount = "count";
					const char* sAttributeExtension = "extension";

					// Add members from the generators' attributes
					for (xml_node<>* pGenerator = pGroup->first_node("memberGenerator"); pGenerator; pGenerator = pGenerator->next_sibling("memberGenerator"))
					{
						// Make sure all attributes are there.
						assertbr(pGenerator->first_attribute(sAttributePrefix) && pGenerator->first_attribute(sAttributeCount) && pGenerator->first_attribute(sAttributeExtension), "A member generator within group \"" + tGroup.m_sName + "\" is missing its required attributes");

						// Set attribute data
						string sPrefix = pGenerator->first_attribute(sAttributePrefix)->value();
						string sCount = pGenerator->first_attribute(sAttributeCount)->value();
						string sExtension = pGenerator->first_attribute(sAttributeExtension)->value();

						// Generate and add members
						const int iCount = stoi(sCount);
						for(int i = 0; i < iCount; ++i)
						{
							tGroup.push_back(sPrefix + to_string(i) + "." + sExtension);
						}
					} 
				}

				m_tGroups.push_back(tGroup);
			}
		}

		void getMinMaxResults(float& o_fMin, float& o_fMax)
		{
			o_fMin = FLT_MAX;
			o_fMax = -FLT_MAX;
			for(CGroup& tGroup : m_tGroups)
			{
				float fGroupMin;
				float fGroupMax;
				tGroup.getMinMaxResults(fGroupMin, fGroupMax);

				if(fGroupMax > o_fMax)
				{
					o_fMax = fGroupMax;
				}

				if(fGroupMin < o_fMin)
				{
					o_fMin = fGroupMin;
				}
			}
		}
	};

	struct CTester
	{
		CTimer m_tTimer;

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
			int iSuccessful = 0;
			for(unsigned int i = 0; i < m_tConfig.m_tGroups.size(); ++i)
			{
				CGroup& tGroup = m_tConfig.m_tGroups[i];
				if(testGroup(tGroup))
				{
					++iSuccessful;
				}
			}

			// Print results
			fprintf(m_pResultsOutFile, "... Summary ...\n");
			fprintf(m_pResultsOutFile, "\tCongruence Rate: %.0f%%\n\n\n", 100.f * ((float)iSuccessful / (float)m_tConfig.m_tGroups.size()));

			if (iSuccessful != m_tConfig.m_tGroups.size())
			{
				// Print the incongruent groups first
				fprintf(m_pResultsOutFile, "Incongruent Results:\n\n");
				for_each(m_tConfig.m_tGroups.begin(), m_tConfig.m_tGroups.end(), [this] (CGroup& tGroup) { if(!tGroup.m_bFullyCongruent) tGroup.printResults(m_pResultsOutFile); });

				// Finally, the congruent ones
				fprintf(m_pResultsOutFile, "Congruent Results:\n\n");
				for_each(m_tConfig.m_tGroups.begin(), m_tConfig.m_tGroups.end(), [this] (CGroup& tGroup) { if(tGroup.m_bFullyCongruent) tGroup.printResults(m_pResultsOutFile); }); 
			}
			else
			{
				for_each(m_tConfig.m_tGroups.begin(), m_tConfig.m_tGroups.end(), [this] (CGroup& tGroup) { tGroup.printResults(m_pResultsOutFile); }); 
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

		// Returns true if all members match
		bool testGroup(CGroup& i_tGroup)
		{
			// Create the trait handlers
			CTraitHandler_DimensionAverages ithDimensionAverages;
			CTraitHandler_ColorFrequency ithColorFrequency;
			CTraitHandler_Grabcut ithGrabcut;
			CTraitHandler_Aspect ithAspect;
			CTraitHandler_CannyCount ithCannyCount;

			for(string sMemberName : i_tGroup)
			{
				const bool bDisplayResultImages = i_tGroup.m_tFlags.get(CGroup::EFlag_Display);

				if(CTraitHandler::setProcessingImage(sMemberName) < 0)
				{
					assertbr(0, "Error: Could not load image: " + sMemberName + "");
					continue;
				}

				string sImageFilename = sMemberName.substr(sMemberName.rfind('/') + 1);
				sImageFilename = sImageFilename.substr(0, sImageFilename.rfind('.'));

				if (bDisplayResultImages)
				{
					displayImage(sImageFilename + ": Original Image", CTraitHandler::s_tImage);
					displayImage(sImageFilename + ": Color Snapped", CTraitHandler::getColorSnapped());
					waitKey(0); 
				}

				// Apply the trait handlers to build the Image ID

				// Time how long it will take
				m_tTimer.start();

				CIDData tImgID;
				printf("%s...\n", sImageFilename.c_str());

				//ithGrabcut.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				//m_tTimer.print_lap("\tGrabcut: ");

				//ithDimensionAverages.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				//m_tTimer.print_lap("\tDimension Averages: ");
				
				//ithColorFrequency.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				//m_tTimer.print_lap("\tColor Frequency: ");

				//ithAspect.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				//m_tTimer.print_lap("\tAspect: ");

				ithCannyCount.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				m_tTimer.print_lap("\tCanny Count: ");

				i_tGroup.addResult(sImageFilename, tImgID);
			}

			i_tGroup.m_bFullyCongruent  = i_tGroup.m_tResults.size() == 1;
			return i_tGroup.m_bFullyCongruent;
		}
	};

	int CGroup::s_iUnnamedGroupCounter = 0;
}
