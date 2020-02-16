
原因比较简单，就平铺直叙吧。

背景：

调试功能，一直出问题，发现是某接口不通，而手动curl可以。

* debug进去，实际请求的url ok

* 抓包没抓到请求

  期间还有一些其他进程发出的类似请求干扰

* http 库没用代理

* `ps eww`查看进程环境变量发现 `http_proxy=...` 

* `ps -ef`不断找父进程，链路大约： `调试程序 -> debugserver -> dlv -> goland`

  发现只有goland没有该环境变量，说明： goland启动dlv时加入了该环境变量

  仔细看代理内容，想起来是前几天给plugin repo设置的代理。。 竟然。。。 slbj goland，golint支持的那么烂，还要出这种低级问题

* 去掉这个代理设置后，ok
