CPPFLAGS=-std=c++11
OPENCV_LIBS=-lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc
BOOST_LIBS=-lboost_system
CPP_LIBS=-lpthread
LIBS=$(OPENCV_LIBS) $(BOOST_LIBS) $(CPP_LIBS)
LDFLAGS=$(LIBS)

srcs=$(wildcard src/*.cpp)
objs=$(patsubst src/%.cpp,release/%.o,$(srcs))

release/%.o: src/%.cpp
	mkdir -p release
	g++ $(CPPFLAGS) -c $^ -o $@
default: $(objs)
	g++ $(CPPFLAGS) -o SmartFilter $(objs) $(LDFLAGS)
clean:
	rm -rf release/* SmartFilter
all: clean default
