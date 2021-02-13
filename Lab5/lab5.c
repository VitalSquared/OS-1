


int main(int argc, char** argv) {
	FILE* file;
	if (argc < 2) {
		printf("Usage: <programname> <filename>\n");
		return 0;
	}
	file = fopen(argv[1], "r");
	if (!file) {
		perror("Unable to open file\n");
		return 0;
	}	
	

	fclose(file);
	return 0;
}
