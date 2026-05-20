
progress := function (pct) {
        i := 0;
        print("\r[");
        hlf := pct / 2;
        while (i < 50)
        {
                chr := if (i < hlf) { "#" } else { " " };
                print(chr);
                i = i + 1;
        }
        
        print("] " pct "\n");
}

progress(23);
