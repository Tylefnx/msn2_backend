# Derleyici
CC = gcc

# Derleme seçenekleri
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Hedef dosya adı
TARGET = server

# Kaynak dosyalar
SRC = main.c auth.c

# Nesne dosyaları
OBJ = $(SRC:.c=.o)

# Varsayılan hedef
all: $(TARGET)

# Hedef uygulamanın derlenmesi
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Kaynak dosyalardan nesne dosyalarının oluşturulması
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Temizlik işlemi
clean:
	rm -f $(OBJ) $(TARGET)

# Test sunucusunu başlatma
run: $(TARGET)
	./$(TARGET)

# Yardımcı hedefler (clean ve run gibi) için
.PHONY: all clean run
