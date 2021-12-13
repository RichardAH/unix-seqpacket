# unix-seqpacket
Add unix domain socket sock_seqpacket support to nodejs

## Install
```bash
npm install unix-seqpacket
```

## Use
```js
const seq = require('unix-seqpacket')
const fs = require('fs')
let sock_fd = seq.open('/some/domain/socket.sock')
let read_bytes = fs.readSync(sock_fd)

// ... also can use seq.available(sock_fd) to get the number of bytes immediately available for reading

seq.close(sock_fd);
```
