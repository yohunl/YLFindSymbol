//
//  YLViewController.m
//  YLFindSymbol
//
//  Created by yohunl on 12/16/2020.
//  Copyright (c) 2020 yohunl. All rights reserved.
//

#import "YLViewController.h"
#import <YLFindSymbol/YLImageSymbol.h>
#include <dlfcn.h>
@interface YLViewController ()

@end

@implementation YLViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = @"测试";
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc]initWithTitle:@"测试" style:UIBarButtonItemStyleDone target:self action:@selector(rightAction:)];
}

- (void)rightAction:(id)sender{
    NSString* funNameStr = @"dlsym";
    const char *funNameStrP = [funNameStr cStringUsingEncoding:NSUTF8StringEncoding];
    YLImageRef image1 = [YLImageSymbol imageByName:"/usr/lib/system/libdyld.dylib"];
    NSLog(@"/usr/lib/system/libdyld.dylib这个镜像的地址是%p",image1);
    void *libdyld = dlopen("/usr/lib/system/libdyld.dylib", RTLD_LAZY);
    void *dlsymFUn = dlsym(libdyld,funNameStrP);
    void *fundlsym = [YLImageSymbol findSymbolInImage:nil name:"_dlsym"];
    NSAssert(dlsymFUn == fundlsym, @"查找的地址不对");
    
    //查找某个符号的的入口地址
    NSString *name1 = [YLImageSymbol symbolNameForAddress:fundlsym];
    NSLog(@"%p方法的名字是 = %@",fundlsym,name1);
    void *funna = (void *)dlsym;
    NSString *name2 = [YLImageSymbol imagePathForAddress:funna];
    NSLog(@"dlsym方法所在的image是 = %@",name2);
    
    void *fun1 = [YLImageSymbol findSymbolInImage:nil name:"_dlsym"];
    NSLog(@"_dlsym方法的地址是 = %p",fun1);
}
@end
