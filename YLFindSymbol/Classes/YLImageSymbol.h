//
//  YLImageSymbol.h


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

#if DEBUG

@interface YLImageSymbol : NSObject

typedef const void *YLImageRef;

/// Get beginning address of a loaded image.
+ (YLImageRef)imageByName:(const char *)file;

/// Enumerate loaded images. In handler, return NO to stop.
+ (void)enumerateImages:(BOOL(^)(YLImageRef image, NSString *path))handler;

/**
 Find function pointer address of a symbol in the loaded image. You can get static function's address which not supported by dlsym().
 @note
 Not all static functions can be found, because its symbol may be striped in the binary file, e.g. those `<redacted>` in system frameworks.
 
 @param image The image to search in, pass NULL to search in all images.
 @param symbolName The symbol to find. Need to add `_` when finding a C function name.
 @return Address of the symbol, NULL when symbol was not found.
 */
+ (void *)findSymbolInImage:(_Nullable YLImageRef)image name:(const char *)symbolName;

/**
 Find function pointer address of a symbol in the loaded image. You can get static function's address which not supported by dlsym().
 @note
 Not all static functions can be found, because its symbol may be striped in the binary file, e.g. those `<redacted>` in system frameworks.
 
 @param image The image to search in, pass NULL to search in all images.
 @param matchingBlock The block to check the symbol name, return true if the name is matched.
 @return Address of the symbol, NULL when symbol was not found.
 */
+ (void *)findSymbolInImage:(_Nullable YLImageRef)image matching:(BOOL(^)(const char *symbolName))matchingBlock;

/// Get symbol of a address.
+ (nullable NSString *)symbolNameForAddress:(void *)address;

/// Get image file path of a address.
+ (nullable NSString *)imagePathForAddress:(void *)address;
@end

#endif

NS_ASSUME_NONNULL_END
