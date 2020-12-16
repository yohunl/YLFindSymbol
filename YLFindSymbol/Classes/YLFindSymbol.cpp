//
//  YLFindSymbol.cpp


//参考 https://github.com/r-plus/substrate/blob/master/DarwinFindSymbol.cpp

#if DEBUG

#include "YLFindSymbol.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <mach/mach.h>
#include <mach/mach_init.h>

#include <mach-o/dyld.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>

extern "C" {
#include <mach-o/nlist.h>
}

#define BSD_KERNEL_PRIVATE
#include <machine/types.h>
#include <sys/sysctl.h>


#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#define _trace() do { \
MSLog(MSLogLevelNotice, "_trace(%u)", __LINE__); \
} while (false)


struct MSSymbolData {
    const char *name_;
    uint8_t type_;
    uint8_t sect_;
    int16_t desc_;
    uintptr_t value_;
};

#ifdef __LP64__
typedef struct mach_header_64 mach_header_xx;
typedef struct nlist_64 nlist_xx;
typedef struct segment_command_64 segment_command_xx;

static const uint32_t LC_SEGMENT_XX = LC_SEGMENT_64;
static const uint32_t MH_MAGIC_XX = MH_MAGIC_64;
#else
typedef struct mach_header mach_header_xx;
typedef struct nlist nlist_xx;
typedef struct segment_command segment_command_xx;

static const uint32_t LC_SEGMENT_XX = LC_SEGMENT;
static const uint32_t MH_MAGIC_XX = MH_MAGIC;
#endif

static ssize_t MSMachONameList_(const void *stuff, struct MSSymbolData *list, size_t nreq, bool(^matching)(const char *)) {
    size_t slide(-1);//查找image在内存中的偏移地址
    for (uint32_t imageIndex(0), images(_dyld_image_count()); imageIndex != images; ++imageIndex){
        if (_dyld_get_image_header(imageIndex) == stuff) {
            slide = _dyld_get_image_vmaddr_slide(imageIndex);
            break;
        }
    }
    if (slide == -1) {
        return -1;
    }
    
    const mach_header_xx *machHeader(reinterpret_cast<const mach_header_xx *>(stuff));//mach header 指针

    

    //find load command of LC_SYMTAB
    const nlist_xx *symbols;
    const char *strings;
    size_t n = 0;
    
    if (machHeader->magic != MH_MAGIC_XX) {
        return -1;
    }
    
       
    const struct load_command *load_commands(reinterpret_cast<const struct load_command *>(machHeader + 1));//接着mach_header_xx的就是load_command了
        
    const struct symtab_command *symtab_command(NULL);//loadcommand中的LC_SYMTAB
    const segment_command_xx *linkedit_segment(NULL);//segmengt中的linkedit;
    const struct load_command *lcp;
    lcp = load_commands;
    
    
    
    //find symbol table and string table from __LINKEDIT segment command
    symbols = NULL;
    strings = NULL;
    
    for (uint32_t i(0); i != machHeader->ncmds; ++i) {
       if (lcp->cmd == LC_SYMTAB) {
           symtab_command = reinterpret_cast<const struct symtab_command *>(lcp);
        }
        else if (lcp->cmd == LC_SEGMENT_XX) {//找到cmd是LC_SEGMENT
            const segment_command_xx *segment(reinterpret_cast<const segment_command_xx *>(lcp));
            if (strcmp(segment->segname, SEG_LINKEDIT) == 0) {//找到 segname是 SEG_LINKEDIT的
                linkedit_segment = reinterpret_cast<const segment_command_xx *>(lcp);
            }
        }
        //每个load command,都有个cmdsize,标识本load command占多大空间
        lcp = reinterpret_cast<const struct load_command *>(reinterpret_cast<const uint8_t *>(lcp) + lcp->cmdsize);
    }
    if (!symtab_command || !linkedit_segment ) {
      return -1;
    }
    
    
    uintptr_t linkedit_base = (uintptr_t)slide + linkedit_segment->vmaddr - linkedit_segment->fileoff;
    symbols = reinterpret_cast<const nlist_xx *>(symtab_command->symoff + linkedit_base);
    strings = reinterpret_cast<const char *>(symtab_command->stroff + linkedit_base);
    
    if (symbols == NULL || strings == NULL){return -1;};
    
    
    
    //find symbols with names
    size_t result(nreq);
    n = symtab_command->nsyms;//number of symbol table entries
    for (size_t m(0); m != n; ++m) {
        const nlist_xx *q(&symbols[m]);//符号表的每一项是一个nlist_xx ,其中n_value就是方法的入口
        if (q->n_un.n_strx == 0 || (q->n_type & N_STAB) != 0)
            continue;
        
        //name of the symbol in string table
        const char *nambuf(strings + q->n_un.n_strx);//从symbol表中,得到string table的索引,从而拿到符号的字符串名称
        //fprintf(stderr, " == %s\n", nambuf);
        
        for (size_t item(0); item != nreq; ++item) {
            struct MSSymbolData *p(list + item);//list + item是指针操作
            if (matching) {
                if (!matching(nambuf)) {
                    continue;
                }
            } else {
                if (p->name_ == NULL) {
                    continue;
                }
                if (strcmp(p->name_, nambuf) != 0) {
                    continue;
                }
            }
            
            p->name_ = NULL;
            
            p->value_ = q->n_value;
            if (p->value_ != 0)
                p->value_ += slide;
            
            p->type_ = q->n_type;
            p->desc_ = q->n_desc;
            p->sect_ = q->n_sect;
            
            if (--result == 0)
                return 0;
            break;
        }
    }
    
    return result;
}

static bool string_ends_with(const char * str, const char * suffix) {
    if (str == NULL || suffix == NULL) {
        return false;
    }
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    bool flag = (str_len >= suffix_len) && (0 == strcmp(str + (str_len-suffix_len), suffix));
    return flag;
    
}

// 获取某个以 file结尾的image,内部是_dyld_get_image_name遍历
YLImageRef YLGetImageByName(const char *file) {
    for (uint32_t image(0), images(_dyld_image_count()); image != images; ++image)
        if (string_ends_with(_dyld_get_image_name(image), file)) {
            return _dyld_get_image_header(image);
        }
            
    return NULL;
}

static void YLFindSymbols(YLImageRef image, size_t count, const char *names[], void *values[], bool(^matching)(const char *)) {
    MSSymbolData items[count];
    
    //构建一个要查找的符号数组
    for (size_t index(0); index != count; ++index) {
        MSSymbolData &item(items[index]);
        if (names) {
            item.name_ = names[index];
        }
        item.type_ = 0;
        item.sect_ = 0;
        item.desc_ = 0;
        item.value_ = 0;
    }
    
    if (image != NULL)
        MSMachONameList_(image, items, count, matching);
    else {
        size_t remain(count);
        
        for (uint32_t image(0), images(_dyld_image_count()); image != images; ++image) {
            //fprintf(stderr, ":: %s\n", _dyld_get_image_name(image));
            
            ssize_t result(MSMachONameList_(_dyld_get_image_header(image), items, count, matching));
            if (result == -1)
                continue;
            
            // XXX: maybe avoid this happening at all? a flag to NSMachONameList_?
            for (size_t index(0); index != count; ++index) {
                MSSymbolData &item(items[index]);
                if (item.name_ == NULL && item.value_ == 0) {
                    ++result;
                    if (names) {
                        item.name_ = names[index];
                    }
                }
            }
            
            remain -= count - result;
            if (remain == 0)
                break;
        }
    }
    
    for (size_t index(0); index != count; ++index) {
        MSSymbolData &item(items[index]);
        uintptr_t value(item.value_);
#ifdef __arm__
        if ((item.desc_ & N_ARM_THUMB_DEF) != 0)
            value |= 0x00000001;
#endif
        values[index] = reinterpret_cast<void *>(value);
    }
}

void *YLFindSymbol(YLImageRef image, const char *name) {
    void *value;
    YLFindSymbols(image, 1, &name, &value, NULL);
    return value;
}

void *YLFindSymbolBlock(YLImageRef image, bool(^matchingBlock)(const char *)) {
    void *value;
    YLFindSymbols(image, 1, NULL, &value, matchingBlock);
    return value;
}

const char *YLSymbolNameForAddress(void *address) {
    Dl_info dlinfo;
    dladdr(address, &dlinfo);
    return dlinfo.dli_sname;
}

const char *YLImagePathForAddress(void *address) {
    Dl_info dlinfo;
    dladdr(address, &dlinfo);
    return dlinfo.dli_fname;
}

#endif
