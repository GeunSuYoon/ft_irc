# ft_irc

## Mandatory Part

---

- Program name: ircserv
- Turn in files: Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp, \
                  an optional configuration file
- Makefile: NAME, all, clean, fclean, re
- Arguments: port: The listening port \
            password: The connection password
- External functs.: Everything in C++ 98. \
                    socket, close, setsockopt, getsockname, \
                    getprotobyname, gethostbyname, getaddrinfo, \
                    freeaddrinfo, bind, connect, listen, accept, htons, \
                    htonl, ntohs, ntohl, inet_addr, inet_ntoa, send, \
                    recv, signal, sigaction, lseek, fstat, fcntl, poll \
                    (or equivalent)
- Libft authorized: n/a
- Description: An IRC server in C++ 98

---

- C++98로 IRC 서버를 개발해야 한다.
- client를 개발해선 **안된다**.
- server간 communication을 처리해선 **안된다**.
- 아래와 같이 실행할 수 있어야 한다.
  - `./ircserv <port> <password>`
  - port: IRC 서버가 수신 IRC connection을 위한 listening port number
  - password: The connection password. 서버에 연결하려는 모든 IRC client에게 필요하다.

---

#### info
 
- subject와 evaluation에서 `pull()`이 언급되더라도, `select()`, `kqueue()` 또는 `epull()`을 사용할 수 있다.

---

### Requirements

- 서버는 여러 client를 동시에 처리할 수 있어야 하며, 절대 연결돼선 안된다.
- forking은 허용되지 않는다.
  - 모든 I/O operation은 **non-blocking**이어야 한다.

- 모든 작업/(read, write, but also listen 등)을 처리하는데 오직 **1**개의 `poll()`/(or equivalent)을 사용할 수 있다.

---

#### caution

- non-blocking file descriptor를 사용하므로, `poll()`\(or equivalent) 없이 read/recv or write/send 함수를 사용할 수 있고, 서버는 blocking되지 않는다.
- 하지만 이건 더 많은 system resource를 소모할 것이다.
- 따라서, `poll()`\(or equivalent) 없이 file descriptor의 read/recv or write/send를 시도하면 0점이다.

---

- 
