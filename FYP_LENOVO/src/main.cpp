//#define DISPLAY_RESULTS
#define TEST_IMAGES_DIR "TestImages/"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "CBitset.h"
#include "CImg.h"

#include "CTraitHandler.h"
#include "CTraitHandler_DimensionAverages.h"
#include "NTesting.h"

using namespace NTesting;

int main()
{
  CTester tTester;
  tTester.run();

  return 0;
}
