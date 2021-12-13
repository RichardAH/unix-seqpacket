{
  "targets": [
    {
      "target_name": "unix_seqpacket",
      "include_dirs": ["<!(node -e \"require('nan')\")"],
      "sources": [ "src/seq.cc" ]
    }
  ]
}
