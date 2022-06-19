build-bdd:
	gcc ./src/bdd.c -o ./output/bdd

build-server:
	gcc ./src/server.c -o ./output/server

build-client:
	gcc ./src/client.c -o ./output/client

build-stack:
	make build-bdd
	make build-server
	make build-client

run-server:
	./output/server

run-client:
	./output/client


clean:
	rm output/*
