#
# Be sure to run `pod lib lint YLFindSymbol.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see https://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'YLFindSymbol'
  s.version          = '0.1.0'
  s.summary          = 'dlsym的一种实现方式'

# This description is used to generate tags and improve search results.
#   * Think: What does it do? Why did you write it? What is the focus?
#   * Try to keep it short, snappy and to the point.
#   * Write the description between the DESC delimiters below.
#   * Finally, don't worry about the indent, CocoaPods strips it!

  s.description      = <<-DESC
在调用一些方法时候,需要使用dlopen,dlsym,但是苹果在审核的时候对使用这两个方法的应用,常常会审核不通过,所以,这里可以通过实现一个查找符号的方式,找到符号dlopen,dlsym,然后调用
                       DESC

  s.homepage         = 'https://github.com/yohunl/YLFindSymbol'
  # s.screenshots     = 'www.example.com/screenshots_1', 'www.example.com/screenshots_2'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'yohunl' => 'yohunl@163.com' }
  s.source           = { :git => 'https://github.com/yohunl/YLFindSymbol.git', :tag => s.version.to_s }
  # s.social_media_url = 'https://twitter.com/<TWITTER_USERNAME>'

  s.ios.deployment_target = '9.0'

  s.source_files = 'YLFindSymbol/Classes/**/*'
  s.libraries = "z", "c++"
  # s.resource_bundles = {
  #   'YLFindSymbol' => ['YLFindSymbol/Assets/*.png']
  # }

  # s.public_header_files = 'Pod/Classes/**/*.h'
  # s.frameworks = 'UIKit', 'MapKit'
  # s.dependency 'AFNetworking', '~> 2.3'
end
