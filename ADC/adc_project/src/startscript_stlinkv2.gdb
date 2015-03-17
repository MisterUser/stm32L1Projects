
target extended-remote :4242
load
b main.c:80
display /wt 0x40012458
display /wt 0x40012440
display /wt 0x40012408
display /wt 0x40012404
display /wt 0x40012400
