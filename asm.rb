module Cmd
    ARG_SHIFTS = [nil, 21, 16, 11].freeze
    ARG_MASK = 2**5 - 1
    FUNC_SHIFT = 26
    FUNC_MASK = 2**6 - 1
    INSTR_MASK = 0xFFFFFFFF
    LAST_5_MASK = 2**5 - 1
    LAST_11_MASK = 2**11 - 1
    LAST_16_MASK = 2**16 - 1
    LAST_26_MASK = 2**26 - 1

    HASH_NOT_FOUND = -1
  
    class << self #open singltone class for this module
        attr_accessor :instr, :file, :instr_counter, :forward_j_hash, :back_j_hash

        @forward_j_hash = {}
        @back_j_hash = {}

        #---------- Jump work ------------
        def instr_counter_incr
            @instr_counter += 1
        end

        def forward_j_table_get(key)
            @forward_j_hash[key]
        end
        
        def forward_j_table_set(key, val)
            @forward_j_hash[key] = val
        end

        def back_j_table_get(key)
            @back_j_hash[key]
        end

        def back_j_table_set(key, val)
            # puts("CLASS: instr_counter: #{@instr_counter}")
            @back_j_hash[key] = val
        end
        
        def set_forward_j(index)
            set_last(index, LAST_26_MASK)
            set_code(0b00110000)
        end
        #---------------------------------

        #------ Coding instructions ------
        def reset_instr
            @instr = 0
        end
    
        def set_arg(num, arg)
            @instr |= ((arg & ARG_MASK) << ARG_SHIFTS[num])
        end
      
        def set_last(arg, mask)
            @instr |= (arg & mask)
        end
      
        def set_code(code)
            @instr |= ((code & FUNC_MASK) << FUNC_SHIFT)
            @instr &= INSTR_MASK
        end
      
        def check_align(addr)
            abort("misaligned address(=#{addr})") if (addr & 3) != 0
        end
      
        def emit
            @file.write([@instr].pack("L<"))
            reset_instr
        end    
        #---------------------------------
    end
  
end

#adding meеhod to class Integer
class Integer
    def call(base)
        [self, base]
    end
end

#redefinition of method_missing
#for it to return array of strings 
def method_missing(method, *args)
    # puts("method: #{method}")

    if args.empty?
        #reg name met
        if method.to_s[0] == 'x'
            num = method.to_s.delete_prefix('x').to_i
            if num > 31 || num < 0
                abort("wrong register num (= #{num})")
            else
                return num
            end
        #jump met 
        elsif method.to_s.start_with?("l_")
            #jump label met
            if method.to_s[-1] == '!' 
                # puts("label")
                target = method.to_s[2..-2]
                saved_counter = Cmd.forward_j_table_get(target)
                # puts("forward jump saved_counter: #{saved_counter}")
                
                #forward jump
                if saved_counter != Cmd::HASH_NOT_FOUND
                    #TODO: вынести логику патчинга файла в метод класса     
                    cur_pos = Cmd.file.pos #save pos in file
                    # puts("cur_pos: #{cur_pos}")
                    Cmd.file.pos = (saved_counter) * 4
                    # puts("instr counter to patch: #{Cmd.instr_counter}")
                    Cmd.set_forward_j(Cmd.instr_counter) 
                    # puts("instr: #{Cmd.instr}")
                    Cmd.file.write([Cmd.instr].pack("L<"))    
                    Cmd.file.pos = cur_pos #return pos in file
                    
                #back jump
                else
                    # puts("back jump instr_counter: #{Cmd.instr_counter}")
                    Cmd.back_j_table_set(target, Cmd.instr_counter)
                end
                
            #jump instr met
            else 
                # puts("jump")
                target = method.to_s[2..]
                saved_counter = Cmd.back_j_table_get(target)
                # puts("back jump saved_counter: #{saved_counter}")

                #back jump
                if saved_counter != Cmd::HASH_NOT_FOUND 
                    return (saved_counter) 
                #forward jump
                else 
                    Cmd.forward_j_table_set(target, Cmd.instr_counter)
                    return 0
                end
            end
        elsif method.to_s == 'to_int'
            super
        else 
            abort("wrong register/label name")
        end 
    else
        # super
        abort("wrong syntax")
    end
end

#------ Callable methods (top-level) ------
  
def init_cmd(filename)
    Cmd.file = File.open(filename, "wb")
    Cmd.reset_instr
    Cmd.instr_counter = 0

    #key = target_name, val = instr_counter, when forward jump was met
    Cmd.forward_j_hash = Hash.new(Cmd::HASH_NOT_FOUND)
    
    #key = target_name, instr_counter, when back jump was met
    Cmd.back_j_hash = Hash.new(Cmd::HASH_NOT_FOUND)
end

def close_cmd
    Cmd.file&.close
end
  
def add(rd, rs, rt)
    Cmd.set_arg(1, rs)
    Cmd.set_arg(2, rt)
    Cmd.set_arg(3, rd)
    Cmd.set_last(0b00011000, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end

def sub(rd, rs ,rt)
    Cmd.set_arg(1, rs)
    Cmd.set_arg(2, rt)
    Cmd.set_arg(3, rd)
    Cmd.set_last(0b00111001, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end

def or(rd, rs, rt)
    Cmd.set_arg(1, rs)
    Cmd.set_arg(2, rt)
    Cmd.set_arg(3, rd)
    Cmd.set_last(0b00010000, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def bext(rd, rs1, rs2)
    Cmd.set_arg(1, rd)
    Cmd.set_arg(2, rs1)
    Cmd.set_arg(3, rs2)
    Cmd.set_last(0b00001111, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def slti(rt, rs, imm)
    Cmd.set_arg(1, rs)
    Cmd.set_arg(2, rt)
    Cmd.set_last(imm, Cmd::LAST_16_MASK)
    Cmd.set_code(0b00111101)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
# mem is array of 2
def st(rt, mem)
    offset, base = mem  

    Cmd.set_arg(1, base)
    Cmd.set_arg(2, rt)
    Cmd.check_align(offset)
    Cmd.set_last(offset, Cmd::LAST_16_MASK)
    Cmd.set_code(0b00111000)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def ssat(rd, rs, imm5)
    Cmd.set_arg(1, rd)
    Cmd.set_arg(2, rs)
    Cmd.set_arg(3, imm5)
    Cmd.set_code(0b00111111)
    Cmd.emit
    Cmd.instr_counter_incr
end

# mem is array of 2
def ldp(rt1, rt2, mem)
    offset, base = mem
    Cmd.set_arg(1, base)
    Cmd.check_align(offset)
    Cmd.set_last(offset, Cmd::LAST_11_MASK)
    Cmd.set_arg(2, rt1)
    Cmd.set_arg(3, rt2)
    Cmd.set_code(0b00110101)
    Cmd.emit
    Cmd.instr_counter_incr
end

def beq(rs, rt, offset)
    Cmd.set_arg(1, rs)
    Cmd.set_arg(2, rt)
    Cmd.set_last(offset, Cmd::LAST_16_MASK)
    Cmd.set_code(0b00010110)
    Cmd.emit
    Cmd.instr_counter_incr
end

# mem is array of 2
def ld(rt, mem)
    offset, base = mem
    Cmd.set_arg(1, base)
    Cmd.set_arg(2, rt)
    Cmd.check_align(offset)
    Cmd.set_last(offset, Cmd::LAST_16_MASK)
    Cmd.set_code(0b00111110)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def syscall
    Cmd.set_last(0b010101, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def j(index)
    # puts("jump: index: #{index}")
    Cmd.set_last(index, Cmd::LAST_26_MASK)
    Cmd.set_code(0b00110000)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def usat(rd, rs, imm5)
    Cmd.set_arg(1, rd)
    Cmd.set_arg(2, rs)
    Cmd.set_arg(3, imm5)
    Cmd.set_code(0b00100010)
    Cmd.emit
    Cmd.instr_counter_incr
end
  
def clz(rd, rs)
    Cmd.set_arg(1, rd)
    Cmd.set_arg(2, rs)
    Cmd.set_last(0b011100, Cmd::LAST_5_MASK)
    Cmd.set_code(0)
    Cmd.emit
    Cmd.instr_counter_incr
end

