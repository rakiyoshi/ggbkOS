# ggbkOS
[30日でできる！ OS自作入門](https://www.amazon.co.jp/dp/B00IR1HYI0) をやっていきます。

## Cheat Sheet
### Run
```bash
IMAGE_PATH="helloos0/helloos.img"
qemu-system-x86_64 -drive file=${IMAGE_PATH},format=raw -nographic
```

### Stop
`Ctrl+a x` で QEMU を終了。
