require_relative("asm.rb") #adding file with module
init_cmd("common/cmd_bin.txt")

#-------- Code ----------

l_cycle!

add x1, x2, x3
sub x1, x2 ,x3

j l_cycle

#------------------------

close_cmd
