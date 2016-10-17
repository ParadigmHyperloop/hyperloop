all:
	pip install -r requirements.txt

run: clean all
	./server.py

test:
	./test

clean:
	rm -f server.out server.err test-data.txt *.csv
