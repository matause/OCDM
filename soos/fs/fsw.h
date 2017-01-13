#include <3ds.h>


Result fsw_init_bin(FS_ArchiveID arch, FS_MediaType mediatype, u64 stuff, char* r);
void fsw_free();
FS_Archive fsw_get();
Result fsw_read2buf(const char* path, u64* size, char** bufptr);
Result fsw_scandir(const char* path, char argv[][262], u32* argc, char* ext);
Result fsw_scandirent(const char* path, FS_DirectoryEntry* argv, u32* argc);
Result fsw_write2buf(const char* path, u32 size, char* bufptr);
