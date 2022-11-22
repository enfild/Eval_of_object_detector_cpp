## Req:
- gcc >= 7.0
- cmake >= 3.10
- c++ 14

## Build:
```
mkdir build
cd build
cmake ..
make
```

## Example for start:
```
cd build
./eval_detector "/home/pavel/Eval_of_object_detector_c++/data/GT.txt" /home/pavel/Eval_of_object_detector_c++/data/DT.txt 0.5
./eval_detector "/home/pavel/Eval_of_object_detector_c++/data/GT.txt" /home/pavel/Eval_of_object_detector_c++/data/DT.txt 0.95
```