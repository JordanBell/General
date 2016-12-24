// This file will hold #define values that will configure the entire project

#define TEST_IMAGES_DIR "TestImages/"
#define MAGNIFY_FACTOR 8.f

// Functions
#define ENSURE_DIR(i_sFilename) { int ensDirRes = ensureDirectory(i_sFilename); if(ensDirRes) return ensDirRes; }