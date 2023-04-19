:: build_test_x64_debug.bat

:: Copyright (c) Mateusz Jandura. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0

mkdir build
cd build
cmake -A x64 -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 17 2022" ..