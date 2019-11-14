
## 背景

socket好像支持shutdown read和shutdown write。
但。。tcp协议好像没有shutdown read的操作。

## 调研

ref： https://stackoverflow.com/questions/740817/behavior-of-shutdownsock-shut-rd-with-tcp

```
shutdown(,SHUT_RD) does not have any counterpart in TCP protocol, so it is pretty much up to implementation how to behave when someone writes to a connection where other side indicated that it will not read or when you try to read after you declared that you wont.

On slightly lower level it is beneficial to remember that TCP connection is a pair of flows using which peers send data until they declare that they are done (by SHUT_WR which sends FIN). And these two flows are quite independent.
```

```
I test shudown(sock,SHUT_RD) on Ubuntu 12.04. I find that when you call shutdown(sock,SHUT_RD) if there are no any type of data(include FIN....) in the TCP buffer, the successive read call will return 0(indicates end of stream). But if there are some data which arrived before or after shutdown function, read call will process normally as if shutdown function was not called. It seems that shutdown(sock,SHUT_RD) doesn't cause any TCP states changed to the socket
```

## 结论

* socket层的语义操作，底层实现不一定都支持
  可能某种socket支持 SHUT_RD，但tcp socket不支持
* socket层有统一的行为
  比如tcp socket的 SHUT_RD 虽然在tcp层啥都不干。 但socket的后续read会 EOF。
  > 上面提到了，在 SHUT_RD 之前到来的包会导致read正常处理
  > 而 SHUT_RD 之后到来的包也会导致read正常处理 这一点不太相信的，也许需要实测一下
