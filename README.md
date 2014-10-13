foo-bar-executor
================

Модуль для Апача, который при запросе на /foo.bar запускает ls и выводит результат пользователю.


Notes:
- ::boost::process was taken from svn co https://svn.boost.org/svn/boost/sandbox/process/ with a few changes according to boost 1.56
- module was tested with http://skylink.dl.sourceforge.net/project/apachehttp.mirror/httpd-2.2.25-win32-x86-no_ssl.msi
- it is supposed that environment variable APACHE_ROOT points to installation location of httpd-2.2.25 and BOOST_ROOT points to boost 1.56 location
- to view debug log (in error.log) set "LogLevel debug" in httpd.conf
- mod_foo_vc.vcxproj is set to output module into $(APACHE_ROOT)\modules and mod_foo_vc.vcxproj.user forces debugger to run $(APACHE_ROOT)\bin\httpd.exe
- modure registration in httpd.conf:
    LoadModule foo_vc_module modules/mod_foo_vc.dll
    <Location /foo_vc>
        SetHandler foo_vc
    </Location>
