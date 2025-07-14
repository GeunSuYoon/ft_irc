# Function

## socket

``` C
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

설명

- 네트워크 프로그래밍에서 소켓을 생성할 때 사용하는 함수.
- 주로 TCP/IP 통신을 위해 사용된다.

매개변수

- int domain
  - 사용할 프로토콜 도메인\(주소)
  - AF_INET: IPv4
  - AF_INET6: IPv6
- int type
  - 소켓의 타입\(데이터 전송 방식)
  - SOCK_STREAM: TCP
  - SOCK_STREAM: UDP
- int protocol
  - 사용할 프로토콜\(0을 사용하면 자동 선택)

반환값

- 성공 시: 생성된 socket descripter\(int)
- 실패 시: -1 + errno 설정

---

## setsockopt

``` C
#include <sys/types.h>
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

설명

- 소켓의 옵션을 설정하할 때 사용한다.

매개변수

- int sockfd
  - 설정할 소켓의 fd
- int level
  - 옵션의 수준
- int optname
  - 설정할 옵션 이름
- const void *optval
  - 옵션 값을 담은 포인터
- socklen_t optlen
  - 옵션 값의 크기

- level과 optname은 반드시 호환되는 것을 써야 한다.
  - \[SOL_SOCKET, SO_REUSEADDR]: 포트를 재사용할 수 있게 한다.
  - \[SOL_SOCKET, SO_RCVBUF / SO_SNDBUF]: 수신/송신 버퍼 크기 설정
  - \[SOL_SOCKET, SO_KEEPALIVE]: TCP keep-Alive 패킷 활성화
  - \[IPPROTO_TCP, TCP_NODELAY]: Nagle 알고리즘 비활성화\(지연 최소화)
  - \[SOL_SOCKET, SO_LINGER]: close 후 남은 데이터 처리 방식 조정

반환값

- 성공 시: 0
- 실패 시: -1 + errno 설정

---

## getsockname

``` C
#include <sys/types.h>
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

설명

- 소켓에 현재 바인드된 로컬 주소 정보를 가져오는 함수.
- 아래의 경우 사용한다.
  - 소켓에 포트 번호 0을 주고 바인드했을 때, 커널이 할당한 실제 포트 번호를 알아내기 위해.
  - 자기 IP 주소 및 포트 번호를 확인하고 싶을 때.

매개변수

- int sockfd
  - 정보를 알아낼 소켓 fd
- struct sockaddr *addr
  - 주소 정보가 저장될 구조체 포인터\(`socketaddr_in *` 등으로 캐스팅 필요)
- socklen_t *addrlen
  - 주소 구조체의 크기를 전달

반환값

- 성공 시: 0
- 실패 시: -1 + errno 설정

---

## getprotobyname

``` C
#include <netdb.h>

struct protoent *getprotobyname(const char *name);

struct protoent {
    char  *p_name;       // 프로토콜 이름 (e.g. "tcp")
    char **p_aliases;    // 별칭 리스트 (보통 NULL)
    int    p_proto;      // 프로토콜 번호 (e.g. 6 for TCP, 17 for UDP)
};

```

설명

- 문자열로 된 프로토콜 이름\(tcp, udp 등)을 사용하여 그에 대응하는 프로토콜 번호를 조회하는 함수.
- `socket()` 호출 시 마지막 인자인 `protocol`에 넘길 수 있도록 번호를 얻는 용도로 사용한다.

매개변수

- const char *name
  - 번호를 알아낼 프로토콜 이름

반환값

- 성공 시: `protocol` 구조체에 대한 포인터
  - tcp: 6\(IPROTO_TCP)
  - udp: 17\(IPROTO_UDP)
  - icmp: 1\(IPROTO_ICMP)
- 실패 시: NULL

---

## gethostbyname

``` C
#include <netdb.h>

struct hostent *gethostbyname(const char *name);

struct hostent {
    char  *h_name;        // 공식 호스트 이름
    char **h_aliases;     // 별칭 목록 (NULL로 끝나는 배열)
    int    h_addrtype;    // 주소 체계 (보통 AF_INET)
    int    h_length;      // 주소 길이 (IPv4의 경우 4)
    char **h_addr_list;   // 주소 배열 (여러 IP일 수 있음)
};

```

설명

- 호스트 이름\(`www.google.com` 등)을 IP주소로 바꾸는 함수.
- 최근엔 `getaddrinfo()`를 사용하는 것이 좋다.
- IPv4 전용이다.

매개변수

- const char *name
  - 조회하려는 도메인 또는 호스트 이름

반환값

- 성공 시: `hostent` 구조체 포인터
- 실패 시: NULL

---

## getaddrinfo

``` C
#include <netdb.h>

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

struct addrinfo {
    int              ai_flags;     // 옵션 (예: AI_PASSIVE 등)
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // 0 또는 IPPROTO_TCP 등
    socklen_t        ai_addrlen;   // sockaddr 구조체 크기
    struct sockaddr *ai_addr;      // 실제 주소 정보
    char            *ai_canonname; // 공식 호스트 이름
    struct addrinfo *ai_next;      // 다음 결과 (연결 리스트)
};

```

설명

- 호스트 이름\(또는 IP주소)과 서비스 이름\(또는 포트 번호)를 소켓 주소로 바꾸는 함수.
- IPv4, IPv6, TCP/UDP, 쓰레드 안정성, 포트명 해석까지 전부 지원한다.
- `gethostbyname`, `getservbyname`을 모두 대체할 수 있다.
- 할당한 메모리를 `free`로 해제하지 말고 아래 `freeaddrinfo()`로 해제해야 한다.

매개변수

- const char *node
  - 도메인 이름 또는 ip 주소\(`www.example.com` 또는 `127.0.0.1`)
- const char *service
  - 포트 번호 또는 서비스 이름\(http 또는 80)
- const struct addrinfo *hints
  - 어떤 주소 결과를 원하는지 조건 지정
- struct addrinfo **res
  - 결과로 나올 주소 정보르 담을 포인터의 포인터\(Linked list 형태로 반환)

반환값

- 성공 시: 0
- 실패 시: !0
  - `gai_strerror()`로 에러 메시지를 출력할 수 있다.

---

## freeaddrinfo

``` C
#include <netdb.h>

void freeaddrinfo(struct addrinfo *res);

```

설명

- `getaddrinfo()`로 할당한 주소 정보 메모리를 해제하는 함수

매개변수

- struct addrinfo *res
  - `getaddrinfo()`가 반환한 연결 리스트의 시작 포인터

---

## bind

``` C
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

```

설명

- 소켓에 특정 IP 주소와 포트 번호를 바인드하는 함수.
- 서버 프로그램에서 클라이언트가 접속할 IP와 포트를 지정할 때 쓴다.
- 포트 번호가 이미 사용 중이면 실패한다.\(`EADDRINUSE`)

매개변수

- int sockfd
  - 바인딩할 소켓 fd
- const struct sockaddr *addr
  - 바인딩할 주소\(IPv4, IPv6, 등 `sockaddr` 구조체 포인터)
- socklen_t addrlen
  - 주소 구조체 크기\(`sizeof(struct sockaddr_in)` 등)

반환값

- 성공 시: 0
- 실패 사: -1, errno 설정

---

## connect

``` C
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

```

설명

- 클라이언트 소켓을 특정 서버 주소\(IP+Port#)에 연결할 때 사용하는 함수.
- 클라이언트 소켓이 서버 IP와 포트에 연결 요청을 보낸다.
- 성공하면 소켓이 서버와 연결 상태가 된다.
- TPC 연결 시 3-way handshake가 내부적으로 처리된다.

매개변수

- int sockfd
  - 연결할 클라이언트 소켓 fd
- const struct sockaddr *addr
  - 서버 정보 주소\(`struct sockaddr_in` 등 포인터)
- socklen_t addrlen
  - 주소 구조체 크기\(`sizeof(struct sockaddr_in)` 등)

반환값

- 성공 시: 0
- 실패 시: -1
  - errno 확인

---

## listen

``` C
#include <sys/types.h>
#include <sys/socket.h>

int listen(int sockfd, int backlog);

```

설명

- TCP 서버 소켓이 클라이언트의 연결 요청을 기다릴 준비 상태로 전환할 때 사용하는 함수.
- `bind()`로 주소 할당한 소켓을
- `listen()` 호출로 수신 대기 상태로 전환한다.

매개변수

- int sockfd
  - 소켓 fd
- int backlog
  - Queue에 대기할 최대 연결 요청 수\(대기열 크기)
  - 너무 작으면 동시 접속 요청을 일부 놓칠 수 있다.
  - 너무 커도 OS 한도 이상은 무시되거나 메모리 낭비.
  - 5 ~ 128 정도 값을 사용하자.

반환값

- 성공 시: 0
- 실패 시: -1
  - errno 설정

---

## accept

``` C
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

```

설명

- TCP 서버에서 클라이언트의 연결 요청을 수락하고 새 소켓을 생성하는 함수.
- `listen()`으로 대기 중인 연결 요청을 받아 실제 통신용 소켓을 생성.
- 서버 소켓에 대기 중인 클라이언트 연결 요청을 하나 꺼내 수락.
- 새로운 소켓 fd 반환 -> 이 소켓으로 클라이언트와 데이터 송수신.
- 원래 서버 소켓\(`sockfd`)은 계속 `listen` 상태 유지.

매개변수

- int sockfd
  - `socket()` -> `bind()` -> `listen()` 상태인 서버 소켓 fd
- struct sockaddr *addr
  - 연결된 클라이언트 주소 정보를 저장할 구조체 포인터\(`sockaddr_in` 등)
- socklen_t *addrlen
  - `addr` 구조체 크기를 가리키는 포인터.
  - 호출 시 크기 전달, 호출 후 실제 크기 저장.
 
- 만일 클라이언트 주소 정보가 필요 없다면 `addr`과 `addrlen`에 `NULL`을 넣을 수 있음.

반환값

- 성공 시: 새로 생성된 클라이언트 소켓 fd\(양수)
- 실패 시: -1
  - errno
  - `EAGAIN` / `EWOULDBLOCK`: nonblocking 모드에서 대기 중인 연결 없음
  - `EBADF`: 잘못된 fd 
  - `ECONNABORTED`: 연결이 중간에 취소됨
  - `EINVAL`: 소켓이 `listen()` 상태가 아님

---

## htons

``` C
#include <arpa/inet.h>

uint16_t htons(uint16_t hostshort);

```

설명

- Port#이나 16비트 정수를 호스트 바이트 순서\(host byte order) -> 네트워크 바이트 순서(network byte order)로 변환하는 함수.
- 호스트 바이트 순서는 머신마다 다르다.
  - x86, ARM 등 대부분 CPU는 Little Indian
- 네트워크 바이트 순서는 Big Indian
- 따라서, 숫자를 네트워크로 보낼 땐 항상 변환해줘야 한다.

매개변수

- uint16_t hostshort
  - 호스트 바이트 순서의 16비트 정수

반환값

- 네트워크 바이트 순서로 변환된 16비트 정수

---

## htonl

``` C
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);

```

설명

- htons와 마찬가지로 호스트 바이트 순서 -> 네트워크 바이트 순서로 변환하는 함수이다.
- 다만, 32비트 정수를 다룬다.
- 즉, IP 주소 같은 값을 다룰 때 사용한다.

매개변수

- uint32_t hostlong
  - 호스트 바이트 순서의 32비트 정수

반환값

- 입력 값을 네트워크 바이트 순서로 변환한 32비트 정수

---

## ntohs

``` C
#include <arpa/inet.h>

uint16_t ntohs(uint16_t netshort);

```

설명

- 네트워크에서 수신한 16비트 정수 데이터를 호스트 바이트 순서로 변환하는 함수.
- `htons()`의 반대 역할을 한다.

매개변수

- uint16_t netshort
  - 네트워크 바이트 순서의 16비트 정수

반환값

- 입력 값을 호스트 바이트 순서로 변환한 16비트 정수

---

## ntohl

``` C
#include <arpa/inet.h>

uint32_t ntohl(uint32_t netlong);

```

설명

- 네트워크에서 수신한 32비트 정수 데이터를 호스트 바이트 순서로 변환하는 함수.
- `htonl()`의 반대 역할.

매개변수

- uint32_t netlong
  - 네트워크 바이트 순서의 32비트 정수

반환값

- 입력 값을 호스트 바이트 순서로 변환한 32비트 정수

---

## inet_addr

``` C
#include <arpa/inet.h>

in_addr_t inet_addr(const char *cp);

```

설명

- 문자열로 된 IPv4 주소\(192.168.2.1 등)를 32비트 네트워크 바이트 순서 정수로 변환하는 함수.
- 사용 비권장 상태로, `inet_pton()` 사용을 권장한다.

매개변수

- const char *cp
  - 변환할 IPv4 주소 문자열.

반환값

- 성공 시: 네트워크 바이트 순서의 IPv4 주소\(`in_addr_t`)
- 실패 시: `INADDR_NONE`

---

## inet_ntoa

``` C
#include <arpa/inet.h>

char *inet_ntoa(struct in_addr in);

```

설명

- IPv4 주소를 사람이 읽을 수 있는 문자열로 바꾸는 함수.
- 즉, `inet_addr()`의 반대 역할이다.
- 반환하는 문자열은 static 버퍼에 저장되므로, 다음 `inet_ntoa()` 호출 시 덮어씌워진다.
  - 즉, 멀티스레드에서 위험하므로 문자열을 복사해 두는 것을 권장한다.

매개변수

- struct in_addr in
  - 변환할 IPv4 주소
  - 네트워크 바이트 순서여야 한다

반환값

- 성공 시: IPv4 주소 문자열을 가리키는 포인터
- 실패 시: NULL

---

## send

``` C
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

```

설명

- 연결된 소켓을 통해 데이터를 전송할 때 사용하는 함수.
- TCP 통신에서 주로 사용되며, 내부적으론 `write()`와 비슷하지만, 전송 옵션을 지정할 수 있다.

매개변수

- int sockfd
  - 연결된 소켓의 fd
- const void *buf
  - 보낼 데이터가 들어있는 버퍼의 포인터
- size_t len
  - 전송할 데이터의 바이트 수
- int flags
  - 전송 옵션
  - 0: 일반 전송
  - MSG_DONTWAIT: nonblocking 전송 시도
  - MSG_NOSIGNAL: 전송 실패 시 `SIGPIPE` 신호 방지\(Linux 전용)

반환값

- 성공 시: 실제 전송된 바이트 수\(0 <= n <= len)
- 실패 시: -1
  - errno

---

## recv

``` C
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);

```

설명

- TCP와 같은 연결 지향형 소켓에서 데이터를 수신할 때 사용하는 기본 함수.
- `read()`와 기능은 비슷하지만, 수신 동작을 제어하는 옵션을 지정할 수 있다.

매개변수

- int sockfd
  - 연결된 소켓 fd
- void *buf
  - 수신 데이터를 저장할 버퍼
- size_t len
  - 버퍼 크기\(최대 수신 바이트 수)
- int flags
  - 수신 옵션
  - 0: 일반 수신
  - MSG_DONTWAIT: nonblocking 수신 시도
  - MSG_PEEK: 읽되, 버퍼에서 제거하지 않음\(미리보기 용)
  - MSG_WAITALL: 버퍼가 찰 때까지 계속 대기

반환값

- 성공 시: 수신한 바이트 수
- 0: 상대방이 연결을 종료\(closed)했음
- 실패 시: -1
  - errno

---

## lseek

``` C
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);

```

설명

- 열린 파일의 읽기/쓰기 위치\(offset)를 이동할 때 사용하는 low level 함수.
- `read()`나 `write()` 전에 파일 포인터 위치를 조정하거나, 파일 크기를 구할 떄 사용.
- 파이프나 소켓에 사용할 수 없다.

매개변수

- int fd
  - fd
- off_t offset
  - 이동할 바이트 수\(음수도 가능)
- int whence
  - 기준 위치 지정 값
  - `SEEK_SET`: 파일 시작 위치
  - `SEEK_CUR`: 현재 위치
  - `SEEK_END`: 파일의 끝\(파일 크기)

반환값

- 성공 시: 이동 후 파일의 offset\(바이트 단위)
- 실패 시: -1
  - errno

---

## fstat

``` C
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int fstat(int fd, struct stat *statbuf);

```

설명

- fd에 대해 파일의 정보를 가져오는 함수.

매개변수

- int fd
  - 확인할 fd
- struct stat *statbuf
  - 파일의 정보를 담을 구조체 포인터 `struct stat *`

반환값

- 성공 시: 0
- 실패 시: -1
  - errno

---

## fcntl

``` C
#include <fcntl.h>
#include <unistd.h>

int fcntl(int fd, int cmd, ... /* arg */);

```

설명

- fd 조작 및 속성 제어 함수.

매개변수

- int fd
  - 조작할 fd
- int cmd
  - 수행할 작업
  - `F_GETFL`: fd의 현재 플래그 조회
  - `F_SETFL`: fd의 플래그 설정
  - `F_GETFD`: close-on-exec 설정 여부 확인
  - `F_SETFD`: close-on-exec 설정
  - `F_DUPFD`: 새 fd 생성
    - `dup()`과 유사하지만, 최소값을 지정할 수 있어 더 유연하다.
  - `F_SETLK`/`F_GETLK`/`F_SETLKW`: 파일 잠금 관련
- arg
  - cmd에 따라 필요할 수도 있다

반환값

- cmd별로 의미가 다름

---

## poll

``` C
#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);

struct pollfd {
    int   fd;        // 감시할 파일 디스크립터
    short events;    // 감시할 이벤트 (입력, 출력 등)
    short revents;   // 결과 이벤트 (poll() 호출 후 설정됨)
};

```

설명

- 여러 fd를 동시에 감시해 read/write 가능 상태나 예외 조건이 발생했는지 검사하는 I/O MUX 함수.
- `select()`보다 유연해 소켓 서버 등에서 자주 사용된다.

매개변수

- struct pollfd fds[]
  - 감시할 `pollfd` 구조체 배열
- nfds_t nfds
  - `fds[]` 배열 길이
- int timeout
  - 대기 시간\[ms]
  - -1: 무한 대기
  - 0: 즉시 반환
  - \>0: 최대 대기 시간

- events
  - `POLLIN`: 읽기 가능\(데이터 있음)
  - `POLLOUT`: 쓰기 가능\(버퍼 공간 있음)
  - `POLLERR`: 에러 조건 발생
  - `POLLHUP`: 연결 종료
  - `POLLNVAL`: 잘못된 fd

반환값

- \> 0: 이벤트가 발생한 fd 개수
- 0: time out
- -1: 오류 발생
  - errno

---

## select

``` C
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

```

설명

- `poll()`과 같은 I/O MUX 함수.
- 여러개의 fd의 상태를 동시에 감시할 수 있다.
- 호출 전 `fd_set`에 fd를 `FD_SET`으로 등록해야 한다.
- 호출 후 해당 `fd_set`이 변경된다.
  - 다시 `FD_ZERO` 후 `FD_SET`

매개변수

- int nfds
  - 감시할 fd 중 가장 큰 값 + 1
- fd_set *readfds
  - 읽기를 감시할 `fd_set`
- fd_set *writefds
  - 쓰기를 감시할 `fd_set`
- fd_set *exceptfds
  - 예외를 감시할 `fd_set`
- struct timeval *timeout
  - 대기 시간
  - NULL이면 무한 대기

반환값

- \> 0: 준비된 fd 개수
- 0: time out
- -1: 오류 발생
  - errno

---

## kqueue

``` C
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

int kqueue(void);

int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);

```

설명

- BSD 계열\(macOS, FreeBSD 등) 운영체제에서 제공하는 고성능 I/O 이벤트 통지 시스템.
- `select()`, `poll()`보다 더 효율적으로 대규모 fd 이벤트 감시 처리.
- 난 Linux 환경이라 안쓸거다...

---

## epoll

``` C
#include <sys/epoll.h>
#include <unistd.h>

int epoll_create(int size); // size는 무시되지만 1 이상이어야 함
// 또는 리눅스 2.6.27 이상에서는:
int epoll_create1(int flags); // 예: EPOLL_CLOEXEC
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

struct epoll_event {
    uint32_t events;   // 감시할 이벤트: EPOLLIN, EPOLLOUT 등
    epoll_data_t data; // 사용자 정의 데이터 (fd 저장에 자주 사용됨)
};

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

```

설명

- Linux에서 사용하는 고성능 I/O MUX 인터페이스.
- 효율적으로 수천개의 소켓을 동시에 감시할 수 있고, 대규모 네트워크 서버에서 필수이다.
- `epoll_create()`로 인터페이스 생성.
- `epoll_ctl(…, EPOLL_CTL_ADD, …)`로 관심 fd 등록.
- `epoll_wait()`로 이벤트 감시.

Events
- `EPOLLIN`: 읽기 가능
- `EPOLLOUT`: 쓰기 가능
- `EPOLLRDHUP`: 소켓 종료
- `EPOLLERR`: 에러
- `EPOLLET`: 엣지 트리거 모드
- `EPOLLONESHOT` :이벤트 1회만 감시하고 비활성화

---
