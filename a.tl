min := function (a b) {
        if (a < b) { a }
        else       { b }
}

factorial := function (n) {
        if (n < 2) { 1 }
        else       { n * factorial(n - 1) }
}

wfactorial := function (n) {
        acc := 1
        while (n > 1)
        {
                n = n - 1
                acc = acc * (n + 1)
                acc
        }
}

makeadder := function () {
        function (a, b) {a + b}
}

funcs := [min, wfactorial]

print("adder: ", makeadder(), "(1,2) => ", makeadder()(1,2), "\n")
print("funcs[1](5) (7!): ", funcs[1](7), "\n");
print("7!: ", wfactorial(7), "\n");
print("Recursive 7!: ", factorial(7), "\n");
print("min(2 3): ", min(2 3), "\n");
print("ABC is [65 66 67]: ", if ("ABC" is [65 66 67]) { "true" } else { "false" }, "\n");
print("11 % 10: ", 11 % 10, "\n");
print("len(\"abc\"): ", len("abc"), "\n");
print("capacity(\"abc\"): ", capacity("abc"), "\n");
print("asarr(\"abc\"): ", asarr("abc"), "\n");
print("asstr([65,66,67]): ", asstr([65,66,67]), "\n");
print("append(65 \"Hello, \"): ", append(65 "Hello, "), "\n");
print("remove(\"Hello, \" 0): ", remove("Hello, " 0), "\n");
print("find(\"Hello, \" \"e\"[0]): ", find("Hello, " "e"[0]), "\n");
print("Hello ", input("Enter your Name: "), "!\n");

function () {
        print("Hello, World\n")
}();
