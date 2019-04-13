
all : jhash

jhash: jhash.cc
	c++ -std=c++11 jhash.cc -o jhash
.PHONY: jhash

clean:
	rm jhash
