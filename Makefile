# Build all
all: clean
	@mkdir bin
	@make bin/server bin/client

# Build targets
bin/server: src/server.cpp src/headsock.h
	@echo "Compiling server..."
	@g++ $^ -o $@
	@echo "Done."

bin/client: src/client.cpp src/headsock.h
	@echo "Compiling client..."
	@g++ $^ -o $@
	@echo "Done."
	
# Clean build
clean:
	@echo "Cleaning build..."
	@rm -rf bin
	@echo "Done."

.PHONY: clean
