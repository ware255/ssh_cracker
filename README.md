# ssh_cracker
sshのパスワード解析ツールです。辞書攻撃、総当たり攻撃、どちらも可能です。<br>
※使用する際は自己責任でお願いします＾＾<br>

## 動作可能OS
* Linux x86_64
* Windows 10 以降
* など

## インスール方法
Linux場合(だけ)
```
$ sudo apt install libssl-dev libssh-dev gcc make
$ git clone https://github.com/ware255/ssh_cracker.git
ssh_cracker$ cd ssh_cracker
ssh_cracker$ make
```

## 実行方法
使い方はこうです。
```
ssh_cracker$ ./ssh_cracker -h <IP> -p <Port> -u <user_name>
```
以下が例となります。<br>
```
ssh_cracker$ ./ssh_cracker -h 255.255.255.255 -p 22 -u user
```

## 参考
[libssh](https://www.libssh.org/)<br>
[Installing libssh](https://subscription.packtpub.com/book/web-development/9781789349863/app02/app02lvl1sec156/installing-libssh)<br>
[SSH-Brute](https://github.com/Ace-Krypton/SSH-Brute/blob/main/ssh_bruter.cpp)<br>
[パスワード解析の時間計測をするが、文字数を変えても時間が変化しない](https://teratail.com/questions/352431)<br>
[【C言語】ioctlでIPアドレスの取得・書き換え方法](https://wireless-network.net/ioctl-ip/)<br>
[Linux Programmer's Manual (7)](http://linuxjm.osdn.jp/html/LDP_man-pages/man7/netdevice.7.html)<br>
[C++ compiling on Windows and Linux: ifdef switch [duplicate]](https://stackoverflow.com/questions/6649936/c-compiling-on-windows-and-linux-ifdef-switch)<br>
