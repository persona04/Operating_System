# Makefile örneği

# Derleme için kullanılacak C derleyicisi
CC = gcc

# Derleme seçenekleri
CFLAGS = -Wall -Wextra -std=c11

# Hedef dosyalar
TARGETS = findtopk findtopk_pthread findtopk_mqueue

# Kaynak dosyalar
SOURCES_FINDTOPK = findtopk.c
SOURCES_FINDTOPK_PTHREAD = findtopk_pthread.c
SOURCES_FINDTOPK_MQUEUE = findtopk_mqueue.c

# Nesne dosyalar
OBJECTS_FINDTOPK = $(SOURCES_FINDTOPK:.c=.o)
OBJECTS_FINDTOPK_PTHREAD = $(SOURCES_FINDTOPK_PTHREAD:.c=.o)
OBJECTS_FINDTOPK_MQUEUE = $(SOURCES_FINDTOPK_MQUEUE:.c=.o)

# Derleme kuralları
all: $(TARGETS)

findtopk: $(OBJECTS_FINDTOPK)
	$(CC) $(CFLAGS) -o findtopk $(OBJECTS_FINDTOPK)

findtopk_pthread: $(OBJECTS_FINDTOPK_PTHREAD)
	$(CC) $(CFLAGS) -pthread -o findtopk_pthread $(OBJECTS_FINDTOPK_PTHREAD)

findtopk_mqueue: $(OBJECTS_FINDTOPK_MQUEUE)
	$(CC) $(CFLAGS) -o findtopk_mqueue $(OBJECTS_FINDTOPK_MQUEUE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS_FINDTOPK) $(OBJECTS_FINDTOPK_PTHREAD) $(OBJECTS_FINDTOPK_MQUEUE) $(TARGETS)