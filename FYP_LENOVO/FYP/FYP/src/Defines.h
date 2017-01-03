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

#define SAVE_INTO_LOCAL_FOLDER

#define MAGNIFY_FACTOR 8.f

// Color snap:
#define CS_SEGMENT_SIZE 16
//#define CS_BLUR_SIZE 3

#define RADIAL_WEIGHTING_EXPONENT 3
#define RADIAL_WEIGHTING_MIN 0.1f

// Functions
#define ENSURE_DIR(i_sFilename) { int ensDirRes = ensureDirectory(i_sFilename); if(ensDirRes) return ensDirRes; }