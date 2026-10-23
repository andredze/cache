cmake -DDEBUG_MODE=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DTESTS=ON -B test_build -S .
cmake --build test_build -j $(nproc) --clean-first