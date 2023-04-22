server:
	clang++ src/util.cpp client.cpp -o client &&\
	clang++ src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp src/EventLoop.cpp src/Server.cpp server.cpp -o server

clean:
	rm server && rm client