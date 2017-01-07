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

// ID similarity allowances
#define ALLOWANCE_COLOR_FREQUENCY 0.25f
#define ALLOWANCE_COLOR_FREQUENCY_MATCHES 2
#define ALLOWANCE_DIMENSION_COLOR_COUNT 2.f
#define ALLOWANCE_WIDTH_HEIGHT_DIFF 10.f
#define ALLOWANCE_TOTAL_MATCHES 1

// Channel Delta Count threshold (used in dimension averages)
#define CDC_THRESHOLD 10

// Functions
#define ENSURE_DIR(i_sFilename) { int ensDirRes = ensureDirectory(i_sFilename); if(ensDirRes) return ensDirRes; }

// Testing
#define CONFIG_FILEPATH "testConfig - all.xml"
//#define CONFIG_FILEPATH "testConfig - all.xml"
#define OUTPUT_LOG_FILENAME "results.txt"