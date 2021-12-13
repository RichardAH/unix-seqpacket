const seq = require("../build/Release/unix_seqpacket");
module.exports = {
    open: seq.open,
    close: seq.close,
    available: seq.available
};
