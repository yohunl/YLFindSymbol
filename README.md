# YLFindSymbol
在调用一些方法时候,需要使用dlopen,dlsym,但是苹果在审核的时候对使用这两个方法的应用,常常会审核不通过,所以,这里可以通过实现一个查找符号的方式,找到符号dlopen,dlsym,然后调用

参考自[DarwinFindSymbol](https://github.com/r-plus/substrate/blob/master/DarwinFindSymbol.cpp) 早期的cydia substrate的源码


## Example

To run the example project, clone the repo, and run `pod install` from the Example directory first.

## Requirements

## Installation

YLFindSymbol is available through [CocoaPods](https://cocoapods.org). To install
it, simply add the following line to your Podfile:

```ruby
pod 'YLFindSymbol'
```

## Author

yohunl, yohunl@163.com

## License

YLFindSymbol is available under the MIT license. See the LICENSE file for more info.
