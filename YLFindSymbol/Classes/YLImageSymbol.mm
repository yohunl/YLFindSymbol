//
//  YLImageSymbol.m


#if DEBUG

#import "YLImageSymbol.h"
#import "YLFindSymbol.h"
#import <mach-o/dyld.h>

@implementation YLImageSymbol

+ (YLImageRef)imageByName:(const char *)file {
    YLImageRef image = YLGetImageByName(file);
    return image;
}

+ (void)enumerateImages:(BOOL(^)(YLImageRef image, NSString *path))handler {
    if (handler == nil) {
        return;
    }
    uint32_t images = _dyld_image_count();
    for (uint32_t image = 0; image != images; ++image) {
        NSString *path = [NSString stringWithUTF8String:_dyld_get_image_name(image)];
        BOOL result = handler(_dyld_get_image_header(image), path);
        if (result == NO) {
            break;
        }
    }
}

+ (void *)findSymbolInImage:(YLImageRef)image name:(const char *)symbolName {
    NSParameterAssert(symbolName);
    void *symbol = YLFindSymbol(image, symbolName);
    return symbol;
}

+ (void *)findSymbolInImage:(YLImageRef)image matching:(BOOL(^)(const char *symbolName))matchingBlock {
    NSParameterAssert(matchingBlock);
    void *symbol = YLFindSymbolBlock(image, ^bool(const char *symbolName) {
        return matchingBlock(symbolName);
    });
    return symbol;
}

+ (nullable NSString *)symbolNameForAddress:(void *)address {
    if (address == NULL) {
        return nil;
    }
    const char *name = YLSymbolNameForAddress(address);
    if (name == NULL) {
        return nil;
    }
    if (strlen(name) == 0) {
        return nil;
    }
    return [NSString stringWithUTF8String:name];
}

+ (nullable NSString *)imagePathForAddress:(void *)address {
    if (address == NULL) {
        return nil;
    }
    const char *imageFile = YLImagePathForAddress(address);
    if (imageFile == NULL) {
        return nil;
    }
    if (strlen(imageFile) == 0) {
        return nil;
    }
    return [NSString stringWithUTF8String:imageFile];
}

@end

#endif
