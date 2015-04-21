target extended-remote :4242
load
b main.c:107

#display /wt 0x40012458
#display /wt 0x40012440
#display /wt 0x40012408
#display /wt 0x40012404
#display /wt 0x40012400
#display /wt 0x40000400
#display /wt 0x40000404
#display /wt 0x40000410
#display /wt 0x40000424

#-------Clocks------#
#RCC base and Control Reg
display /wt 0x40023800 
#Config reg - prescalars and sources
display /wt 0x40023808               
              
