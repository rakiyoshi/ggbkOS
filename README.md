# ggbkOS
[30日でできる！ OS自作入門](https://www.amazon.co.jp/dp/B00IR1HYI0) をやっていきます。

## Requirements
- qemu
- wsl

## Cheat Sheet
### Create image
```
make img
```

### Run
```
make run
```

### Stop
`Ctrl+a x` で QEMU を終了。

### Build Container
```bash
cd ./dockerfiles && make build
```

### VNC
`.wslconfig` に `localhostForwarding=true` を追記。
`localhost:5900` に VNC クライアントで接続
