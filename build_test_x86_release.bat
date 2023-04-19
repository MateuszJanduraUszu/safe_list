:: build_test_x64_release.bat

:: Copyright (c) Mateusz Jandura. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0

mkdir build
cd build
cmake -A Win32 -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" ..