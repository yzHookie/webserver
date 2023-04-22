server:
	clang++ util.cpp client.cpp -o client && \
	clang++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp -o server

clean:
	rm server && rm client