mkdir build-conda
cd build-conda

cmake .. ^
-D CMAKE_BUILD_TYPE:STRING="Release" ^
-D CMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% ^
-D CONDA_BUILD:BOOL=TRUE ^
-G "Ninja" ^
-Wno-dev

ninja install

cd ..
