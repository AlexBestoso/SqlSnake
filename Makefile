all:
	g++ ./main.cc $(shell mysql_config --cflags) -o ./sqlTest $(shell mysql_config --libs)
clean:
	rm ./sqlTest
install:
	sudo apt-get install libmysql++-dev -y
