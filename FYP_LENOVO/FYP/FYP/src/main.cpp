// Include <>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

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

// Pre-headers
#include "Types.h"
#include "Defines.h"

// Utilities
#include "CBitset.h"
#include "EHue.h"
#include "util.h"

// Core
#include "CTraitHandler.h"
#include "CTraitHandler_DimensionAverages.h"
#include "CTraitHandler_Color.h"

// Testing & Verification
#include "NTesting.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
	NTesting::CTester tTester;
	tTester.run();

	return 0;
}
