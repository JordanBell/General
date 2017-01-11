#include "Defines.h"
#ifdef RUN_ITH_EVALUATOR
#pragma warning (disable : 4244)

// Include <>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <chrono>

// Windows Defines
#if defined(_WIN32)
#include <windows.h>
#endif

// Linux Defines
#if defined(__linux__)
#include <sys/stat.h>)
#endif

// RapidXML
#include "../rapidxml/rapidxml.hpp"
#include "../rapidxml/rapidxml_utils.hpp"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

// Pre-headers
#include "Types.h"

// Utilities
#include "CBitset.h"
#include "Histogram.h"
#include "util.h"
#include "CTimer.h"
#include "CFrequencyCounter.h"

// Core
#include "CIDData.h"
#include "CTraitHandler.h"
#include "CTraitHandler_DimensionAverages.h"
#include "CTraitHandler_ColorFrequency.h"
#include "CTraitHandler_Grabcut.h"
#include "CTraitHandler_Aspect.h"
#include "CTraitHandler_OCR.h"
#include "CTraitHandler_CannyCount.h"

// Testing & Verification
#include "NTesting.h"

using namespace cv;
using namespace std;

namespace NTesting
{
	template <class ITH_T>
	struct CTesterITH
	{
		CTimer m_tTimer;

		FILE* m_pResultsOutFile;
		CConfig m_tConfig;
		ITH_T m_tITH;

		void run(const string& i_sITHName)
		{
			printf("%s", i_sITHName.c_str());

			if(!m_tITH.m_bIsFloatBased)
			{
				// No floats for this ITH. 
				printf(": No\n");
				return;
			}

			// Test every group
			printf(" ");
			for(unsigned int i = 0; i < m_tConfig.m_tGroups.size(); ++i)
			{
				printf("%d/%d", i, m_tConfig.m_tGroups.size());

				CGroup& tGroup = m_tConfig.m_tGroups[i];
				testGroup(tGroup);
				int numDigitsFirst = i / 10;
				int numDigitsSecond = m_tConfig.m_tGroups.size() / 10;
				// WARNING: Works on assumption of two-digit numbers only
				printf("\b");
				printf("\b");
				printf("\b");
				if(numDigitsFirst > 0)
				{
					printf("\b");
				}
				if(numDigitsSecond > 0)
				{
					printf("\b");
				}
			}

			float fMin;
			float fMax;
			m_tConfig.getMinMaxResults(fMin, fMax);

			if(fMin == FLT_MAX || fMax == -FLT_MAX)
			{
				// No floats for this ITH. 
				printf(": Error: No Min/Max results extracted.\n");
				return;
			}
			printf(": Yes\n");

			// Create the graph
			const int k_iGraphW = 750;
			const int k_iGraphH = 750;
			const int k_iGraphMarginY = 50;
			const int k_iGraphMarginX = 50;
			const int k_iGraphMarginX_Right = 300;
			const int k_iPlotRadius = 1;
			const int k_iDotTickness = 5;
			Mat tITHGraph(k_iGraphMarginY * 2 + k_iGraphH, k_iGraphMarginX * 2 + k_iGraphW + k_iGraphMarginX_Right, CV_8UC4);
			for_each(tITHGraph.begin<Vec4b>(), tITHGraph.end<Vec4b>(), [] (Vec4b& o_v) { o_v[0] = 255; o_v[1] = 255; o_v[2] = 255; });
			RNG tRng;

			float fITHMaxSD = -FLT_MAX;
			float fITHMaxRange = -FLT_MAX;
			string sITHMaxSDGroup;
			string sITHMaxRangeGroup;

			// Line: Bottom of graph
			line(tITHGraph,
				Point(k_iGraphMarginX, k_iGraphMarginY),
				Point(k_iGraphMarginX + k_iGraphW, k_iGraphMarginY), 
				cvScalar(0, 0, 0), 2);

			// Line: Left of graph
			line(tITHGraph,
				Point(k_iGraphMarginX, k_iGraphMarginY),
				Point(k_iGraphMarginX, k_iGraphMarginY + k_iGraphH), 
				cvScalar(0, 0, 0), 2);

			// Add the min and max values as text
			Point tPosMin(k_iGraphMarginX, k_iGraphMarginY - 5);
			Point tPosMax(k_iGraphMarginX + k_iGraphW, k_iGraphMarginY - 5);

			char sMin[32];
			sprintf_s(sMin, "MIN: %g", fMin);
			putText(tITHGraph, sMin, tPosMin, 0,
				0.5, cvScalar(0, 0, 0));

			char sMax[32];
			sprintf_s(sMax, "MAX: %g", fMax);
			putText(tITHGraph, sMax, tPosMax, 0,
				0.5, cvScalar(0, 0, 0));

			// Output results to graph
			const int iGroupYPosMargin = 10;
			const int iGroupYStep = (k_iGraphH - iGroupYPosMargin * 2) / m_tConfig.m_tGroups.size();
			int iGroupNum = 0;
			for(CGroup& tGroup : m_tConfig.m_tGroups)
			{
				float fGroupMin;
				float fGroupMax;
				tGroup.getMinMaxResults(fGroupMin, fGroupMax);
				float fGroupRange = fGroupMax - fGroupMin;

				iGroupNum++;
				int iGroupY = tRng.uniform(k_iGraphMarginY, k_iGraphMarginY + k_iGraphH);
				Vec3b tGroupColor((char)tRng.uniform(0, 255), 127, 255); // Random hue
				cvtColor(tGroupColor, CV_HLS2BGR);

				const float fYPos = k_iGraphMarginY + iGroupYPosMargin + (iGroupNum * iGroupYStep);

				// Line: Group Line
				line(tITHGraph,
					Point(k_iGraphMarginX, fYPos),
					Point(k_iGraphMarginX + k_iGraphW, fYPos), 
					cvScalar(220, 220, 220), 1);

				for(const CGroup::CResultGroup& tResultGroup : tGroup.m_tResults)
				{
					for(unsigned int i = 0; i < tResultGroup.m_tImgIDs.size(); ++i)
					{
						const CIDData& tImageID = tResultGroup.m_tImgIDs[i];

						// Plot that image's floats on the graph
						for(auto& tMem : tImageID.m_tFloats)
						{
							const float fXPos = k_iGraphMarginX + (((tMem.m_tValue - fMin) / (fMax - fMin)) * k_iGraphW);
							circle(tITHGraph, 
								Point(fXPos, fYPos), 
								k_iPlotRadius, 
								cvScalar(tGroupColor[0], tGroupColor[1], tGroupColor[2]),
								k_iDotTickness);
						}
					}
				}

				// Line: Group Line Range Highlight
				const float fXPosRangeLineStart = k_iGraphMarginX + (((fGroupMin - fMin) / (fMax - fMin)) * k_iGraphW);
				const float fXPosRangeLineEnd = k_iGraphMarginX + (((fGroupMax - fMin) / (fMax - fMin)) * k_iGraphW);
				line(tITHGraph,
					Point(fXPosRangeLineStart, fYPos),
					Point(fXPosRangeLineEnd, fYPos), 
					cvScalar(tGroupColor[0], tGroupColor[1], tGroupColor[2]), 2);

				// Put the group name and their range at the end of the line
				char sRangeLabel[128];

				float fStandardDeviation = tGroup.getStandardDeviation();

				if(fStandardDeviation > fITHMaxSD)
				{
					fITHMaxSD = fStandardDeviation;
					sITHMaxSDGroup = tGroup.m_sName;
				}

				if(fGroupRange > fITHMaxRange)
				{
					fITHMaxSD = fGroupRange;
					sITHMaxRangeGroup = tGroup.m_sName;
				}

				sprintf_s(sRangeLabel, "%s: [%g to %g] (%g)", tGroup.m_sName.c_str(), fGroupMin, fGroupMax, fGroupRange);
				putText(tITHGraph, 
					sRangeLabel, 
					Point(k_iGraphMarginX + k_iGraphW + 5, fYPos + 5),
					0,
					0.5, 
					cvScalar(0, 0, 0));
			}

#if defined(ITH_SHOW_GRAPHS)
			displayImage("ITH Graph: " + i_sITHName, tITHGraph, 1.f, 1.f);
			waitKey(0);  
#endif 

			// Build up the log file
			// Open the output file in the target directory
			string sResultsDirectory = ITH_RESULTS_DIR + i_sITHName + "/";
			ensureDirectory(sResultsDirectory);
			string sResultsTxt = sResultsDirectory + "report.txt";
			string sResultsGraph = sResultsDirectory + "graph.bmp";
			errno_t err = fopen_s(&m_pResultsOutFile, sResultsTxt.c_str(), "w");
			assertbr(err == 0, "Failed to open file: " + sResultsTxt);

			// Add the summary to the log file
			fprintf(m_pResultsOutFile, "Summary\n\tMax SD: Group %s = %g\n\tMax Range: Group %s = %g\n\n\n", sITHMaxSDGroup.c_str(), fITHMaxSD, sITHMaxRangeGroup.c_str(), fITHMaxRange);

			for(CGroup& tGroup : m_tConfig.m_tGroups)
			{
				float fGroupMin;
				float fGroupMax;
				tGroup.getMinMaxResults(fGroupMin, fGroupMax);
				float fGroupRange = fGroupMax - fGroupMin;

				float fStandardDeviation = tGroup.getStandardDeviation();

				fprintf(m_pResultsOutFile, "Group: %s\n\tMin: %g\n\tMax: %g\n\tRange: %g\n\tStd Dev: %g\n\n", tGroup.m_sName.c_str(), fGroupMin, fGroupMax, fGroupRange, fStandardDeviation);
			}

			fclose(m_pResultsOutFile);
			imwrite(sResultsGraph, tITHGraph);
		}

		// Returns true if all members match
		void testGroup(CGroup& i_tGroup)
		{
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

				// Apply this ITH to the image
				CIDData tImgID;
				m_tITH.evaluate(tImgID, sImageFilename, bDisplayResultImages);
				i_tGroup.addResult(sImageFilename, tImgID);
			}
		}
	};
}

int main(int argc, char** argv )
{
	ensureDirectory(ITH_RESULTS_DIR);

	NTesting::CTesterITH<CTraitHandler_Aspect>().run("Aspect");
	NTesting::CTesterITH<CTraitHandler_CannyCount>().run("Canny Count");
	NTesting::CTesterITH<CTraitHandler_ColorFrequency>().run("Color Frequency");
	NTesting::CTesterITH<CTraitHandler_DimensionAverages>().run("Dimension Averages");
	NTesting::CTesterITH<CTraitHandler_Grabcut>().run("Grabcut");
	NTesting::CTesterITH<CTraitHandler_OCR>().run("OCR");

	printf("Done.\n");
	char c;
	cin >> c;

	return 0;
}

#endif // RUN_ITH_EVALUATOR
