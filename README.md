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

- 여러 IRC client가 존재한다. 그 중 하나를 **reference**로 선택해야 한다. reference client는 평가 과정에서 사용된다.
- reference client는 어떤 error 없이 서버에 연결할 수 있어야 한다.
- client와 서버간 통신은 TCP/IP\(v4 or v6)를 통해 이루어져야 한다.
- reference client를 서버와 함께 사용하는 것은 공식 IRC 서버와 유사해야 한다. 하지만, 아래의 기능만 구현하면 된다.
  - reference client를 사용하여 authenticate, username/nickname 설정, channel 가입, send/receive private message가 가능해야 한다.
  - 한 client에서 channel로 전송되는 모든 message는 channel에 가입한 모든 client에게 전달되어야 한다.
  - **운영자**와 일반 사용자가 있어야 한다.
  - channel operator에게 특정 명령어를 구현해야 한다.
    - KICK: channel에서 client를 추방
    - INVITE: channel에 client를 초대
    - TOPIC: channel topic을 변경 또는 보기
    - MODE: channel mode를 변경
      - i: set/remove Invite-only channel
      - t: set/remove channel 운영자의 TOPIC 명령어 제한
      - k: set/remove channel key\(password)
      - o: give/take channel 운영자 권한
      - l: set/remove channel 사용자 제한
- 물론, 깔끔하게 코드를 작성하도록 하자!

### For MacOS only

- MacOS는 다른 Unix OS와 다른 방식으로 `write()`를 구현하므로 `fcntl()`을 사용할 수 있다.
- 다른 Unix OS와 유사한 동작을 하기 위해 non-blocking mode의 file descritptor를 사용해야 한다.

- `fcntl()`은 아래와 같이 사용할 수 있다.
  - `fcntl(fd, F_SETFL, O_NONBLOCK)`
  - 다른 flag는 금지한다.
 
### Test example

- 가능한 모든 오류와 문제\(receiving partial data, low bandwidth, 등등)를 확인해야 한다.
- 서버가 보내는 모든 것을 올바르게 수행할 수 있도록 다음과 같은 간단한 테스트를 수행할 수 있다.

```
\$> nc -C 127.0.0.1 6667
com^Dman^Dd
\$>
```

- `ctrl+D`를 사용하여 명령어를 여러 부분으로 보낸다. \'com', \'man', \'d\n'
- 명령어를 처리하기 위해 먼저 수신된 packet을 모아 재구성해야 한다.
