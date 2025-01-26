# Nombre del ejecutable
TARGET = ecuafast

# Compilador y banderas
CC = gcc
CFLAGS = -Wall -pthread

# Archivos fuente
SRC = Codigo_ContraChanChay.c

# Regla por defecto: compilar
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para ejecutar el programa
run: $(TARGET)
	./$(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)

# Regla para recompilar desde cero
rebuild: clean all
