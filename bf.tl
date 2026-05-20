"Simple BrainFuck Interpreter written in TL"

program := "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++."
index := 0
tape := [0]
ptr := 0
chr := 0

while (index < 255)
{
        tape = append(0, tape);
        index = index + 1;
}

index = 0
while (index < len(program))
{
        chr = program[index]
        if (chr is 43)
        {
                tape[ptr] = tape[ptr] + 1
        }
        if (chr is 45)
        {
                tape[ptr] = tape[ptr] - 1
        }
        index = index + 1
}
