cmake -DDEBUG_MODE=OFF -DTESTS=ON -B test_build -S .
cmake --build test_build -j $(nproc) --clean-first