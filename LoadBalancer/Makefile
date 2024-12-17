.PHONY: build clean

build:
	make clean
	gcc src/main.c -g -o ./build/load_balancer

clean:
	find build -type f -not -name '.gitkeep' -exec rm {} +
