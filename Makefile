format:
	@echo "Formatting source files with clang-format..."
	find src/ include/ -name '*.cc' -o -name '*.h' -print0 | xargs -0 clang-format -style=file -i
	@echo "Formatting source files with clang-format done."