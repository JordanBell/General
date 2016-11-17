#include <stdio.h>
#include <string>
#include <vector>
#include "CImg.h"
#include "CTraitHandler.h"
#include "CTraitHandler_DimensionAverages.h"

int main(int argc, char** argv)
{
  cimg::info();

  CTraitHandler::setProcessingImage("hair2.bmp");

  CImgDisplay main_disp(*CTraitHandler::s_pImage,"Testing!");
  main_disp.resize(CTraitHandler::s_pImage->width() * 8, CTraitHandler::s_pImage->height() * 8);

  // Start the trait handlers
  CTraitHandler_DimensionAverages hda;
  hda.handle();

  while (!main_disp.is_closed())
  {
    main_disp.wait();
  }

  return 0;
}
