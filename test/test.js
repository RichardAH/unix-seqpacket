const seq = require("../lib/index.js");
const fs = require('fs')
let fd = seq.open("/var/run/xrpl-uplink/subscriber.sock")

console.log("fd:", fd)
let buf = Buffer.alloc(1024*1024);
let bytes_read = fs.readSync(fd, buf);
console.log("bytes_read:", bytes_read)
console.log(buf.slice(0, bytes_read).toString('hex'))

seq.close(fd);
