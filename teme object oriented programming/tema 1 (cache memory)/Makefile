.PHONY: build clean run
build:
	mkdir classes;
	javac src/*.java -d classes;
	javac -cp classes src/*.java -d classes;
clean:
	rm -r classes
run:
	java -Xmx512m -cp classes Main ${INPUT_PATH} ${OUTPUT_PATH}
