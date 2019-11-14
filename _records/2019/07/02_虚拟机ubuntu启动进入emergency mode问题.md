
背景：
* vm中ubuntu挂载单独一块硬盘，三个ext4分区，以smb方式提供服务
* host里通过smb挂载这三个分区
* 当时host在下载，写入这三个分区
* 似乎出了啥分区，host的各个硬盘都卸载了。 可能还发生了重启。 
* 可能导致ubuntu里文件系统写入错误（不完整），可能在ubuntu的某FS相关journal里记录了
* 启动时，这块盘挂载不上（mac的坑？似乎没办法把盘彻底卸载）。 导致ubuntu启动时从journal里发现有写入不完整，而涉及的硬盘又没了，于是启动不了，直接进入emergency mode，从log中看到相关信息
* mac上各种unmount、eject都不行。 到家把盘拔插（其实是断电、上电）后ok，可以connect to ubuntu了
* 启动ubuntu，ok。。。 估计这时候通过journal恢复了这个硬盘上文件系统的不完整写入



