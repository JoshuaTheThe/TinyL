min := function (a b) {
        if (a < b) { a }
        else       { b }
}

factorial := function (n) {
        if (n < 2) { 1 }
        else       { n * factorial(n - 1) }
}

wfactorial := function (n) {
        acc := 1;
        while (n > 1)
        {
                acc = acc * n;
                n = n - 1;
        }
        
        acc
}

print("7!: " wfactorial(7));
print("Recursive 7!: " factorial(7))
print("min(2 3): " min(2 3))
print("ABC is [65 66 67]: " if ("ABC" is [65 66 67]) { "true" } else { "false" })
print("11 % 10: " 11 % 10)
print("len(\"abc\"): " len("abc"));
print("capacity(\"abc\"): " capacity("abc"));
print("asarr(\"abc\"): " asarr("abc"));
print("asstr([65,66,67]): " asstr([65,66,67]));
print("append(65 \"Hello, \"): " append(65 "Hello, "));
print("remove(\"Hello, \" 0): " remove("Hello, " 0));
print("find(\"Hello, \" \"e\"[0]): " find("Hello, " "e"[0]));
print("Hello " input("Enter your Name: ") "!")
