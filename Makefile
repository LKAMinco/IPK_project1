TARGET = server
OUTPUT = hinfosvc

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	g++ -g -Wall -o $(OUTPUT) $(TARGET).cpp
clean:
	$(RM) $(OUTPUT)