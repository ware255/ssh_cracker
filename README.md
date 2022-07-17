# ssh_cracker
sshのパスワード解析ツールです。辞書攻撃、総当たり攻撃、どちらも可能です。<br>
※使用する際は自己責任でお願いします＾＾<br>

## 動作可能OS
* Linux
* Windows 10 ~
* other unix...

## インスール方法
Linux場合(だけ)
```
sudo apt install libssl-dev libssh-dev
git clone https://github.com/ware255/ssh_cracker.git
cd ssh_cracker
make
```

## 実行方法
使い方はこうです。
```
./ssh_cracker -h <IP> -p <Port> -u <user_name>
```
以下が例となります。<br>
```
./ssh_cracker -h 255.255.255.255 -p 22 -u user
```

## 参考
[libssh](https://www.libssh.org/)<br>
[Installing libssh](https://subscription.packtpub.com/book/web-development/9781789349863/app02/app02lvl1sec156/installing-libssh)<br>
[SSH-Brute](https://github.com/Ace-Krypton/SSH-Brute/blob/main/ssh_bruter.cpp)<br>
[パスワード解析の時間計測をするが、文字数を変えても時間が変化しない](https://teratail.com/questions/352431)<br>
