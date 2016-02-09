default:
	cc test.c -o unit_tests

test:
	cc test.c -o unit_tests && ./unit_tests

clean:
	rm unit_tests