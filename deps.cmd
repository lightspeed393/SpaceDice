set curdir=%~dp0
set curdir=%curdir:~0,-1%

:compile_curl
rem 1. curl

mkdir %curdir%\depends
cd %curdir%\depends

rem https://github.com/alexa/avs-device-sdk/wiki/Optimize-libcurl

git clone https://github.com/curl/curl
mkdir curl-build
cd curl-build
rem cmake -G "Visual Studio 14 2015 Win64" ../curl -DCURL_STATICLIB:BOOL=ON -DCURL_STATIC_CRT:BOOL=ON -DHTTP_ONLY:BOOL=ON -DENABLE_IPV6:BOOL=OFF -DCMAKE_USE_WINSSL:BOOL=ON -DCMAKE_INSTALL_PREFIX=%curdir%
cmake -G "Visual Studio 14 2015 Win64" ../curl -DCURL_STATICLIB:BOOL=ON -DCURL_STATIC_CRT:BOOL=ON -DHTTP_ONLY:BOOL=ON -DENABLE_IPV6:BOOL=OFF -DCMAKE_INSTALL_PREFIX=%curdir%
cmake -G "Visual Studio 14 2015 Win64" ../curl -LA > %curdir%\curl_options.txt
rem cmake --build . --config Release --target libcurl 
rem cmake -P cmake_install.cmake 
cmake --build . --config Release --target INSTALL

rem make -j$(nproc)
rem make install
cd %curdir%

:compile_jansson

rem 2. libjansson (http://www.digip.org/jansson/)

mkdir  %curdir%\depends
cd %curdir%\depends
git clone https://github.com/akheron/jansson
mkdir jansson-build
cd jansson-build
cmake -G "Visual Studio 14 2015 Win64" ../jansson -DCMAKE_INSTALL_PREFIX=%curdir% -DJANSSON_BUILD_SHARED_LIBS:BOOL=OFF -DJANSSON_EXAMPLES:BOOL=OFF -DJANSSON_BUILD_DOCS:BOOL=OFF
rem cmake ../jansson -LA > $curdir/options.txt
rem make -j$(nproc)
rem make install
cmake --build . --config Release --target INSTALL
cd %curdir%
