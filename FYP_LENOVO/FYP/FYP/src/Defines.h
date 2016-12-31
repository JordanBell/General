// This file configures the project as a whole

//#define RAW_INPUT

#ifdef RAW_INPUT
#define COPY_RAW_TO_QUICK // When using raw input, copy over the compressed, pre-processed image to the quick directory
#endif

#define TEST_IMAGES_DIR_RAW "TestImages_Raw/"
#define TEST_IMAGES_DIR_QUICK "TestImages_Quick/"

#ifdef RAW_INPUT
#define TEST_IMAGES_DIR TEST_IMAGES_DIR_RAW
#else
#define TEST_IMAGES_DIR TEST_IMAGES_DIR_QUICK
#endif

#define MAGNIFY_FACTOR 8.f

// Functions
#define ENSURE_DIR(i_sFilename) { int ensDirRes = ensureDirectory(i_sFilename); if(ensDirRes) return ensDirRes; }