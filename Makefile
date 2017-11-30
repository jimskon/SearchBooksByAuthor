#MakeFile to build and deploy SearchBooks
# For CSC3004 Software Development

# Put your user name below:
USER= skon

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -std=c++11

RM= /bin/rm -f

all: testSearch SearchBooksByAuthor DisplayBook PutCGI PutHTML

SearchBooksByAuthor.o: SearchBooksByAuthor.cpp 
	$(CC) -c $(CFLAGS) SearchBooksByAuthor.cpp

SearchBooksByAuthor: SearchBooksByAuthor.o
	$(CC) $(CFLAGS) SearchBooksByAuthor.o -o SearchBooksByAuthor -L/usr/local/lib -lcgicc

DisplayBook.o: DisplayBook.cpp 
	$(CC) -c $(CFLAGS) DisplayBook.cpp

DisplayBook: DisplayBook.o
	$(CC) $(CFLAGS) DisplayBook.o -o DisplayBook -L/usr/local/lib -lcgicc

testSearch.o: testSearch.cpp 
	$(CC) $(CFLAGS) -c testSearch.cpp 

testSearch: testSearch.o 
	$(CC) $(CFLAGS) testSearch.o -o testSearch 

PutCGI: SearchBooksByAuthor
	chmod 757 SearchBooksByAuthor DisplayBook
	cp SearchBooksByAuthor /usr/lib/cgi-bin/$(USER)_SearchBooksByAuthor.cgi 
	cp DisplayBook /usr/lib/cgi-bin/$(USER)_DisplayBook.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp SearchBooksByAuthor.html /var/www/html/class/softdev/$(USER)/Books/
	cp SearchBooksByAuthor.js /var/www/html/class/softdev/$(USER)/Books/
	cp SearchBooksByAuthor.css /var/www/html/class/softdev/$(USER)/Books/
	cp gears.gif /var/www/html/class/softdev/$(USER)/Books/

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)/Books

clean:
	rm -f *.o SearchBooksByAuthor testSearch DisplayBook
