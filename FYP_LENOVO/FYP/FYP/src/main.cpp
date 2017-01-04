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

// Pre-headers
#include "Types.h"
#include "Defines.h"

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

// Testing & Verification
#include "NTesting.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
	NTesting::CTester tTester;
	tTester.run();
	printf("Done.\n");
	char c;
	cin >> c;

	return 0;
}
