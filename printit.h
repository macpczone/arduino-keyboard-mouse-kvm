void printit(const __FlashStringHelper *);
void printit(const String &);
void printit(const char[]);
void printit(char);
void printit(unsigned char, int = DEC);
void printit(int, int = DEC);
void printit(unsigned int, int = DEC);
void printit(long, int = DEC);
void printit(unsigned long, int = DEC);
void printit(double, int = 2);
void printit(const Printable&);

void printit(const __FlashStringHelper *ifsh)
{
    ui.print(ifsh);
    if (noecho == 0) {
    mySerial.print(ifsh);
    }
}

void printit(const String &s)
{
    ui.print(s);
    if (noecho == 0) {
    mySerial.print(s);
    }
}

void printit(const char str[])
{
    ui.print(str);
    if (noecho == 0) {
    mySerial.print(str);
    }
}

void printit(char c)
{
    ui.print(c);
    if (noecho == 0) {
    mySerial.print(c);
    }
}

void printit(unsigned char b, int base)
{
    ui.print(b, base);
    if (noecho == 0) {
    mySerial.print(b, base);
    }
}

void printit(int n, int base)
{
    ui.print(n, base);
    if (noecho == 0) {
    mySerial.print(n, base);
    }
}

void printit(unsigned int n, int base)
{
    ui.print(n, base);
    if (noecho == 0) {
    mySerial.print(n, base);
    }
}

void printit(long n, int base)
{
    ui.print(n, base);
    if (noecho == 0) {
    mySerial.print(n, base);
    }
}

void printit(unsigned long n, int base)
{
    ui.print(n, base);
    if (noecho == 0) {
    mySerial.print(n, base);
    }
}

void printit(double n, int digits)
{
    ui.print(n, digits);
    if (noecho == 0) {
    mySerial.print(n, digits);
    }
}

void printit(const Printable& x)
{
    ui.print(x);
    if (noecho == 0) {
    mySerial.print(x);
    }
}
