C_FLAGS  = -g
CV_FLAGS = pkg-config --cflags --libs opencv4
CV3_FLAGS = pkg-config --cflags --libs opencv
VL_FLAGS = -I$(VLFEAT_DIR) -L$(VLFEAT_DIR)/bin/glnxa64/ -lvl

cv:
	g++ $(par) -o test $(C_FLAGS) `$(CV_FLAGS)`

cv3:
	g++ $(par) -o test $(C_FLAGS) `$(CV3_FLAGS)`

vl:
	g++ $(par) -o test $(C_FLAGS) `$(CV_FLAGS)` $(VL_FLAGS)

test:
	echo "shit"
	./test --mode panorama ./boat*
