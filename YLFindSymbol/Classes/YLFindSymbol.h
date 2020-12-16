//
//  YLFindSymbol.hpp



#ifndef YLFindSymbol_hpp
#define YLFindSymbol_hpp

#include <stdio.h>

#if DEBUG

typedef const void *YLImageRef;

/// Get beginning address of a loaded image.
extern YLImageRef YLGetImageByName(const char *file);

/**
 Find function pointer address of a symbol in the loaded image. You can get static function's address which not supported by dlsym().
 @note
 Not all static functions can be found, because its symbol may be striped in the binary file, e.g. those `<redacted>` in system frameworks.
 
 @param image The image to search in, pass NULL to search in all images.
 @param name The symbol to completely match. Need to add `_` when finding a C function name.
 @return Address of the symbol, NULL when symbol was not found.
 */
extern void *YLFindSymbol(YLImageRef image, const char *name);

/**
 Find function pointer address of a symbol in the loaded image. You can get static function's address which not supported by dlsym().
 @note
 Not all static functions can be found, because its symbol may be striped in the binary file, e.g. those `<redacted>` in system frameworks.

 @param image The image to search in, pass NULL to search in all images.
 @param matchingBlock The block to check the symbol name, return true if the name is matched.
 @return Address of the symbol, NULL when symbol was not found.
 */
extern void *YLFindSymbolBlock(YLImageRef image, bool(^matchingBlock)(const char *));

/// Get symbol of a address.
extern const char *YLSymbolNameForAddress(void *address);

/// Get image file path of a address.
extern const char *YLImagePathForAddress(void *address);

#endif
#endif
