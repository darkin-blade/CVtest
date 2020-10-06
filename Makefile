C_FLAGS  = -g
CV_FLAGS = pkg-config --cflags --libs opencv4
VL_FLAGS = -I$(VLFEAT_DIR) -L$(VLFEAT_DIR)/bin/glnxa64/ -lvl

cv:
	g++ $(par).cpp -o test $(C_FLAGS) `$(CV_FLAGS)`

vl:
	g++ $(par).cpp -o test $(C_FLAGS) `$(CV_FLAGS)` $(VL_FLAGS)

test:
	echo "shit"
	./test --mode panorama ./boat*
