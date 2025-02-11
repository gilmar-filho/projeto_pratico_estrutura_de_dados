# Define o compilador C++
CXX = g++

# Define as flags de compilação
CXXFLAGS = -Wall

# Define o nome do executável final
TARGET = app

# Define os arquivos fonte
SOURCES = main.cpp  interface.cpp   sequenceset.cpp

# Define os arquivos objeto correspondentes aos arquivos fonte
OBJECTS = $(SOURCES:.cpp=.o)

# Regra padrão que compila o alvo final
all: $(TARGET)

# Regra para criar o executável a partir dos arquivos objeto
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Regra para compilar arquivos .cpp em arquivos .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar os arquivos objeto e o executável
clear:
	rm -f $(OBJECTS) $(TARGET)

# Regra para executar o programa
run: $(TARGET)
	./$(TARGET)

# Define as regras que não correspondem a arquivos reais
.PHONY: all clear run